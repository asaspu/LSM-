CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -pthread
TARGET = lsm_kv

OBJS=compaction.o kvstore.o memtable.o sstable.o wal.o main.o skiplist.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf ./test_data ./perf_data ./recovery_data

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
