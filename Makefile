all: test
test: main.cc lru_cache.cc lru_cache.h
	g++ -std=c++11 main.cc lru_cache.cc -o test
