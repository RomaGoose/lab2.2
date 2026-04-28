#pragma once

#include "linked_list.hpp"
#include <cstddef>

template<typename  T>
class list_sequence {
    private:
        linked_list<T> items_;

        size_t size_;
    public:
        
        template<bool is_const>
        class base_iterator;
        struct sentinel;

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;

        using value_type = T;
        using reference = T&;
        

        list_sequence(std::initializer_list<T> items = {}) : items_(items), size_(items.size()) {};

        ~list_sequence() = default;

        const T& operator[](size_t index) const {
            auto it = items_.begin();
            for(size_t i = 0; i < index; ++i){
                ++it;
            }
            return *it;
        }

        T& operator[](size_t index) {
            auto it = items_.begin();
            for(size_t i = 0; i < index; ++i){
                ++it;
            }
            return *it;
        }

        const T& at(size_t index) const {
            return items_.at(index);
        }
        
        T& at(size_t index) {
            return items_.at(index);
        }
        
        const T& first() const {
            return items_.first();
        }
        
        T& first() {
            return items_.first();
        }
        
        const T& last() const {
            return items_.last();
        }
        
        T& last() {
            return items_.last();
        }
        
        size_t size() const noexcept {
            return size_;
        }

        template<class U>
        list_sequence<T>& append(U&& item) {
            items_.append(std::forward<U>(item));
            ++size_;
            return *this;
        }

        
        template<class ...Args>
        list_sequence<T>& append(Args&& ...args){
            (append(std::forward<Args>(args)), ...);
            return *this;
        }

        template<class U>
        list_sequence<T>& prepend(U&& item) {
            items_.prepend(std::forward<U>(item));
            ++size_;
            return *this;
        }
        template<class ...Args>
        list_sequence<T>& prepend(Args&& ...args){
            (prepend(std::forward<Args>(args)), ...);
            return *this;
        }

        template<class U>
        list_sequence<T>& insert_at(size_t index, U&& item) {
            items_.insert_at(index, std::forward<U>(item));
            ++size_;
            return *this;
        }

        void clear() { items_.clear(); size_ = 0; }


        iterator begin() {
            return iterator(items_.begin());
        }
        const_iterator begin() const {
            return cbegin();
        }
        sentinel end() const {
            return cend();
        }
        const_iterator cbegin() const {
            return const_iterator(items_.cbegin());
        }
        sentinel cend() const {
            return sentinel();
        }

        list_sequence<T> get_subsequence(size_t start, size_t end) const {
            list_sequence<T> tmp;
            for(auto el: items_.get_sublist(start, end)){
                tmp.append(el);
            }
            return tmp;
        }
};



template<class T>
struct list_sequence<T>::sentinel {
        linked_list<T>::sentinel sen_inner;
    public:
        sentinel() : sen_inner() {};
};


template<class T>
template<bool is_const>
class list_sequence<T>::base_iterator{
    private: 
        using iter = linked_list<T>::template base_iterator<is_const>; 
    
        iter it_inner;
    public:
        using reference = std::conditional_t<is_const, const T&, T&>;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator_category = std::forward_iterator_tag;

        base_iterator(iter it) : it_inner(it) {};
        base_iterator() : it_inner() {};

        base_iterator& operator++(){
            ++it_inner;
            return *this;
        };
        base_iterator operator++(int){
            auto tmp = *this;
            ++(*this);
            return tmp;
        };
        reference operator*() const {
            return *it_inner;
        }
        friend bool operator==(const base_iterator<is_const>& it, sentinel s){
            return it.it_inner == s.sen_inner;
        }
        friend bool operator==(const base_iterator<is_const>& it1, const base_iterator<is_const>& it2){
            return it1.it_inner == it2.it_inner;
        }
};