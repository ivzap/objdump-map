#include <iostream>
#include <string>
#include "ObjDumpAddressMap.hpp"  // Your header file with the classes

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Provide objdump test file(s) as arguments\n";
        return 1;
    }

    bool all_passed = true;

    for(int i=1; i < argc; i++) {
        std::string filepath = argv[i];
        auto result = ObjDump::ObjDumpParser::parse(filepath);

        std::cout << "Results for file: " << filepath << "\n";

        for(auto& [address, info] : std::get<0>(result)) {
            std::cout << "[Address]: 0x" << std::hex << address
                      << ", [filepath]: " << std::get<2>(result)[info.fileID]
                      << ", [line number]: " << std::dec << info.lineNumber << "\n";
        }

        // Add assertions here if you want, e.g. check map size > 0 etc.
        if(std::get<0>(result).empty()) {
            std::cerr << "Parse failed or empty for " << filepath << "\n";
            all_passed = false;
        }
    }

    return all_passed ? 0 : 1;
}
