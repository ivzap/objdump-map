#include <string>
#include <unordered_map>
#include <cstdint>

struct AddressInfo {
    uint32_t lineNumber = 0;
    uint32_t fileID = 0;
};

namespace std {
    class ObjDumpAddressMap {
        private:
            unordered_map<uint64_t, AddressInfo> addrMap;
            unordered_map<string, uint32_t> fileIDs;
            unordered_map<uint32_t, string> filePaths;
        public:
            string dumpPath;
            ObjDumpAddressMap(string path) : dumpPath(path){
                // TODO: parse objdump
            }

            uint32_t get_pathID(string path){
                return fileIDs[path];
            }

            string get_path(uint32_t fileID){
                return filePaths[fileID];
            }

            AddressInfo& operator[](uint64_t addr) {
                return addrMap[addr];
            }
    };
}