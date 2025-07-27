#include <string>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>

struct AddressInfo {
    uint32_t lineNumber = 0;
    uint32_t fileID = 0;
};

bool try_stoui(const std::string& input, int base, uint32_t& output) {
    try {
        std::size_t pos = 0;
        output = std::stoi(input, &pos, base);

        return pos == input.size();
    } catch (...) {
        return false;
    }
}

bool try_stoul(const std::string& input, int base, uint64_t& output) {
    try {
        std::size_t pos = 0;
        output = std::stoi(input, &pos, base);

        return pos == input.size();
    } catch (...) {
        return false;
    }
}

namespace std {

    enum ParseState{
        FOUND_FILENAME,
        LOOKING,
    };

    class ObjDumpParser {
        public:
        vector<string> tokenize_line(string line){
            // split line by empty spaces
            vector<string> tokens;
                    
            stringstream line_stream(line);
            string token;
            while(getline(line_stream, token, ' ')){
                if(!token.empty()){
                    tokens.push_back(token);
                }
            }
            return tokens;
        }

        void parse(
            string path,
            unordered_map<uint64_t, AddressInfo>& addrMap,
            unordered_map<string, uint32_t>& fileIDs,
            unordered_map<uint32_t, string>& filePaths
        ){
            ifstream file(path);
            
            string line, current_filename;
            uint32_t next_fileID = 0;
            ParseState current_state = ParseState::LOOKING;
            
            while(getline(file, line)){
                if(current_state == ParseState::LOOKING){
                    if(line.empty()){
                        continue;
                    }

                    current_state = ParseState::FOUND_FILENAME;

                    // split line by empty spaces
                    vector<string> tokens = tokenize_line(line);
                    if(!tokens.empty()){
                        current_filename = tokens[0];
                    }
                } else {
                    if(line.empty()){
                        current_state = ParseState::LOOKING;
                    } else {
                        vector<string> tokens = tokenize_line(line);
                        uint32_t line_number = 0;
                        uint64_t address = 0xdeadbeeful;
                        if(tokens.size() >= 3 && try_stoui(tokens[1], 10, line_number) && try_stoul(tokens[2], 16, address)){
                            addrMap[address] = AddressInfo{line_number, next_fileID++};
                            filePaths[addrMap[address].fileID] = current_filename;
                            fileIDs[tokens[0]] = addrMap[address].fileID;
                        }
                    }
                }
            }
        }
    };

    class ObjDumpAddressMap {
        private:
            ObjDumpParser p;
            string dumpPath;
            unordered_map<uint64_t, AddressInfo> addrMap;
            unordered_map<string, uint32_t> fileIDs;
            unordered_map<uint32_t, string> filePaths;
        public:
            ObjDumpAddressMap(string path) : dumpPath(path){
                p.parse(path, addrMap, fileIDs, filePaths);
            }

            uint32_t get_pathID(string path){
                return fileIDs[path];
            }

            string get_path(uint32_t fileID){
                return filePaths[fileID];
            }

            const unordered_map<uint64_t, AddressInfo>& get_addr_map(){
                return addrMap;
            }

            void set_pathID(string path, uint32_t value){
                fileIDs[path] = value;
            }

            void set_path(uint32_t fileID, string value){
                filePaths[fileID] = value;
            }

            AddressInfo& operator[](uint64_t addr) {
                return addrMap[addr];
            }

            string get_dump_path(){
                return dumpPath;
            }
    };

    

}

