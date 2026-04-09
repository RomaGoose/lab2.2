#pragma once

#include "dynamic_array.hpp"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

template<class T>
class array_sequence {
    private:
        dynamic_array<T> items_;
        size_t count;
    public:
        
        template<bool is_const>
        class base_iterator;
        struct sentinel;

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;

    
        using value_type = T;


        array_sequence(std::initializer_list<T> new_items = {}) : count(new_items.size()), items_(){
            items_.resize(std::bit_ceil(count));
            std::copy_n(new_items.begin(), std::min({items_.size(), new_items.size()}), items_.begin());
        }

        ~array_sequence() = default;

        T& operator[](size_t index){
            return items_[index];
        }
        const T& operator[](size_t index) const {
            return items_[index];
        }

        const T& at(size_t index) const {
            if (index >= count) throw std::out_of_range("index out of range");
            return items_[index];
        }
        
        T& at(size_t index) {
            if (index >= count) throw std::out_of_range("index out of range");
            return items_[index];
        }
        
        const T& first() const {
            if (count == 0) throw std::out_of_range("index out of range");
            return items_.at(0);
        }
        
        T& first() {
            if (count == 0) throw std::out_of_range("index out of range");
            return items_.at(0);
        }
        
        const T& last() const {
            if (count == 0) throw std::out_of_range("index out of range");
            return items_.at(count - 1);
        }
        
        T& last() {
            if (count == 0) throw std::out_of_range("index out of range");
            return items_.at(count - 1);
        }
        
        size_t size() const noexcept {
            return count;
        }

        template<class U>
        array_sequence<T>& append(U&& item) {
            if (count == items_.size()) 
                items_.resize(items_.size() * 2);

            items_[count++] = std::forward<U>(item);
            return *this;
        }
        
        template<class U>
        array_sequence<T>& prepend(U&& item) {
            if(count == items_.size())
                items_.resize(items_.size() * 2);

            std::move_backward(items_.begin(), items_.begin() + count, items_.begin() + count + 1);
            items_[0] = std::forward<U>(item);
            ++count;
            return *this;
        }
        
        template<class U>
        array_sequence<T>& insert_at(size_t index, U&& item) {
            if(index > count) throw std::out_of_range("index out of range");
            if(count == items_.size())
                items_.resize(items_.size() * 2);

            std::move_backward(items_.begin() + index, items_.begin() + count, items_.begin() + count + 1);
            items_[index] = std::forward<U>(item);
            count++;
            return *this;
        }

        void clear() {
            count = 0;
        }

        iterator begin() {
            return iterator(items_.begin());
        }
        iterator end() {
            return iterator(items_.begin() + count);
        }
        const_iterator begin() const {
            return const_iterator(items_.cbegin());
        }
        const_iterator end() const {
            return const_iterator(items_.begin() + count);
        }
        const_iterator cbegin() const {
            return const_iterator(items_.cbegin());
        }
        const_iterator cend() const {
            return const_iterator(items_.begin() + count);
        }

        array_sequence<T> get_subsequence(size_t start, size_t end) const {
            if (start > end || end > size()) throw std::out_of_range("invalid range");
            
            array_sequence<T> tmp;
            tmp.items_.resize(std::bit_ceil(end - start));
            for (auto it = begin() + start; it != begin() + end; ++it) {
                tmp.append(*it);
            }
            return tmp;
        }
};

template<class T>
template<bool is_const>
class array_sequence<T>::base_iterator{
    private: 
        dynamic_array<T>::template base_iterator<is_const> it_inner;
    public:
        using reference = std::conditional_t<is_const, const T&, T&>;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator_category = std::random_access_iterator_tag;

        base_iterator(auto it) : it_inner(it) {};
        base_iterator() : it_inner() {};

        base_iterator& operator++(){
            ++it_inner;
            return *this;
        }
        base_iterator& operator--(){
            --it_inner;
            return *this;
        }
        base_iterator operator++(int){
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        reference operator*() const {
            return *it_inner;
        }

        base_iterator& operator+=(difference_type n) {
            it_inner += n;
            return *this;
        };
        base_iterator operator+(difference_type n) const { 
            auto tmp = *this; 
            tmp += n; 
            return tmp; 
        }
        base_iterator& operator-=(difference_type n) {
            it_inner -= n;
            return *this;
        }
        base_iterator operator-(difference_type n) const { 
            auto tmp = *this; 
            tmp -= n; 
            return tmp; 
        }
        difference_type operator-(const iterator& other) const {
            return it_inner - other.it_inner;
        }
        reference operator[](difference_type n) const { 
            return *(*this + n); 
        }
        auto operator<=>(const base_iterator<is_const>& other) const = default;
};

