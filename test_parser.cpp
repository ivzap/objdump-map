#include <iostream>
#include <string>
#include "ObjDumpAddressMap.hpp"  // Your header file with the classes
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> get_files_in_dir(const std::string& path) {
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }

    return files;
}

int main(int argc, char** argv) {
    bool all_passed = true;
    std::vector<std::string> files = get_files_in_dir("./tests/input");

    for(auto filename: files) {
        auto map = ObjDump::ObjDumpAddressMap("./tests/input/" + filename);
        std::ifstream expected_file("./tests/expect/" + filename);
        
        std::ostringstream test_got, test_expected;

        test_expected << expected_file.rdbuf();

        for (auto& [address, info] : map) {
            test_got << std::hex << address << ", "
                << map.get_path(info.fileID) << std::dec 
                << info.line_number << "\n";
        }

        bool test_result = test_expected.str() == test_got.str();

        if(!test_result){
            printf("\033[31m[%s test failed!]\033[0m\n", filename.c_str());
            printf("got:\n%s\n", test_got.str().c_str());
            printf("expected:\n%s\n", test_expected.str().c_str());
        }

        all_passed &= test_result;
    }
    
    if(all_passed)
        printf("\033[32m[all tests passed!]\033[0m\n");
    else
        printf("\033[31m[some or all tests failed!]\033[0m\n");

    return all_passed;
}
