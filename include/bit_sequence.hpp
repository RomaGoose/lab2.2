#include "dynamic_array.hpp"
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <sys/types.h>
#include <type_traits>

template<std::unsigned_integral Buffer>
class bit_sequence {
    private:
        dynamic_array<Buffer> items_;
        size_t size_;

        static constexpr size_t max_idx = sizeof(Buffer)*8 - 1;
        static constexpr size_t buff_sz = sizeof(Buffer)*8;
    public:        
        template<bool is_const>
        class base_iterator;
        template<bool is_const>
        class bit_ref {
            private:
                using buffer_type = std::conditional_t<is_const, Buffer, Buffer&>;
                
                buffer_type buf;
                size_t index;
            public:

                bit_ref(buffer_type buff, uint8_t index) : index(index), buf(buff) {};
                

                // void operator=(bool b)  requires (!is_const) { TODO: константность прокси :))
                void operator=(bool b) {
                    if(b) 
                        buf |= static_cast<Buffer>(1) << (max_idx - index);
                    else 
                        buf &= ~(static_cast<Buffer>(1) << (max_idx - index));
                };
                operator bool() const {
                    return (buf >> (max_idx - index)) & static_cast<Buffer>(1);
                };
        };

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;

    
        using value_type = bool;
        using reference = bit_ref<false>;

        // bit_sequence() = default;
        
        bit_sequence(size_t size) : size_(size), items_(std::bit_ceil((size + buff_sz - 1)/buff_sz)) {}
        bit_sequence(size_t size, bool value) : size_(size), items_(std::bit_ceil((size + buff_sz - 1)/buff_sz)) {
            Buffer b = value ? ~(static_cast<Buffer>(0)) : static_cast<Buffer>(0);
            for(auto& el: items_){
                el = b;
            }
        }

        explicit bit_sequence(std::initializer_list<Buffer> buffers) : 
            size_(buffers.size()*buff_sz), items_(std::bit_ceil(buffers.size())) {
            
            std::copy_n(buffers.begin(), buffers.size(), items_.begin());
        }
        
        explicit bit_sequence(std::initializer_list<bool> bools = {}) : size_(bools.size()), items_(std::bit_ceil(bools.size()/buff_sz)) {
            auto it = begin();
            for(bool b: bools){
                *(it++) = b;
            }
        }

        ~bit_sequence() = default;

        value_type operator[](size_t index) const {
            return bit_val(index);
        }
        reference operator[](size_t index){
            return make_bit_ref(index);
        }

        value_type at(size_t index) const {
            if (index >= size_) throw std::out_of_range("index out of range");
            return bit_val(index);
        }
        reference at(size_t index) {
            if (index >= size_) throw std::out_of_range("index out of range");
            return make_bit_ref(index);
        }
        
        value_type first() const {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return bit_val(0);
        }
        
        reference first() {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return make_bit_ref(0);
        }
        
        value_type last() const {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return bit_val(size_ - 1);
        }
        
        reference last() {
            if (size_ == 0) throw std::out_of_range("index out of range");
            return make_bit_ref(size_ - 1);
        }
        
        size_t size() const noexcept {
            return size_;
        }

        bit_sequence<Buffer>& append(bool item) {
            if (size_ == items_.size() * buff_sz) 
                items_.resize(items_.size() * 2);

            operator[](size_++) = item;
            return *this;
        }
        
        template<class ...Args>
        bit_sequence<Buffer>& append(Args&& ...args){
            (append(args), ...);
            return *this;
        }


        bit_sequence<Buffer>& prepend(bool item) {
            if(size_ == items_.size() * buff_sz)
                items_.resize(items_.size() * 2);

            for(auto rit = items_.begin() + (size_ / buff_sz); rit != items_.begin(); --rit){
                Buffer appeared = *(rit-1) & static_cast<Buffer>(1);
                
                *rit >>= 1;
                *rit |= appeared << max_idx;
            }
            items_[0] >>= 1;
            operator[](0) = item;

            ++size_;
            return *this;
        }

