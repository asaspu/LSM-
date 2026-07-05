#ifndef KVSTORE_H
#define KVSTORE_H

#include "memtable.h"
#include "sstable.h"
#include "wal.h"
#include "compaction.h"
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

class KVStore {
public:
    explicit KVStore(const std::string& dir = "./data");
    ~KVStore();
    
    bool Put(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string* value);
    bool Delete(const std::string& key);
    
    void MinorCompaction();
    void MajorCompaction();
    
private:
    std::string dir_;
    MemTable* mem_;
    WAL* wal_;  // 改用原始指针，避免 make_unique 问题
    std::vector<SSTable*> level0_;
    std::vector<SSTable*> level1_;
    uint64_t next_file_id_;
    size_t memtable_size_threshold_;
    
    void RecoverFromWAL();
    void SwitchMemTable();
};

#endif
