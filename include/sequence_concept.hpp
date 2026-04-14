#pragma once

#include <iterator>

template<class Seq>
concept sequence = requires (Seq s, const Seq& cs, size_t i, size_t end, size_t start, typename Seq::value_type item) {
    typename Seq::iterator;
    typename Seq::const_iterator;
    
    { s.at(i) } -> std::same_as<typename Seq::value_type&>;
    { s.first() } -> std::same_as<typename Seq::value_type&>;
    { s.last() } -> std::same_as<typename Seq::value_type&>;
    { s.append(item) };
    { s.prepend(item) };
    { s.insert_at(i, item) };
    { s.append(std::move(item)) };
    { s.prepend(std::move(item)) };
    { s.insert_at(i, std::move(item)) };
    { s.begin() } -> std::forward_iterator;
    { s.end() } -> std::sentinel_for<decltype(s.begin())>;
    
    { cs.at(i) } -> std::same_as<const typename Seq::value_type&>;
    { cs.first() } -> std::same_as<const typename Seq::value_type&>;
    { cs.last() } -> std::same_as<const typename Seq::value_type&>;
    { cs.cbegin() } -> std::forward_iterator;
    { cs.cend() } -> std::sentinel_for<decltype(cs.begin())>;
    
    { cs.size() } -> std::same_as<size_t>;
    { cs.get_subsequence(start, end) } -> std::same_as<Seq>;
};