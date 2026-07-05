#ifndef WAL_H
#define WAL_H

#include <string>
#include <fstream>
#include <vector>

class WAL {
public:
    explicit WAL(const std::string& filename);
    ~WAL();
    
    bool Append(const std::string& key, const std::string& value);
    bool AppendDelete(const std::string& key);
    bool Replay(std::vector<std::pair<std::string, std::string>>& entries,
                std::vector<std::string>& deletes);
    void Clear();
    
private:
    std::string filename_;
    std::fstream file_;
};

#endif
