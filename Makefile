all: test
test: main.cc lru_cache.h
	$(CXX) -std=c++11 main.cc -o test
clean:
	rm test
