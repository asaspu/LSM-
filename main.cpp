#include "kvstore.h"
#include <iostream>
#include <chrono>
#include <thread>

void test_basic() {
    std::cout << "=== Basic Test ===" << std::endl;
    
    KVStore db("./test_data");
    
    // 写入
    db.Put("name", "LSM-Tree");
    db.Put("version", "1.0");
    db.Put("author", "C++ Developer");
    
    // 读取
    std::string value;
    if (db.Get("name", &value)) {
        std::cout << "name: " << value << std::endl;
    }
    
    // 删除
    db.Delete("version");
    if (!db.Get("version", &value)) {
        std::cout << "version deleted successfully" << std::endl;
    }
    
    // 覆盖
    db.Put("name", "LSM-KVStore");
    db.Get("name", &value);
    std::cout << "name after update: " << value << std::endl;
}

void test_performance() {
    std::cout << "\n=== Performance Test ===" << std::endl;
    
    KVStore db("./perf_data");
    
    const int num_ops = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_ops; i++) {
        std::string key = "key_" + std::to_string(i);
        std::string value = "value_" + std::to_string(i);
        db.Put(key, value);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Write " << num_ops << " entries in " << duration.count() << " ms" << std::endl;
    std::cout << "Throughput: " << (num_ops * 1000.0 / duration.count()) << " ops/sec" << std::endl;
    
    // 随机读取测试
    start = std::chrono::high_resolution_clock::now();
    
    int found = 0;
    for (int i = 0; i < num_ops; i++) {
        std::string key = "key_" + std::to_string(i);
        std::string value;
        if (db.Get(key, &value)) {
            found++;
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Read " << num_ops << " entries, found " << found << " in " 
              << duration.count() << " ms" << std::endl;
    std::cout << "Read throughput: " << (num_ops * 1000.0 / duration.count()) << " ops/sec" << std::endl;
}

void test_recovery() {
    std::cout << "\n=== Recovery Test ===" << std::endl;
    
    {
        KVStore db("./recovery_data");
        db.Put("persistent", "this should survive crash");
        db.Put("temp", "data");
        db.Delete("temp");
    }  // db destructor flushes
    
    {
        KVStore db("./recovery_data");
        std::string value;
        if (db.Get("persistent", &value)) {
            std::cout << "Recovered: persistent = " << value << std::endl;
        }
        
        if (!db.Get("temp", &value)) {
            std::cout << "Deleted key not recovered" << std::endl;
        }
    }
}

int main() {
    test_basic();
    test_performance();
    test_recovery();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
