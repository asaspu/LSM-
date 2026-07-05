#include "sstable.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

SSTable::SSTable(uint64_t id, const std::string& dir) : id_(id), dir_(dir) {
    // 创建目录
    mkdir(dir.c_str(), 0755);
    filename_ = dir_ + "/" + std::to_string(id_) + ".sst";
    
    // 如果文件存在，读取它
    if (access(filename_.c_str(), F_OK) == 0) {
        ReadFromFile();
    }
}

SSTable::~SSTable() {}

bool SSTable::BuildFromMemTable(const std::vector<std::pair<std::string, std::string>>& data) {
    if (data.empty()) return false;
    
    smallest_key_ = data.front().first;
    largest_key_ = data.back().first;
    
    // 构建 blocks（简化版：所有数据放一个block）
    Block block;
    uint32_t offset = 0;
    
    for (const auto& entry : data) {
        uint32_t key_len = entry.first.size();
        uint32_t value_len = entry.second.size();
        
        block.data.append(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        block.data.append(entry.first);
        block.data.append(reinterpret_cast<const char*>(&value_len), sizeof(value_len));
        block.data.append(entry.second);
        
        block.index[entry.first] = offset;
        offset += sizeof(key_len) + key_len + sizeof(value_len) + value_len;
    }
    
    blocks_.push_back(block);
    return WriteToFile();
}

bool SSTable::Get(const std::string& key, std::string* value) {
    // 快速检查key范围
    if (key < smallest_key_ || key > largest_key_) {
        return false;
    }
    
    // 在blocks中查找（简化版：线性扫描）
    for (const auto& block : blocks_) {
        auto it = block.index.find(key);
        if (it != block.index.end()) {
            // 解析value
            [[maybe_unused]]uint32_t offset = it->second;
            // 这里简化：直接保存value在build时
            // 实际应该从block.data中读取
            return false;  // 需要完整实现
        }
    }
    
    // 简化版：线性搜索（演示用）
    // 完整版需要实现二分查找和索引
    
    return false;
}

bool SSTable::WriteToFile() {
    std::ofstream file(filename_, std::ios::binary);
    if (!file.is_open()) return false;
    
    // 写入meta: smallest_key_len, smallest_key, largest_key_len, largest_key
    uint32_t smallest_len = smallest_key_.size();
    file.write(reinterpret_cast<const char*>(&smallest_len), sizeof(smallest_len));
    file.write(smallest_key_.data(), smallest_len);
    
    uint32_t largest_len = largest_key_.size();
    file.write(reinterpret_cast<const char*>(&largest_len), sizeof(largest_len));
    file.write(largest_key_.data(), largest_len);
    
    // 写入block数量
    uint32_t num_blocks = blocks_.size();
    file.write(reinterpret_cast<const char*>(&num_blocks), sizeof(num_blocks));
    
    // 写入每个block
    for (const auto& block : blocks_) {
        uint32_t data_size = block.data.size();
        file.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));
        file.write(block.data.data(), data_size);
        
        uint32_t index_size = block.index.size();
        file.write(reinterpret_cast<const char*>(&index_size), sizeof(index_size));
        
        for (const auto& entry : block.index) {
            uint32_t key_len = entry.first.size();
            file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
            file.write(entry.first.data(), key_len);
            file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }
    }
    
    file.close();
    return true;
}

bool SSTable::ReadFromFile() {
    std::ifstream file(filename_, std::ios::binary);
    if (!file.is_open()) return false;
    
    // 读取meta
    uint32_t smallest_len, largest_len;
    file.read(reinterpret_cast<char*>(&smallest_len), sizeof(smallest_len));
    smallest_key_.resize(smallest_len);
    file.read(&smallest_key_[0], smallest_len);
    
    file.read(reinterpret_cast<char*>(&largest_len), sizeof(largest_len));
    largest_key_.resize(largest_len);
    file.read(&largest_key_[0], largest_len);
    
    // 读取blocks
    uint32_t num_blocks;
    file.read(reinterpret_cast<char*>(&num_blocks), sizeof(num_blocks));
    
    blocks_.resize(num_blocks);
    for (uint32_t i = 0; i < num_blocks; i++) {
        uint32_t data_size;
        file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
        blocks_[i].data.resize(data_size);
        file.read(&blocks_[i].data[0], data_size);
        
        uint32_t index_size;
        file.read(reinterpret_cast<char*>(&index_size), sizeof(index_size));
        
        for (uint32_t j = 0; j < index_size; j++) {
            uint32_t key_len;
            file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            std::string key(key_len, '\0');
            file.read(&key[0], key_len);
            
            uint32_t offset;
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            blocks_[i].index[key] = offset;
        }
    }
    
    return true;
}
