#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"

#include "list_sequence.hpp"
#include "array_sequence.hpp"
#include "concat.hpp"
#include "map.hpp"
#include "filter.hpp"
#include <algorithm>
#include <iterator>
#include <string>

using std::ranges::equal;
using std::ranges::distance;

TEST_CASE("concat_view", "[view]"){

    array_sequence<int> arr1 = {1,2,3,4};
    array_sequence<int> arr2 = {5,6,7,8};
    list_sequence<int> lst1 = {1,2,3,4};
    list_sequence<int> lst2 = {5,6,7,8};

    array_sequence<int> empty_arr1;
    array_sequence<int> empty_lst1;
    list_sequence<int> empty_arr2;
    list_sequence<int> empty_lst2;

    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8};

    SECTION("binary concat"){
        SECTION("same types") {
            SECTION("array_sequence + array_sequence") {
                auto c = arr1 + arr2;

                CHECK(distance(c) == 8);
                CHECK(equal(c, expected));
            }
    
            SECTION("list_sequence + list_sequence") {
                auto c = lst1 + lst2;

                CHECK(distance(c) == 8);
                CHECK(equal(c, expected));
            }
        }
    
        SECTION("different types") {
            SECTION("array_sequence + list_sequence") {
                auto c = arr1 + lst2;

                CHECK(distance(c) == 8);
                CHECK(equal(c, expected));
            }
    
            SECTION("list_sequence + array_sequence") {
                auto c = lst1 + arr2;

                CHECK(distance(c) == 8);
                CHECK(equal(c, expected));
            }
        }
    
        SECTION("empty sequences") {
            SECTION("empty + non-empty") {
                auto c1 = empty_arr1 + arr2;
                auto c2 = empty_lst1 + lst2;

                
                CHECK(distance(c1) == 4);
                CHECK(distance(c2) == 4);
                
                CHECK(equal(c1, arr2));
                CHECK(equal(c2, lst2));
            }
    
            SECTION("non-empty + empty") {
                auto c1 = arr1 + empty_arr2;
                auto c2 = lst1 + empty_lst2;

                CHECK(distance(c1) == 4);
                CHECK(distance(c2) == 4);
                
                CHECK(equal(c1, arr1));
                CHECK(equal(c2, lst1));
            }
    
            SECTION("empty + empty") {
                auto c1 = empty_arr1 + empty_arr2;
                auto c2 = empty_lst1 + empty_lst2;
                
                CHECK(c1.begin() == c1.end());
                CHECK(c2.begin() == c2.end());
            }
        }
    }

    SECTION("chained concat") {
        auto c11 = arr1 + arr2 + lst1 + lst2;
        auto c12 = arr1 + arr2 + arr1 + lst2;
        auto c13 = arr1 + lst2 + lst1 + lst2;
        auto c14 = arr1 + lst2 + lst1 + arr2;
        auto c15 = lst1 + arr2 + arr1 + lst2;
        
        int expected[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};


        CHECK(distance(c11) == 16);
        CHECK(distance(c12) == 16);
        CHECK(distance(c13) == 16);
        CHECK(distance(c14) == 16);
        CHECK(distance(c15) == 16);
        CHECK(equal(c11, expected));
        CHECK(equal(c12, expected));
        CHECK(equal(c13, expected));
        CHECK(equal(c14, expected));
        CHECK(equal(c15, expected));
    }

    SECTION("self concat") {
        SECTION("twice"){
            auto c1 = arr1 + arr1;
            auto c2 = lst1 + lst1;

            int expected[] = {1,2,3,4,1,2,3,4};

            CHECK(distance(c1) == 8);
            CHECK(distance(c2) == 8);
            CHECK(equal(c1, expected));
            CHECK(equal(c2, expected));
        }

        SECTION("thrice"){
            auto c1 = arr1 + arr1 + arr1;
            auto c2 = lst1 + lst1 + lst1;

            int expected[] = {1,2,3,4,1,2,3,4,1,2,3,4};

            CHECK(distance(c1) == 12);
            CHECK(distance(c2) == 12);
            CHECK(equal(c1, expected));
            CHECK(equal(c2, expected));
        }
    }
}

