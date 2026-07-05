#ifndef COMPACTION_H
#define COMPACTION_H

#include <vector>
#include <string>
#include "sstable.h"
#include "memtable.h"

class Compaction {
public:
    static bool DoMinorCompaction(MemTable* mem, 
                                   std::vector<SSTable*>& level0,
                                   uint64_t& next_file_id,
                                   const std::string& dir);
    
    static bool DoMajorCompaction(std::vector<SSTable*>& level0,
                                   std::vector<SSTable*>& level1,
                                   uint64_t& next_file_id,
                                   const std::string& dir);
    
private:
    static void MergeAndWrite(const std::vector<std::pair<std::string, std::string>>& data,
                              uint64_t file_id,
                              const std::string& dir,
                              std::vector<SSTable*>& target_level);
};

#endif
