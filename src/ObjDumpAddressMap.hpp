#include <string>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <tuple>
#include <utility>

#define u_map unordered_map

struct AddressInfo {
    uint32_t line_number = 0;
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
        output = std::stoull(input, &pos, base);

        return pos == input.size();
    } catch (...) {
        return false;
    }
}


namespace ObjDump { 

    using namespace std;

    typedef tuple<
        map<uint64_t, AddressInfo>, 
        u_map<string, uint32_t>, 
        u_map<uint32_t, string>
    > ParseResult;

    enum ParseState {
        FOUND_FILENAME,
        LOOKING,
    };

    class ObjDumpParser {
        public:
            static vector<string> tokenize_line(string line){
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

            static ParseResult parse(string path){

                ParseResult result;

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
                        if(tokens.size() == 1){
                            // filenames always have only one token on their line
                            current_filename = tokens[0];
                        } else {
                            current_state = ParseState::LOOKING;
                        }
                    } else {
                        if(line.empty()){
                            current_state = ParseState::LOOKING;
                        } else {
                            vector<string> tokens = tokenize_line(line);
                            uint32_t line_number = 0;
                            uint64_t address = 0xdeadbeeful;
                            if(tokens.size() >= 3 && try_stoui(tokens[1], 10, line_number) && try_stoul(tokens[2], 16, address)){
                                get<0>(result)[address] = AddressInfo{line_number, next_fileID++};
                                get<1>(result)[current_filename] = get<0>(result)[address].fileID;
                                get<2>(result)[get<0>(result)[address].fileID] = current_filename;
                            }
                        }
                    }
                }
                return result;
            }
    };

    class ObjDumpAddressMap {
        private:
            string dumpPath;
            map<uint64_t, AddressInfo> addrMap;
            u_map<string, uint32_t> fileIDs;
            u_map<uint32_t, string> filePaths;
        public:
            ObjDumpAddressMap() = default;

            ObjDumpAddressMap(string path) : dumpPath(path){
                auto result = ObjDumpParser::parse(path);
                tie(addrMap, fileIDs, filePaths) = move(result);
            }

            uint32_t get_pathID(string path){
                return fileIDs[path];
            }

            string get_path(uint32_t fileID){
                return filePaths[fileID];
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

            bool contains(uint64_t addr){
                return addrMap.count(addr) > 0;
            }

            auto begin() const { return addrMap.begin(); }

            auto end() const { return addrMap.end(); }
    };
}

