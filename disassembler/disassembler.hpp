#ifndef disassembler_hpp
#define disassembler_hpp

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>

using chunk = std::vector<std::uint8_t>;

struct insn
{
    std::string bytes;
    int length;
};

//unsophisticated and interprets all data as code--fix later
std::string disassembleChunk(const chunk& chunk) noexcept;
insn disassembleInstruction(const chunk& chunk, int& offset) noexcept;

#endif
