#include "kvstore.h"
#include <iostream>
#include <filesystem>
#include <cstring>

KVStore::KVStore(const std::string& dir) 
    : dir_(dir), mem_(new MemTable()), wal_(nullptr),
      next_file_id_(0), memtable_size_threshold_(4 * 1024 * 1024) {
    
    // 创建目录
    std::filesystem::create_directories(dir);
    
    // 初始化WAL（使用 new 而不是 make_unique）
    wal_ = new WAL(dir_ + "/wal.log");
    
    // 从WAL恢复数据
    RecoverFromWAL();
}

KVStore::~KVStore() {
    // 刷盘
    if (!mem_->IsEmpty()) {
        MinorCompaction();
    }
    
    delete mem_;
    delete wal_;  // 释放WAL
    
    for (auto* sst : level0_) delete sst;
    for (auto* sst : level1_) delete sst;
}

bool KVStore::Put(const std::string& key, const std::string& value) {
    // 1. 先写WAL（保证崩溃恢复）
    if (!wal_->Append(key, value)) {
        return false;
    }
    
    // 2. 再写MemTable
    mem_->Put(key, value);
    
    // 3. 检查是否需要刷盘
    if (mem_->Size() > memtable_size_threshold_) {
        SwitchMemTable();
    }
    
    return true;
}

bool KVStore::Get(const std::string& key, std::string* value) {
    // 1. 查MemTable
    if (mem_->Get(key, value)) {
        return true;
    }
    
    // 2. 查L0（从新到旧）
    for (auto it = level0_.rbegin(); it != level0_.rend(); ++it) {
        if ((*it)->Get(key, value)) {
            return true;
        }
    }
    
    // 3. 查L1
    for (auto* sst : level1_) {
        if (sst->Get(key, value)) {
            return true;
        }
    }
    
    return false;
}

bool KVStore::Delete(const std::string& key) {
    // 1. 写WAL删除记录
    if (!wal_->AppendDelete(key)) {
        return false;
    }
    
    // 2. 删除MemTable中的记录
    return mem_->Delete(key);
}

void KVStore::MinorCompaction() {
    if (mem_->IsEmpty()) return;
    
    // 将MemTable刷到L0
    Compaction::DoMinorCompaction(mem_, level0_, next_file_id_, dir_);
    
    // 清空MemTable
    delete mem_;
    mem_ = new MemTable();
    
    // 清空WAL
    wal_->Clear();
    
    // 如果L0文件太多，触发Major Compaction
    if (level0_.size() >= 4) {
        MajorCompaction();
    }
}

void KVStore::MajorCompaction() {
    // 合并L0和L1
    Compaction::DoMajorCompaction(level0_, level1_, next_file_id_, dir_);
}

void KVStore::SwitchMemTable() {
    if (!mem_->IsEmpty()) {
        MinorCompaction();
    }
}

void KVStore::RecoverFromWAL() {
    std::vector<std::pair<std::string, std::string>> entries;
    std::vector<std::string> deletes;
    
    // 读取WAL
    if (!wal_->Replay(entries, deletes)) {
        return;
    }
    
    // 重放写入操作
    for (const auto& entry : entries) {
        mem_->Put(entry.first, entry.second);
    }
    
    // 重放删除操作
    for (const auto& key : deletes) {
        mem_->Delete(key);
    }
    
    if (entries.size() > 0 || deletes.size() > 0) {
        std::cout << "Recovered " << entries.size() << " entries, " 
                  << deletes.size() << " deletes from WAL" << std::endl;
    }
}
