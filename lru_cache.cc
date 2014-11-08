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

#include "lru_cache.h"

#include <iostream>
#include <mutex>
#include <unordered_map>

template<typename T, typename U>
struct LRUCache<T, U>::Impl {
    class Node;

    using map_type = std::unordered_map<T, Node*>;

    struct Node {
        Node(U data) : data(data), prev(NULL), next(NULL) {}

        U data;
        Node* prev;
        Node* next;
        typename map_type::iterator map_it;
    };

    Impl(int max_size) : max_size_(max_size), size_(0),
                         list_head_(NULL), list_tail_(NULL),
                         hit_count_(0), miss_count_(0) {
    }

    void put(T key, U value);

    std::pair<U, bool> retrieve(T key);

    bool remove(T key);

    void evict_all();

    void print();

    int size() const { return size_; }

    int max_size() const { return max_size_; }

    int hit_count() const { return hit_count_; }

    int miss_count() const { return miss_count_; }

    void bump_to_front(Node* node);

    map_type hashmap_;
    Node* list_head_;
    Node* list_tail_;
    int size_;
    int max_size_;

    int hit_count_;
    int miss_count_;

    std::mutex mutex_;
};

template<typename T, typename U>
LRUCache<T, U>::LRUCache(int max_size) : impl_(new Impl(max_size)) {}

template<typename T, typename U>
void LRUCache<T, U>::put(T key, U value) { impl_->put(key, value); }

template<typename T, typename U>
std::pair<U, bool> LRUCache<T, U>::retrieve(T key) { return impl_->retrieve(key); }

template<typename T, typename U>
bool LRUCache<T, U>::remove(T key) { return impl_->remove(key); }

template<typename T, typename U>
void LRUCache<T, U>::evict_all() { impl_->evict_all(); }

template<typename T, typename U>
void LRUCache<T, U>::print() { impl_->print(); }

template<typename T, typename U>
int LRUCache<T, U>::size() const { return impl_->size_; }

template<typename T, typename U>
int LRUCache<T, U>::max_size() const { return impl_->max_size_; }

template<typename T, typename U>
int LRUCache<T, U>::hit_count() const { return impl_->hit_count_; }

template<typename T, typename U>
int LRUCache<T, U>::miss_count() const { return impl_->miss_count_; }


// Private Implementation:
template<typename T, typename U>
void LRUCache<T, U>::Impl::put(T key, U value) {
    std::lock_guard<std::mutex> lock(mutex_);

    Node* new_node = new Node(value);

    auto it = hashmap_.insert(typename map_type::value_type(key, new_node));
        
    if (!it.second) {
        delete it.first->second;
        it.first->second = new_node;
        bump_to_front(it.first->second);
        return;
    }

    new_node->map_it = it.first;

    if (list_head_ == NULL) {
        list_head_ = list_tail_ = new_node;
    } else {
        list_head_->prev = new_node;
        new_node->next = list_head_;
        list_head_ = new_node;
    }
        
    size_++;
        
    if (size_ > max_size_) {
        list_tail_ = list_tail_->prev;
        hashmap_.erase(list_tail_->next->map_it);
        delete list_tail_->next;
        list_tail_->next = NULL;
        size_--;
    }
}

template<typename T, typename U>
std::pair<U, bool> LRUCache<T, U>::Impl::retrieve(T key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = hashmap_.find(key);

    if (it == hashmap_.end()) {
        miss_count_++;
        return std::pair<U, bool>(U(), false);
    }

    miss_count_++;

    Node* node = it->second;

    bump_to_front(node);

    return std::pair<U, bool>(node->data, true);
}

template<typename T, typename U>
bool LRUCache<T, U>::Impl::remove(T key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = hashmap_.find(key);

    if (it == hashmap_.end())
        return false;

    Node* node = it->second;

    bump_to_front(node);

    list_head_ = list_head_->next;

    if (list_head_ != NULL)
        list_head_->prev = NULL;

    hashmap_.erase(node->map_it);
    delete node;

    size_--;

    return true;
}

template<typename T, typename U>
void LRUCache<T, U>::Impl::evict_all() {
    std::lock_guard<std::mutex> lock(mutex_);

    Node* cur = list_head_;
    while (cur != NULL) {
        hashmap_.erase(cur->map_it);
        Node* save = cur;
        cur = cur->next;
        delete save;
    }
    list_head_ = list_tail_ = NULL;
}

template<typename T, typename U>
void LRUCache<T, U>::Impl::print() {
    std::lock_guard<std::mutex> lock(mutex_);

    Node* cur = list_head_;
    std::cout << "-----" << std::endl << "CACHE STATE:" << std::endl;
    while (cur != NULL) {
        std::cout << cur->data << " ";
        cur = cur->next;
    }
    std::cout << std::endl << "-----" << std::endl;
}

template<typename T, typename U>
void LRUCache<T, U>::Impl::bump_to_front(Node* node) {
    if (list_head_ == node)
        return;

    list_head_->prev = node;

    if (node != list_tail_)
        node->next->prev = node->prev;
    else
        list_tail_ = list_tail_->prev;

    node->prev->next = node->next;

    node->prev = NULL;
    node->next = list_head_;

    list_head_ = node;
}
