#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"

#include "list_sequence.hpp"
#include "array_sequence.hpp"
#include "concat.hpp"
#include "map.hpp"
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
    TestType seq = {1,2,3,4,5};
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
        }
        SECTION("T -> U"){
            auto m = cref | map(to_str);
            auto m2 = cref | map(to_str);
            
            CHECK(equal(m, strr));
            CHECK(equal(m2, strr));
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
    }
}