        template<class ...Args>
        bit_sequence<Buffer>& prepend(Args&& ...args){
            (prepend(args), ...);
            return *this;
        }
        
        
        bit_sequence<Buffer>& insert_at(size_t index, bool item) {
            if(index == 0) return prepend(item);
            if(index == size_) return append(item);

            if(index > size_) throw std::out_of_range("index out of range");
            if(size_ == items_.size() * buff_sz)
                items_.resize(items_.size() * 2);

            size_t inserted_buf_idx = index / buff_sz;

            for(auto rit = items_.begin() + (size_ / buff_sz); rit != items_.begin() + inserted_buf_idx; --rit){
                Buffer appeared = *(rit-1) & static_cast<Buffer>(1);
                
                *rit >>= 1;
                *rit |= appeared << max_idx;
            }

            for(auto bit_it = begin() + inserted_buf_idx*buff_sz + max_idx; bit_it != begin() + index; --bit_it){
                *bit_it = (bool)*(bit_it - 1);
            }
            
            operator[](index) = item;
            
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
            return iterator(begin() + size_);
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
            return const_iterator(begin() + size_);
        }

        bit_sequence<Buffer> get_subsequence(size_t start, size_t end) const {
            if (start > end || end > size_) throw std::out_of_range("invalid range");
            
            bit_sequence<Buffer> tmp;
            tmp.items_.resize(std::bit_ceil((end - start + buff_sz - 1)/ buff_sz));
            for (auto it = begin() + start; it != begin() + end; ++it) {
                tmp.append(((bool)*it));
            }
            return tmp;
        }
    private:
        bool bit_val(size_t index) const {
            return (items_[index/buff_sz] >> (max_idx - (index % buff_sz))) & static_cast<Buffer>(1);
        }

        reference make_bit_ref(size_t index){
            return reference(items_[index/buff_sz], index % buff_sz);
        };
        
};






























template<std::unsigned_integral Buffer>
template<bool is_const>
class bit_sequence<Buffer>::base_iterator{
    private:
        using iter = dynamic_array<Buffer>::template base_iterator<is_const>;
        
        iter buf_iter;
        size_t bit_index;
    public:
        using value_type = bool;
        using reference = std::conditional_t<is_const, value_type, typename bit_sequence<Buffer>::reference>;
        using difference_type = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        base_iterator(iter it) : buf_iter(it), bit_index(0) {};
        base_iterator(const base_iterator& other) : buf_iter(other.buf_iter), bit_index(other.bit_index) {};
        base_iterator() : buf_iter(), bit_index(0) {};

        base_iterator& operator++(){
            if(++bit_index == buff_sz){
                bit_index = 0;
                ++buf_iter;
            }
            return *this;
        }
        base_iterator& operator--(){
            if(bit_index-- == 0){
                bit_index = max_idx;
                --buf_iter;
            }
            return *this;
        }
        base_iterator operator++(int){
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        bit_ref<is_const> operator*() const {
            return bit_ref<is_const>(*buf_iter, bit_index);
        }

        base_iterator& operator+=(difference_type n) {
            if (n<0) return *this -= -n;

            bit_index += n;
            buf_iter += bit_index / buff_sz;
            bit_index %= buff_sz;

            return *this;
        };

        base_iterator operator+(difference_type n) const { 
            auto tmp = *this; 
            tmp += n; 
            return tmp; 
        }
        
        base_iterator& operator-=(difference_type n) {
            if (n<0) return *this += -n;

            auto rem = n % buff_sz;
            buf_iter -= (n / buff_sz);
            if (bit_index < rem) {
                bit_index += 8 - rem;
                --buf_iter;

                return *this;
            }

            bit_index -= rem;
            return *this;
        }
        base_iterator operator-(difference_type n) const { 
            auto tmp = *this; 
            tmp -= n; 
            return tmp; 
        }
        
        difference_type operator-(const iterator& other) const {
            return (buf_iter - other.buf_iter) * buff_sz + (bit_index - other.bit_index);
        }
        
        reference operator[](difference_type n) const { 
            return *(*this + n); 
        }
        auto operator<=>(const base_iterator<is_const>& other) const = default;
};

