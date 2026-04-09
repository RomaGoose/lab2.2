#pragma once

#include <algorithm>
#include <iterator>

template <class T>
class dynamic_array {
    public:
        template<bool is_const>
        class base_iterator;

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;


        dynamic_array();
        dynamic_array(size_t size_);
        dynamic_array(T* items_ptr, size_t size_);
        dynamic_array(const dynamic_array<T>& other);
        dynamic_array(dynamic_array<T>&& other) noexcept;
        dynamic_array(std::initializer_list<T> init_list);

        ~dynamic_array();

        dynamic_array<T>& operator=(const dynamic_array<T>& other);
        dynamic_array<T>& operator=(dynamic_array<T>&& other) noexcept;
        const T& operator[](size_t index) const noexcept;
        T& operator[](size_t index) noexcept;

        size_t size() const noexcept;

        const T& at(size_t index) const;
        T& at(size_t index);

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        
        void resize(size_t new_size);
    private:
        T* items;
        size_t size_;
};

template <class T>
dynamic_array<T>::dynamic_array() : items(nullptr), size_(0) {}

template <class T>
dynamic_array<T>::dynamic_array(size_t size) : items(new T[size]), size_(size) {}

template <class T>
dynamic_array<T>::dynamic_array(T* items_, size_t size) : items(new T[size]), size_(size) {
    std::copy(items_, items_ + size_, items);
}

template <class T>
dynamic_array<T>::dynamic_array(const dynamic_array<T>& other) : items(nullptr), size_() {
    *this = other;
}
template <class T>
dynamic_array<T>::dynamic_array(dynamic_array<T>&& other) noexcept : items(nullptr), size_() {
    *this = std::move(other);
}
template <class T>
dynamic_array<T>::dynamic_array(std::initializer_list<T> init_list) : items(new T[init_list.size()]), size_(init_list.size()){
    std::copy(std::begin(init_list), std::end(init_list), items);
};


template <class T>
dynamic_array<T>::~dynamic_array() {
    delete [] items;
}


template <class T>
dynamic_array<T>& dynamic_array<T>::operator=(const dynamic_array<T>& other){
    if (this == &other) 
        return *this;
    auto new_items = new T[other.size_];
    std::copy(other.items, other.items + other.size_, new_items);
    
    delete[] items;
    items = new_items;
    size_ = other.size_;
    return *this;
}

template <class T>
dynamic_array<T>& dynamic_array<T>::operator=(dynamic_array<T>&& other) noexcept {
    if (this == &other) 
        return *this;
    
    delete[] items;
    items = other.items;
    size_ = other.size_;

    other.items = nullptr;
    other.size_ = 0;
    return *this;
}

template<class T>
const T& dynamic_array<T>::operator[](size_t index) const noexcept{
    return items[index];
}
template<class T>
T& dynamic_array<T>::operator[](size_t index) noexcept{
    return items[index];
}

template <class T>
size_t dynamic_array<T>::size() const noexcept{
    return size_;    
}

template<class T>
T& dynamic_array<T>::at(size_t index){
    if(index >= size_) 
        throw std::out_of_range("index out of range");
    return items[index];
}
template<class T>
const T& dynamic_array<T>::at(size_t index) const{
    if(index >= size_) 
        throw std::out_of_range("index out of range");
    return items[index];
}

template<class T>
void dynamic_array<T>::resize(size_t new_size){
    if(new_size == size_) return;

    auto new_items = new T[new_size];
    std::move(items, items + std::min({size_, new_size}), new_items);

    delete[] items;
    items = new_items;
    size_ = new_size;
}


template<class T>
template<bool is_const>
class dynamic_array<T>::base_iterator{
    private: 
        T* ptr;
    public:
        using reference = std::conditional_t<is_const, const T&, T&>;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator_category = std::random_access_iterator_tag;

        base_iterator(T* ptr_) : ptr(ptr_) {};

        base_iterator& operator++(){
            ++ptr;
            return *this;
        }
        base_iterator& operator--(){
            --ptr;
            return *this;
        }
        reference operator*() const {
            return *ptr;
        }
        base_iterator& operator+=(difference_type n) {
            ptr += n;
            return *this;
        };
        base_iterator operator+(difference_type n) const { 
            auto tmp = *this; 
            tmp += n; 
            return tmp; 
        }
        base_iterator& operator-=(difference_type n) {
            ptr -= n;
            return *this;
        }
        base_iterator operator-(difference_type n) const { 
            auto tmp = *this; 
            tmp -= n; 
            return tmp; 
        }
        difference_type operator-(const iterator& other) const {
            return ptr - other.ptr;
        }
        reference operator[](difference_type n) const { 
            return *(*this + n); 
        }

        auto operator<=>(const base_iterator<is_const>& other) const = default;
};

template<class T>
dynamic_array<T>::iterator dynamic_array<T>::begin() {
    return iterator(items);
}
template<class T>
dynamic_array<T>::iterator dynamic_array<T>::end() {
    return iterator(items + size_);
}
template<class T>
dynamic_array<T>::const_iterator dynamic_array<T>::begin() const{
    return const_iterator(items);
}
template<class T>
dynamic_array<T>::const_iterator dynamic_array<T>::end() const{
    return const_iterator(items + size_);
}
template<class T>
dynamic_array<T>::const_iterator dynamic_array<T>::cbegin() const{
    return const_iterator(items);
}
template<class T>
dynamic_array<T>::const_iterator dynamic_array<T>::cend() const{
    return const_iterator(items + size_);
}

