#include "disassembler.hpp"

#include "opcode.h"

namespace
{
    std::uint8_t read8(const chunk& chunk, int& offset)
    {
        //optimize
        auto val = chunk[offset];
        offset++;
        return val;
    }
    
    std::uint16_t read16(const chunk& vec, int& offset)
    {
        auto val1 = read8(vec, offset);
        auto val2 = read8(vec, offset);
        
        return std::uint16_t((val2 << 8) | val1);
    }
    
    template<typename I>
    std::string hx(I w, std::size_t hex_len = sizeof(I) << 1)
    {
        static const char* digits = "0123456789abcdef";
        std::string rc(hex_len, '0');
        
        for (std::size_t i = 0, j = (hex_len - 1) * 4 ; i < hex_len; ++i, j -= 4)
            rc[i] = digits[(w >> j) & 0x0f];
        
        return rc;
    }
} //anonymous namespace


std::string disassembleChunk(const chunk& chunk) noexcept
{
    std::string out;
    
    for (auto index = 0x8000; index < chunk.size();)
    {
        const auto&&[insn, offset] = disassembleInstruction(chunk, index);
        out += insn;
        out += "\n";
    }

    return out;
}

insn disassembleInstruction(const chunk& chunk, int& offset) noexcept
{
#define STRINTERN(x) #x
#define STR(x) std::string(STRINTERN(x))
    
#define BIGINSN(o, t)                                                            \
    case o##_A_B: return { STR(t r0) + ", " + STR(r1), 1 };                      \
    case o##_A_C: return { STR(t r0) + ", " + STR(r2), 1 };                      \
    case o##_A_D: return { STR(t r0) + ", " + STR(r3), 1 };                      \
    case o##_A_IMM: return { STR(t r0) + ", #" + hx(read8(chunk, offset)), 2 };  \
    case o##_A_MEM: return { STR(t r0) + ", &" + hx(read16(chunk, offset)), 3 }; \
                                                                                 \
    case o##_B_A: return { STR(t r1) + ", " + STR(r0), 1 };                      \
    case o##_B_C: return { STR(t r1) + ", " + STR(r2), 1 };                      \
    case o##_B_D: return { STR(t r1) + ", " + STR(r3), 1 };                      \
    case o##_B_IMM: return { STR(t r1) + ", #" + hx(read8(chunk, offset)), 2 };  \
    case o##_B_MEM: return { STR(t r1) + ", &" + hx(read16(chunk, offset)), 3 }; \
                                                                                 \
    case o##_C_A: return { STR(t r2) + ", " + STR(r0), 1 };                      \
    case o##_C_B: return { STR(t r2) + ", " + STR(r1), 1 };                      \
    case o##_C_D: return { STR(t r2) + ", " + STR(r3), 1 };                      \
    case o##_C_IMM: return { STR(t r2) + ", #" + hx(read8(chunk, offset)), 2 };  \
    case o##_C_MEM: return { STR(t r2) + ", &" + hx(read16(chunk, offset)), 3 }; \
                                                                                 \
    case o##_D_A: return { STR(t r3) + ", " + STR(r0), 1 };                      \
    case o##_D_B: return { STR(t r3) + ", " + STR(r1), 1 };                      \
    case o##_D_C: return { STR(t r3) + ", " + STR(r2), 1 };                      \
    case o##_D_IMM: return { STR(t r3) + ", #" + hx(read8(chunk, offset)), 2 };  \
    case o##_D_MEM: return { STR(t r3) + ", &" + hx(read16(chunk, offset)), 3 }; \
    
#define NOTINSN(r, t)                       \
    case NOT_##r: return { STR(not t), 1 }; \
    
#define ROXINSN(d, r, t)                                                                      \
    case RO##d##_##r##_IMM: return { STR(ro##d r##t) + ", #" + hx(read8(chunk, offset)), 2 }; \
    
#define LDBINSN(x, t)                                                                    \
    case LDB_##x##_IMM: return { STR(ldb r##t) + ", #" + hx(read8(chunk, offset)), 2 };  \
    case LDB_##x##_MEM: return { STR(ldb r##t) + ", &" + hx(read16(chunk, offset)), 3 }; \
    
#define STBINSN(x, t)                                                                           \
    case STB_MEM_##x:  return { STR(stb &) + hx(read16(chunk, offset)) + ", " + STR(r##t), 3 }; \
    
#define MVBINSN(r1, r2, t1, t2)                                       \
    case MVB_##r1##_##r2: return { STR(mvb t1) + ", " + STR(t2), 1 }; \
    
#define JMPINSN(i, o)                                                 \
    case i##_MEM: return { STR(o &) + hx(read16(chunk, offset)), 3 }; \
   
#define STACKINSN(o, r, l, t)             \
    case o##_##r: return { STR(l t), 1 }; \
    
    switch (auto insn = read8(chunk, offset))
    {
        case NOP: return { "nop", 1 };
        case BRK: return { "brk", 1 };
        
        BIGINSN(ADC, adc)
        BIGINSN(SBB, sbb)
        BIGINSN(LOR, or)
        BIGINSN(AND, and)
            
        NOTINSN(A, r0)
        NOTINSN(B, r1)
        NOTINSN(C, r2)
        NOTINSN(D, r3)
            
        ROXINSN(L, A, 0)
        ROXINSN(L, B, 1)
        ROXINSN(L, C, 2)
        ROXINSN(L, D, 3)
            
        ROXINSN(R, A, 0)
        ROXINSN(R, B, 1)
        ROXINSN(R, C, 2)
        ROXINSN(R, D, 3)
            
        LDBINSN(A, 0)
        LDBINSN(B, 1)
        LDBINSN(C, 2)
        LDBINSN(D, 3)
            
        STBINSN(A, 0)
        STBINSN(B, 1)
        STBINSN(C, 2)
        STBINSN(D, 3)
            
        case STB_MEM_IMM: return { STR(stb &) + hx(read16(chunk, offset)) + ", #" + hx(read8(chunk, offset)), 4 };
             
        MVBINSN(A, B, r0, r1)
        MVBINSN(A, C, r0, r2)
        MVBINSN(A, D, r0, r3)
        MVBINSN(A, F, r0, flags)
            
        MVBINSN(B, A, r1, r0)
        MVBINSN(B, C, r1, r2)
        MVBINSN(B, D, r1, r3)
        MVBINSN(B, F, r1, flags)
            
        MVBINSN(C, A, r2, r0)
        MVBINSN(C, B, r2, r1)
        MVBINSN(C, D, r2, r3)
        MVBINSN(C, F, r2, flags)
            
        MVBINSN(D, A, r3, r0)
        MVBINSN(D, B, r3, r1)
        MVBINSN(D, C, r3, r2)
        MVBINSN(D, F, r3, flags)
            
        MVBINSN(F, A, flags, r0)
        MVBINSN(F, B, flags, r1)
        MVBINSN(F, C, flags, r2)
        MVBINSN(F, D, flags, r3)
        
        JMPINSN(JEZ, jez)
        JMPINSN(JGZ, jgz)
        JMPINSN(JCS, jcs)
        
        STACKINSN(PUSH, A, push, r0)
        STACKINSN(PUSH, B, push, r1)
        STACKINSN(PUSH, C, push, r2)
        STACKINSN(PUSH, D, push, r3)
        STACKINSN(PUSH, F, push, flags)
        STACKINSN(PUSH, IP, push, ip)
        
        STACKINSN(POP, A, pop, r0)
        STACKINSN(POP, B, pop, r1)
        STACKINSN(POP, C, pop, r2)
        STACKINSN(POP, D, pop, r3)
        STACKINSN(POP, F, pop, flags)
        STACKINSN(POP, IP, pop, ip)
        STACKINSN(POP, DISCARD, pop, discard)

        case PUSH_IMM: return { STR(push #) + hx(read8(chunk, offset)), 2 };
        case PUSH_MEM: return { STR(push &) + hx(read16(chunk, offset)), 3 };
            
        case DEREF_AB_A: return { "deref r0-1, r0", 1 };
        case DEREF_CD_C: return { "deref r2-3, r2", 1 };
        
        default: std::fprintf(stderr, "[Error] Illegal instruction %i was found\n", insn); std::exit(1);
    }
}
