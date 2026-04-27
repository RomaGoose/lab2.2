#pragma once

#include "dynamic_array.hpp"

template<class T>
class array_sequence {
    private:
        dynamic_array<T> items_;
        size_t size_;
    public:
        
        template<bool is_const>
        class base_iterator;

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;

    
        using value_type = T;
        using reference = T&;


        array_sequence(std::initializer_list<T> new_items = {}) : size_(new_items.size()), items_(std::bit_ceil(new_items.size())){
            std::copy_n(new_items.begin(), new_items.size(), items_.begin());
        }

        ~array_sequence() = default;

        T& operator[](size_t index){
            return items_[index];
        }
        const T& operator[](size_t index) const {
            return items_[index];
        }

        const T& at(size_t index) const {
            if (index >= size_) throw std::out_of_range("index out of range");
            return items_[index];
        }
        
        T& at(size_t index) {
            if (index >= size_) throw std::out_of_range("index out of range");
            return items_[index];
        }
        
        const T& first() const {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return items_.at(0);
        }
        
        T& first() {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return items_.at(0);
        }
        
        const T& last() const {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return items_.at(size_ - 1);
        }
        
        T& last() {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return items_.at(size_ - 1);
        }
        
        size_t size() const noexcept {
            return size_;
        }

        template<class U>
        array_sequence<T>& append(U&& item) {
            if (size_ == items_.size()) 
                items_.resize(items_.size() * 2);

            items_[size_++] = std::forward<U>(item);
            return *this;
        }
        
        template<class ...Args>
        array_sequence<T>& append(Args&& ...args){
            (append(std::forward<Args>(args)), ...);
            return *this;
        }


        template<class U>
        array_sequence<T>& prepend(U&& item) {
            if(size_ == items_.size())
                items_.resize(items_.size() * 2);

            std::move_backward(items_.begin(), items_.begin() + size_, items_.begin() + size_ + 1);
            items_[0] = std::forward<U>(item);
            ++size_;
            return *this;
        }

        template<class ...Args>
        array_sequence<T>& prepend(Args&& ...args){
            (prepend(std::forward<Args>(args)), ...);
            return *this;
        }
        

        template<class U>
        array_sequence<T>& insert_at(size_t index, U&& item) {
            if(index > size_) throw std::out_of_range("index out of range");
            if(size_ == items_.size())
                items_.resize(items_.size() * 2);

            std::move_backward(items_.begin() + index, items_.begin() + size_, items_.begin() + size_ + 1);
            items_[index] = std::forward<U>(item);
            size_++;
            return *this;
        }

        void clear() {
            size_ = 0;
        }

        iterator begin() {
            return iterator(items_.begin());
        }
        iterator end() {
            return iterator(items_.begin() + size_);
        }
        const_iterator begin() const {
            return cbegin();
        }
        const_iterator end() const {
            return cend();
        }
        const_iterator cbegin() const {
            return const_iterator(items_.cbegin());
        }
        const_iterator cend() const {
            return const_iterator(items_.begin() + size_);
        }

        array_sequence<T> get_subsequence(size_t start, size_t end) const {
            if (start > end || end > size_) throw std::out_of_range("invalid range");
            
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
        using iter = dynamic_array<T>::template base_iterator<is_const>;
        
        iter it_inner;
    public:
        using reference = std::conditional_t<is_const, const T&, T&>;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator_category = std::random_access_iterator_tag;

        base_iterator(iter it) : it_inner(it) {};
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
        reference operator*() const { //TODO: mutable 
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

