#include "memtable.h"

MemTable::MemTable() {}

MemTable::~MemTable() {}

bool MemTable::Put(const std::string& key, const std::string& value) {
    return skiplist_.Put(key, value);
}

bool MemTable::Get(const std::string& key, std::string* value) {
    return skiplist_.Get(key, value);
}

bool MemTable::Delete(const std::string& key) {
    return skiplist_.Delete(key);
}

void MemTable::GetAll(std::vector<std::pair<std::string, std::string>>& data) {
    skiplist_.GetAll(data);
}
