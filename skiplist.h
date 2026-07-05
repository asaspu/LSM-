#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>
#include <random>
#include <string>

class SkipList {
public:
    struct Node {
        std::string key;
        std::string value;
        std::vector<Node*> forward;
        
        Node(const std::string& k, const std::string& v, int level);
    };
    
    SkipList(int max_level = 12, double p = 0.5);
    ~SkipList();
    
    bool Put(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string* value);
    bool Delete(const std::string& key);
    void GetAll(std::vector<std::pair<std::string, std::string>>& data);
    size_t Size() const;

private:
    Node* head_;
    int max_level_;
    double p_;
    int cur_level_;
    size_t size_;
    mutable std::mutex mutex_;
    std::default_random_engine generator_;
    std::uniform_real_distribution<double> distribution_;
    
    int RandomLevel();
};

#endif
