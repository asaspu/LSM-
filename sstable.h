#ifndef SSTABLE_H
#define SSTABLE_H

#include <string>
#include <vector>
#include <map>
#include <fstream>

class SSTable {
public:
    struct Block {
        std::string data;  // 连续存储的 key-value
        std::map<std::string, uint32_t> index;  // key -> offset in block
    };
    
    SSTable(uint64_t id, const std::string& dir = "./data");
    ~SSTable();
    
    bool BuildFromMemTable(const std::vector<std::pair<std::string, std::string>>& data);
    bool Get(const std::string& key, std::string* value);
    std::string GetSmallestKey() const { return smallest_key_; }
    std::string GetLargestKey() const { return largest_key_; }
    uint64_t GetId() const { return id_; }
    std::string GetFilename() const { return filename_; }
    
private:
    uint64_t id_;
    std::string dir_;
    std::string filename_;
    std::string smallest_key_;
    std::string largest_key_;
    std::vector<Block> blocks_;
    
    bool ReadFromFile();
    bool WriteToFile();
};

#endif
