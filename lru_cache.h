// Copyright (c) 2014 Robert Davis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
