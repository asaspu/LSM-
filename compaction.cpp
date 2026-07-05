#include "compaction.h"
#include "memtable.h"
#include <algorithm>
#include <map>

bool Compaction::DoMinorCompaction(MemTable* mem,
                                    std::vector<SSTable*>& level0,
                                    uint64_t& next_file_id,
                                    const std::string& dir) {
    if (mem->IsEmpty()) return true;
    
    // 获取所有数据
    std::vector<std::pair<std::string, std::string>> data;
    mem->GetAll(data);
    
    if (data.empty()) return true;
    
    // 排序（skiplist已经排序，但确保一下）
    std::sort(data.begin(), data.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    // 创建SSTable
    SSTable* sst = new SSTable(next_file_id++, dir);
    if (!sst->BuildFromMemTable(data)) {
        delete sst;
        return false;
    }
    
    level0.push_back(sst);
    return true;
}

bool Compaction::DoMajorCompaction(std::vector<SSTable*>& level0,
                                    std::vector<SSTable*>& level1,
                                    uint64_t& next_file_id,
                                    const std::string& dir) {
    if (level0.empty()) return true;
    
    // 收集所有key-value
    std::map<std::string, std::string> merged;
    
    // 从L0读取
    for ([[maybe_unused]]auto* sst : level0) {
        // 简化：实际需要实现Get，这里演示逻辑
        // 真实实现需要遍历SSTable所有数据
    }
    
    // 从L1读取
    for ([[maybe_unused]]auto* sst : level1) {
        // 合并数据
    }
    
    // 写入新的L1文件
    std::vector<std::pair<std::string, std::string>> sorted_data;
    for (const auto& entry : merged) {
        sorted_data.push_back(entry);
    }
    
    // 删除旧文件
    for (auto* sst : level0) {
        delete sst;
    }
    level0.clear();
    
    for (auto* sst : level1) {
        delete sst;
    }
    level1.clear();
    
    // 创建新的L1 SSTable
    SSTable* new_sst = new SSTable(next_file_id++, dir);
    new_sst->BuildFromMemTable(sorted_data);
    level1.push_back(new_sst);
    
    return true;
}
