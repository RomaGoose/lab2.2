#pragma once

#include <memory>


template<class T>
class linked_list{
    public:
        template<bool is_const>
        class base_iterator;
        struct sentinel {};

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;
        
        linked_list();
        linked_list(T* items, size_t size_);
        linked_list(const linked_list<T>& other);
        linked_list(std::initializer_list<T> init_list);

        ~linked_list() = default;

        linked_list<T>& operator=(const linked_list<T>& other);
        linked_list<T> operator+(const linked_list<T>& other) const;
        linked_list<T>& operator+=(const linked_list<T>& other);
        
        

        const T& first() const;
        const T& last() const;
        const T& at(size_t index) const;
        T& first();
        T& last();
        T& at(size_t index);
        size_t size() const noexcept;

        iterator begin();

        const_iterator begin() const;
        sentinel end() const;
        const_iterator cbegin() const;
        sentinel cend() const;

        
        template<class U> 
        void append(U&& item);
        template<class U> 
        void prepend(U&& item);
        template<class U> 
        void insert_at(size_t index, U&& item);
        void clear() noexcept;

        linked_list<T> get_sublist(size_t start_index, size_t end_index) const;
    
    private:
        struct node;

        std::shared_ptr<node> head;
        std::weak_ptr<node> tail;
        size_t size_;

        std::shared_ptr<node> get_node(size_t index);
        std::shared_ptr<node> get_node(size_t index) const;
};

template<class T>
template<bool is_const>
class linked_list<T>::base_iterator{
    private: 
        std::weak_ptr<node> ptr;
    public:
        using reference = std::conditional_t<is_const, const T&, T&>;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator_category = std::forward_iterator_tag;
    
        base_iterator(std::shared_ptr<node> ptr_) : ptr(ptr_) {};

        base_iterator& operator++(){
            ptr = ptr.lock()->next;
            return *this;
        };
        reference operator*() const {
            return ptr.lock()->value;
        }
        friend bool operator==(const base_iterator<is_const>& it, sentinel s){
            return it.ptr.lock() == nullptr;
        }
        bool operator==(const base_iterator<is_const>& other) const {
            return ptr.lock() != other.ptr.lock(); 
        }
};


template<class T> 
linked_list<T>::iterator linked_list<T>::begin() {
    return iterator(head);
};
template<class T> 
linked_list<T>::const_iterator linked_list<T>::begin() const {
    return cbegin();
};
template<class T> 
linked_list<T>::sentinel linked_list<T>::end() const {
    return cend();
};
template<class T> 
linked_list<T>::const_iterator linked_list<T>::cbegin() const{
    return const_iterator(head);
};
template<class T> 
linked_list<T>::sentinel linked_list<T>::cend() const {
    return {};
};


template<class T> 
linked_list<T>::linked_list() : size_(0){};
template<class T> 
linked_list<T>::linked_list(T* items, size_t size) : size_(0) {
    for(; size > 0; --size){
        prepend(items[size - 1]);
    }
};
template<class T> 
linked_list<T>::linked_list(const linked_list<T>& other) : size_(0){
    for(const auto& el: other){
        append(el);
    }
};
template<class T> 
linked_list<T>::linked_list(std::initializer_list<T> init_list) : size_(0){
    for(auto el: init_list){
        append(el);
    }
};



