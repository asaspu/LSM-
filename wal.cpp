#include "wal.h"
#include <iostream>
#include <cstring>

WAL::WAL(const std::string& filename) : filename_(filename) {
    // 以二进制读写模式打开文件
    file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        // 文件不存在，创建新文件
        file_.clear();
        file_.open(filename_, std::ios::binary | std::ios::out | std::ios::trunc);
        file_.close();
        file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
    }
}

WAL::~WAL() {
    if (file_.is_open()) {
        file_.flush();  // 刷新缓冲区
        file_.close();
    }
}

bool WAL::Append(const std::string& key, const std::string& value) {
    if (!file_.is_open()) {
        return false;
    }
    
    // 记录类型：0 = Put
    uint8_t type = 0;
    uint32_t key_len = key.size();
    uint32_t value_len = value.size();
    
    // 写入记录
    file_.write(reinterpret_cast<const char*>(&type), sizeof(type));
    file_.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    file_.write(key.data(), key_len);
    file_.write(reinterpret_cast<const char*>(&value_len), sizeof(value_len));
    file_.write(value.data(), value_len);
    
    // 立即刷新到磁盘（关键！）
    file_.flush();
    
    // 检查是否写入成功
    if (!file_.good()) {
        std::cerr << "WAL append failed for key: " << key << std::endl;
        return false;
    }
    
    return true;
}

bool WAL::AppendDelete(const std::string& key) {
    if (!file_.is_open()) {
        return false;
    }
    
    // 记录类型：1 = Delete
    uint8_t type = 1;
    uint32_t key_len = key.size();
    
    // 写入删除记录
    file_.write(reinterpret_cast<const char*>(&type), sizeof(type));
    file_.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    file_.write(key.data(), key_len);
    
    // 立即刷新到磁盘（关键！）
    file_.flush();
    
    return file_.good();
}

bool WAL::Replay(std::vector<std::pair<std::string, std::string>>& entries,
                 std::vector<std::string>& deletes) {
    if (!file_.is_open()) {
        return false;
    }
    
    // 移动到文件开头
    file_.seekg(0, std::ios::beg);
    
    while (file_.peek() != EOF) {
        uint8_t type;
        file_.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (file_.eof()) break;
        
        if (type == 0) {  // Put 操作
            uint32_t key_len, value_len;
            
            file_.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            std::string key(key_len, '\0');
            file_.read(&key[0], key_len);
            
            file_.read(reinterpret_cast<char*>(&value_len), sizeof(value_len));
            std::string value(value_len, '\0');
            file_.read(&value[0], value_len);
            
            entries.push_back({key, value});
            
        } else if (type == 1) {  // Delete 操作
            uint32_t key_len;
            
            file_.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            std::string key(key_len, '\0');
            file_.read(&key[0], key_len);
            
            deletes.push_back(key);
        }
    }
    
    // 清除错误状态
    file_.clear();
    
    return true;
}

void WAL::Clear() {
    // 清空WAL文件
    file_.close();
    file_.open(filename_, std::ios::binary | std::ios::out | std::ios::trunc);
    file_.close();
    file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
}
