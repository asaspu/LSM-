#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "skiplist.h"

class MemTable {
public:
    MemTable();
    ~MemTable();
    
    bool Put(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string* value);
    bool Delete(const std::string& key);
    size_t Size() const { return skiplist_.Size(); }
    void GetAll(std::vector<std::pair<std::string, std::string>>& data);
    bool IsEmpty() const { return skiplist_.Size() == 0; }
    
private:
    SkipList skiplist_;
};

#endif