TEMPLATE_TEST_CASE("map_view", "[view]", array_sequence<int>, list_sequence<int>) {
    TestType before_seq = {1,2,3,4,5};
    TestType seq = before_seq;
    const auto& cref = seq;

    auto square = [](int x){ return x * x; };
    auto add_2 = [](int x){ return x + 2; };
    auto to_str = [](int x){ return std::to_string(x); };
    
    int exp_sqr[] = {1,4,9,16,25};
    std::string strr[] = {"1","2","3","4","5"};

    
    SECTION("basic functionality"){
        SECTION("T -> T"){
            auto m = seq | map(square);
            auto m2 = cref | map(square);
            
            CHECK(equal(m, exp_sqr));
            CHECK(equal(m2, exp_sqr));

            CHECK(equal(seq, before_seq));
        }
        SECTION("T -> U"){
            auto m = cref | map(to_str);
            auto m2 = cref | map(to_str);
            
            CHECK(equal(m, strr));
            CHECK(equal(m2, strr));

            CHECK(equal(seq, before_seq));
        }
    }

    SECTION("empty sequence") {
        array_sequence<int> empty_arr;
        list_sequence<int> empty_lst;

        auto m1 = empty_arr | map(square);
        auto m2 = empty_lst | map(square);

        CHECK(m1.begin() == m1.end());
        CHECK(m2.begin() == m2.end());
    }

    SECTION("chaining") {
        auto m = seq | map(square)
                     | map(add_2)
                     | map(to_str);
        
        std::string expected[] = {"3","6","11","18","27"};
        
        CHECK(equal(m, expected));
        CHECK(equal(seq, before_seq));
    }
}

TEMPLATE_TEST_CASE("filter_view", "[view]", array_sequence<int>, list_sequence<int>) {
    TestType before_seq = {1,2,3,4,5};
    TestType seq = before_seq;
    const auto& cref = seq;
    TestType empty;
    const auto& cref_empty = empty;

    int exp_even[] = {2,4};
    int exp_odd[] = {1,3,5};

    auto even = [](int x) { return (x & 1) == 0; };
    auto id = [](int x){ return 1; };
    auto nul = [](int x){ return 0; };

    auto f = seq | filter(even);

    CHECK(distance(f) == 2);
    CHECK(equal(f, exp_even));

    SECTION("basic functionality"){
        SECTION("all pass"){
            auto f = seq | filter(id);
            auto f1 = cref | filter(id);

            CHECK(distance(f) == 5);
            CHECK(distance(f1) == 5);
            CHECK(equal(f, seq));
            CHECK(equal(f1, seq));
            CHECK(equal(seq, before_seq));
        }
        SECTION("none pass"){
            auto f = seq | filter(nul);
            auto f1 = cref | filter(nul);

            CHECK(distance(f) == 0);
            CHECK(distance(f1) == 0);
            CHECK(equal(seq, before_seq));
        }
        SECTION("some pass"){
            auto f = seq | filter(even);
            auto f1 = cref | filter(even);

            CHECK(distance(f) == 2);
            CHECK(distance(f1) == 2);
            CHECK(equal(f, exp_even));
            CHECK(equal(f1, exp_even));
            CHECK(equal(seq, before_seq));
        }
    }

    SECTION("empty sequence"){
        auto f = empty | filter(even);
        auto f1 = cref_empty | filter(even);

        CHECK(distance(f) == 0);
        CHECK(distance(f1) == 0);
    
        CHECK(f.begin() == f.end());
        CHECK(f1.begin() == f1.end());
    }

    SECTION("chaining"){
        TestType big_seq = {1,2,3,4,5,6,7,8,9,10};

        auto mult_of_3 = [](int x) { return x % 3 == 0; };

        auto f = big_seq | filter(id)
                         | filter(even)
                         | filter(id)
                         | filter(mult_of_3)
                         | filter(id);

        CHECK(distance(f) == 1);
        CHECK(*(f.begin()) == 6);
    }
}

TEMPLATE_TEST_CASE("views interaction", "[view]", array_sequence<int>, list_sequence<int>) {
    TestType seq1 = {1,2,3,4,5};
    TestType seq2 = {6,7,8,9,10};
    
    auto id = [](int x){ return 1; };

    
    auto v = (((seq1 + seq2) | filter([](int x){ return 1; })
                             | map([](int x){ return x + 10; })
                             | filter([](int x){ return x % 3 == 0; })
                             | map([](int x){ return x - 5; }))
                             + seq1
                             | map([](int x){ return std::to_string(x); })
                             | map([](std::string s){ return (int)s.length(); }))
                             + seq2;
    
    int exp[] = {1,2,2,1,1,1,1,1,6,7,8,9,10};

    CHECK(equal(v, exp));
}