template<class T> 
template<class U> 
void linked_list<T>::append(U&& item) {
    auto new_node = std::make_shared<node>(std::forward<U>(item));
    if (size_ == 0){
        head = new_node;
        tail = new_node;
    }
    else{
        auto temp = tail.lock();
        new_node->prev = temp;
        temp->next = new_node;
        tail = new_node;
    }
    ++size_;
}
template<class T> 
template<class U> 
void linked_list<T>::prepend(U&& item){
    auto new_node = std::make_shared<node>(std::forward<U>(item));
    if (size_ == 0){
        head = new_node;
        tail = new_node;
    }
    else{
        head->prev = new_node;
        new_node->next = head;
        head = new_node;
    }
    ++size_;
};
template<class T> 
template<class U> 
void linked_list<T>::insert_at(size_t index, U&& item){
    if(index > size_) throw std::out_of_range("index out of range");
    if(index == 0) { prepend(std::forward<U>(item)); return; }
    if(index == size_) { append(std::forward<U>(item)); return; }

    auto new_node = std::make_shared<node>(std::forward<U>(item));
    auto behind = get_node(index - 1);

    new_node->next = behind->next;
    new_node->prev = behind;

    behind->next->prev = new_node;
    behind->next = new_node;
    ++size_;
};
template<class T> 
void linked_list<T>::clear() noexcept{
    head.reset();
    tail.reset();
    size_ = 0;
}

template<class T> 
size_t linked_list<T>::size() const noexcept{
    return size_;
};
template<class T>
const T& linked_list<T>::at(size_t index) const{
    if(index >= size_) 
        throw std::out_of_range("index out of range");
    return get_node(index)->value;
};
template<class T>
const T& linked_list<T>::first() const{
    if(head.get() == nullptr) 
        throw std::out_of_range("index out of range");
    return head->value;
};
template<class T>
const T& linked_list<T>::last() const{
    if(tail.lock() == nullptr) 
        throw std::out_of_range("index out of range");
    return tail.lock()->value;
};
template<class T>
T& linked_list<T>::at(size_t index) {
    if(index >= size_) 
        throw std::out_of_range("index out of range");
    return get_node(index)->value;
};
template<class T>
T& linked_list<T>::first() {
    if(head.get() == nullptr) 
        throw std::out_of_range("index out of range");
    return head->value;
};
template<class T>
T& linked_list<T>::last() {
    if(tail.lock() == nullptr) 
        throw std::out_of_range("index out of range");
    return tail.lock()->value;
};

template<class T>
std::shared_ptr<typename linked_list<T>::node> linked_list<T>::get_node(size_t index) {
    if (index >= size_) throw std::out_of_range("index out of range");
    if (index == 0) return head;

    node* cur = head.get();
    for(; index > 1; --index) 
        cur = cur->next.get();
    return cur->next;
};
template<class T>
std::shared_ptr<typename linked_list<T>::node> linked_list<T>::get_node(size_t index) const{
    if (index >= size_) throw std::out_of_range("index out of range");
    if (index == 0) return head;

    node* cur = head.get();
    for(; index > 1; --index) 
        cur = cur->next.get();
    return cur->next;
};
template<class T>
linked_list<T> linked_list<T>::get_sublist(size_t start_index, size_t end_index) const {
    if(end_index > size_ || 
       start_index > end_index) 
        throw std::out_of_range("index out of range");

    linked_list<T> sub;
    size_t i = 0;
    for (const auto& el: *this){
        if(i == end_index) return sub;
        if(i++ < start_index) continue;
        sub.append(el);
    }
    return sub;
};


template<class T> 
linked_list<T>& linked_list<T>::operator=(const linked_list<T>& other){
    if(this == &other) return *this;
    clear();
    for(const auto& el: other){
        append(el);
    }
    return *this;
}

template<class T>
linked_list<T> linked_list<T>::operator+(const linked_list<T>& other) const {
    linked_list<T> new_list;
    for(auto el1: *this) new_list.append(el1);
    for(auto el2: other) new_list.append(el2);
    return new_list;
}
template<class T>
linked_list<T>& linked_list<T>::operator+=(const linked_list<T>& other) {
    for(auto el: other) append(el);
    return *this;
}


template<class T>
struct linked_list<T>::node{
        std::shared_ptr<node> next;
        std::weak_ptr<node> prev;
        T value;

        template<class U>
        requires std::is_convertible_v<U, T>
        node(U&& val) : value(std::forward<U>(val)) {};
};
