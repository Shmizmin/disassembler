#include <filesystem>
#include <iostream>
#include <fstream>

#include "disassembler.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "[Error] Usage: disassembler (filepath) -options\n");
        return EXIT_FAILURE;
    }
    
    std::string filepath = argv[1];
    const auto ending = filepath.substr(filepath.length() - 2, filepath.length());
    
    if (ending != ".o")
    {
        std::fprintf(stderr, "[Error] File %s does not have the correct extension\n", filepath.c_str());
        return EXIT_FAILURE;
    }
    
    const auto filename = filepath.substr(0, filepath.length() - 2);
    const auto size = std::filesystem::file_size(filepath);
    auto handlein = std::ifstream(filepath, std::ios::in | std::ios::binary);
    
    if (!handlein.good())
    {
        std::fprintf(stderr, "[Error] Could not open file %s for reading\n", filepath.c_str());
        return EXIT_FAILURE;
    }
    
    chunk chunk(size, '\0');
    handlein.read(reinterpret_cast<char*>(&chunk[0]), size);
    
    auto source = disassembleChunk(chunk);
    
    if (argc > 2)
    {
        std::string_view option = argv[2];
        
        if (option == "-stdout")
        {
            std::cout << source;
            return EXIT_SUCCESS;
        }
        
        else
        {
            std::fprintf(stderr, "[Error] Unrecognized launch option %s\nExiting...\n", option.data());
            return EXIT_FAILURE;
        }
    }
    
    else
    {
        auto outfile = filename + ".dis.s";
        
        auto handleout = std::ofstream(outfile, std::ios::out);
        handleout.write(source.data(), source.length());
    }
    
    return EXIT_SUCCESS;
}
