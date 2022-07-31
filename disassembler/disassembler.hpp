#ifndef disassembler_hpp
#define disassembler_hpp

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>

namespace ti
{
    using chunk = std::vector<std::uint8_t>;
    
    struct insn
    {
        std::string bytes;
        int length;
    };
    
    //unsophisticated and interprets all data as code--fix later
    std::string disassembleChunk(const ti::chunk& chunk) noexcept;
    ti::insn disassembleInstruction(const ti::chunk& chunk, int& offset) noexcept;
}

#endif
