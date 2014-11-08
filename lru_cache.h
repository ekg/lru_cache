// Author: Robert Davis

#ifndef LRU_CACHE_
#define LRU_CACHE_

#include <memory>
#include <utility>

template<typename T, typename U>
class LRUCache {
  public:
    explicit LRUCache(int max_size);
  
    void put(T key, U value);

    std::pair<U, bool> retrieve(T key);

    bool remove(T key);

    void evict_all();

    void print();

    int size() const;

    int max_size() const;

    int hit_count() const;

    int miss_count() const;

  private:
    struct Impl;
    std::auto_ptr<Impl> impl_;

    LRUCache(const LRUCache&);
    void operator=(const LRUCache&);
};

#endif
