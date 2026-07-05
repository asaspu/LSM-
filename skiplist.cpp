#include "skiplist.h"

using namespace std;

SkipList::Node::Node(const string& k, const string& v, int level)
    : key(k), value(v), forward(level + 1, nullptr) {}

SkipList::SkipList(int max_level, double p)
    : max_level_(max_level), p_(p), cur_level_(0), size_(0),
      distribution_(0.0, 1.0) {
    head_ = new Node("", "", max_level_);
}

SkipList::~SkipList() {
    Node* curr = head_;
    while (curr) {
        Node* next = curr->forward[0];
        delete curr;
        curr = next;
    }
}

int SkipList::RandomLevel() {
    int level = 0;
    while (level < max_level_ && distribution_(generator_) < p_) {
        level++;
    }
    return level;
}

bool SkipList::Put(const string& key, const string& value) {
    lock_guard<mutex> lock(mutex_);
    
    Node* curr = head_;
    vector<Node*> update(max_level_ + 1, nullptr);
    
    for (int i = cur_level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }
    curr = curr->forward[0];
    
    if (curr && curr->key == key) {
        curr->value = value;
        return true;
    }
    
    int new_level = RandomLevel();
    if (new_level > cur_level_) {
        for (int i = cur_level_ + 1; i <= new_level; i++) {
            update[i] = head_;
        }
        cur_level_ = new_level;
    }
    
    Node* new_node = new Node(key, value, new_level);
    for (int i = 0; i <= new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    
    size_++;
    return true;
}

bool SkipList::Get(const string& key, string* value) {
    lock_guard<mutex> lock(mutex_);
    
    Node* curr = head_;
    for (int i = cur_level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
    }
    curr = curr->forward[0];
    
    if (curr && curr->key == key) {
        *value = curr->value;
        return true;
    }
    return false;
}

bool SkipList::Delete(const string& key) {
    lock_guard<mutex> lock(mutex_);
    
    Node* curr = head_;
    vector<Node*> update(max_level_ + 1, nullptr);
    
    for (int i = cur_level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }
    curr = curr->forward[0];
    
    if (!curr || curr->key != key) {
        return false;
    }
    
    for (int i = 0; i <= cur_level_; i++) {
        if (update[i]->forward[i] != curr) break;
        update[i]->forward[i] = curr->forward[i];
    }
    
    while (cur_level_ > 0 && head_->forward[cur_level_] == nullptr) {
        cur_level_--;
    }
    
    delete curr;
    size_--;
    return true;
}

void SkipList::GetAll(vector<pair<string, string>>& data) {
    lock_guard<mutex> lock(mutex_);
    Node* curr = head_->forward[0];
    while (curr) {
        data.push_back({curr->key, curr->value});
        curr = curr->forward[0];
    }
}

size_t SkipList::Size() const {
    return size_;
}
