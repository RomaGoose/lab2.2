#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"

#include "list_sequence.hpp"
#include "array_sequence.hpp"
#include "view.hpp"
#include <cstddef>
#include <initializer_list>

// #include <ostream>

// template<sequence Seq>
// static std::ostream& operator<<(std::ostream& os, const Seq& seq) {
//     os << "seq: {";
//     for (const auto& el : seq) {
//         os << el << ", ";
//     }
//     return os << "}";
// }


// template<range Seq,range Seq2>
// static bool operator==(const Seq& seq, const Seq2& arr) {
//     if (arr.size() != seq.size()) return false;
//     return std::equal(arr.begin(), arr.end(), seq.begin());
// }
template<range Seq>
static bool operator==(Seq& seq, std::initializer_list<typename Seq::value_type> arr) {
    return std::equal(arr.begin(), arr.end(), seq.begin());
}


TEMPLATE_TEST_CASE("common sequence behaviour test", "[sequence]",
                    list_sequence<int>,
                    list_sequence<float>,
                    list_sequence<short>,
                    list_sequence<double>,
                    list_sequence<size_t>,
                    array_sequence<int>,
                    array_sequence<float>,
                    array_sequence<short>,
                    array_sequence<double>,
                    array_sequence<size_t>){

    using type = TestType::value_type;

    SECTION("sequence constructor", "[seq][constructor]") {
        SECTION("default constructor") {
            TestType seq;
            CHECK(seq.size() == 0);
        }
        
        SECTION("initializer_list constructor") {
            TestType seq = {1, 2, 3, 4, 5};
            REQUIRE(seq.size() == 5);
            CHECK(seq == std::initializer_list<type>{1, 2, 3, 4, 5});
        }
        
        SECTION("empty initializer_list") {
            TestType seq = {};
            CHECK(seq.size() == 0);
        }
    }
    SECTION("sequence access", "[seq]") {
        TestType seq = {1, 2, 3, 4, 5};
        const TestType& cref = seq;
        
        SECTION("at") {
            CHECK(seq.at(0) == 1);
            CHECK(seq.at(4) == 5);
            CHECK(cref.at(2) == 3);
            
            seq.at(2) = 10;
            CHECK(seq.at(2) == 10);
            CHECK(seq == std::initializer_list<type>{1, 2, 10, 4, 5});
            
            CHECK_THROWS(seq.at(5));
            CHECK_THROWS(seq.at(-1));
        }
        
        SECTION("first") {
            CHECK(seq.first() == 1);
            CHECK(cref.first() == 1);
            
            seq.first() = 234;
            CHECK(seq.first() == 234);
            CHECK(seq == std::initializer_list<type>{234, 2, 3, 4, 5});
        }
        
        SECTION("last") {
            CHECK(seq.last() == 5);
            CHECK(cref.last() == 5);
            
            seq.last() = 234;
            CHECK(seq.last() == 234);
            CHECK(seq == std::initializer_list<type>{1, 2, 3, 4, 234});
        }

        SECTION("operator[]") {
            CHECK(seq[0] == 1);
            CHECK(seq[2] == 3);
            CHECK(seq[4] == 5);

            CHECK(cref[0] == 1);
            CHECK(cref[2] == 3);
            CHECK(cref[4] == 5);

            seq[0] = 0;
            seq[2] = 0;
            seq[4] = 0;

            CHECK(seq[0] == 0);
            CHECK(seq[2] == 0);
            CHECK(seq[4] == 0);
        }
    }
    SECTION("sequence adding elements", "[seq]") {
        TestType seq = {1, 2, 3};
        TestType empty;


        SECTION("append"){
            SECTION("to empty") {
                empty.append(12);
                CHECK(empty.size() == 1);
                CHECK(empty.first() == 12);
                CHECK(empty.last() == 12);
                CHECK(empty == std::initializer_list<type>{12});
            }
            
            SECTION("to existing") {
                seq.append(4);
                CHECK(seq.size() == 4);
                CHECK(seq.last() == 4);
                CHECK(seq == std::initializer_list<type>{1, 2, 3, 4});
            }

            SECTION("fold expr") {
                seq.append(4,5,6);
                CHECK(seq.size() == 6);
                CHECK(seq == std::initializer_list<type>{1, 2, 3, 4, 5, 6});
            }
        }

        SECTION("prepend"){
            SECTION("to empty") {
                empty.prepend(42);
                CHECK(empty.size() == 1);
                CHECK(empty.first() == 42);
                CHECK(empty.last() == 42);
            }
            
            SECTION("to existing") {
                TestType seq = {2, 3, 4};
                seq.prepend(1);
                CHECK(seq.size() == 4);
                CHECK(seq.first() == 1);
                CHECK(seq == std::initializer_list<type>{1, 2, 3, 4});
            }

            SECTION("fold expr") {
                seq.prepend(4,5,6);
                CHECK(seq.size() == 6);
                CHECK(seq == std::initializer_list<type>{6, 5, 4, 1, 2, 3});
            }
        }

        SECTION("insert"){
            TestType seq = {1, 2, 3, 4};

            SECTION("at beginning") {
                seq.insert_at(0, 1);
                CHECK(seq == std::initializer_list<type>{1, 1, 2, 3, 4});
            }
            
            SECTION("at middle") {
                seq.insert_at(2, 3);
                CHECK(seq == std::initializer_list<type>{1, 2, 3, 3, 4});
            }
            
            SECTION("at end") {
                seq.insert_at(4, 5);
                CHECK(seq == std::initializer_list<type>{1, 2, 3, 4, 5});
            }
            
            SECTION("to empty") {
                TestType seq;
                seq.insert_at(0, 42);
                CHECK(seq.size() == 1);
                CHECK(seq.first() == 42);
            }
        }

        SECTION("chaining"){
            empty.append(1).append(2).prepend(0).append(3).insert_at(3, 10);
            
            CHECK(empty == std::initializer_list<type>{0, 1, 2, 10, 3});
        }
    }
    SECTION("sequence iterators", "[seq]") {
        TestType seq = {1, 2, 3, 4, 5};
        SECTION("range-based for const") {
            type expected[] = {1, 2, 3, 4, 5};
            int i = 0;
            for (const auto& el : seq) {
                CHECK(el == expected[i++]);
            }
            CHECK(i == 5);
        }

        SECTION("range-based for mutable") {
            int i = 0;
            for (auto& el : seq) {
                el = 0;
                ++i;
            }
            CHECK(i == 5);
            CHECK(seq == std::initializer_list<type>{0,0,0,0,0});
        }

        SECTION("range-based copy") {
            int i = 0;
            for (auto el : seq) {
                el = 0;
                ++i;
            }
            CHECK(i == 5);
            CHECK(seq == std::initializer_list<type>{1, 2, 3, 4, 5});
        }
        
        SECTION("empty sequence iteration") {
            TestType seq;
            for (const auto& el : seq) {
                FAIL("iterating empty sequence");
            }
            SUCCEED();
        }
    }
    SECTION("sequence clear", "[seq]") {
        TestType seq = {1, 2, 3, 4};
        seq.clear();
        CHECK(seq.size() == 0);
        CHECK_THROWS(seq.first());
        CHECK_THROWS(seq.last());
    }
    SECTION("sequence exceptions", "[seq]") {
        TestType seq = {1, 2, 3};
        TestType empty;
        
        SECTION("at out of range") {
            CHECK_THROWS(seq.at(3));
            CHECK_THROWS(seq.at(234));
        }
        SECTION("insert out of range") {
            CHECK_THROWS(seq.insert_at(4, 10));
        }
        SECTION("first empty") {
            CHECK_THROWS(empty.first());
        }
        SECTION("last empty") {
            CHECK_THROWS(empty.last());
        }
    }
    SECTION("sequence get_subsequence", "[linked_list]"){
        TestType seq = {1,2,3,4,5,6,7};
        TestType sub;

        SECTION("middle sublist"){
            sub = seq.get_subsequence(2, 5);

            CHECK(sub == std::initializer_list<type>{3,4,5});
        }
        SECTION("start sublist"){
            sub = seq.get_subsequence(0, 4);

            CHECK(sub == std::initializer_list<type>{1,2,3,4});
        }
        SECTION("end sublist"){
            sub = seq.get_subsequence(4, 7);

            CHECK(sub == std::initializer_list<type>{5,6,7});
        }
        SECTION("full sublist"){
            sub = seq.get_subsequence(0, 7);

            CHECK(sub == std::initializer_list<type>{1,2,3,4,5,6,7});
        }
        SECTION("empty sublist"){
            TestType sub1 = seq.get_subsequence(4, 4);
            TestType sub2 = seq.get_subsequence(7, 7);
            TestType sub3 = seq.get_subsequence(0, 0);

            CHECK(sub1 == std::initializer_list<type>{});
            CHECK(sub2 == std::initializer_list<type>{});
            CHECK(sub3 == std::initializer_list<type>{});
        }
    }
}

TEMPLATE_TEST_CASE("sequence move methods", "[sequence]", 
                    list_sequence<std::string>,
                    array_sequence<std::string>) {
    TestType empty_str;
    std::string s = "hello";
    std::string s2 = "hello2";
    std::string s3 = "hello3";

    SECTION("append"){
        empty_str.append(std::move(s));
        CHECK(empty_str.size() == 1);
        CHECK(empty_str.first() == "hello");
        CHECK(s.empty());  
        CHECK(s == "");  
    }

    SECTION("prepend"){
        empty_str.prepend(std::move(s));
        CHECK(empty_str.size() == 1);
        CHECK(empty_str.first() == "hello");
        CHECK(s.empty());  
    }

    SECTION("insert"){
        empty_str.insert_at(0, std::move(s));
        CHECK(empty_str.size() == 1);
        CHECK(empty_str.first() == "hello");
        CHECK(s.empty());  
    }

    SECTION("variadic move methods"){
        empty_str.append("initial");
    
        SECTION("append"){
            empty_str.append(std::move(s), s2, std::move(s3));

            CHECK(s.empty());  
            CHECK(!s2.empty());  
            CHECK(s3.empty());  

            CHECK(empty_str.size() == 4);
            CHECK(empty_str[0] == "initial");
            CHECK(empty_str[1] == "hello");
            CHECK(empty_str[2] == "hello2");
            CHECK(empty_str[3] == "hello3");
        }
        SECTION("prepend"){
            empty_str.prepend(std::move(s), s2, std::move(s3));

            CHECK(s.empty());  
            CHECK(!s2.empty());  
            CHECK(s3.empty());  

            CHECK(empty_str.size() == 4);
            CHECK(empty_str[0] == "hello3");
            CHECK(empty_str[1] == "hello2");
            CHECK(empty_str[2] == "hello");
            CHECK(empty_str[3] == "initial");
        }
    }
}

TEMPLATE_TEST_CASE("array_seq specific", "[arr_seq]",
                    array_sequence<int>,
                    array_sequence<float>,
                    array_sequence<short>,
                    array_sequence<double>,
                    array_sequence<size_t>){
    SECTION("growth check"){
        TestType seq;
        for(size_t i = 0; i < 10000; ++i){
            seq.append(i);
        }
        CHECK(seq.size() == 10000);
    }

    SECTION("RA iterator"){
        TestType seq = {1, 2, 3, 4, 5, 6};
        auto it = seq.begin();
        auto end = seq.end();

        SECTION("operator+, operator+= and operator++") {
            CHECK(*(it+2) == 3);
            CHECK(*(it+5) == 6);
            it += 2;
            CHECK(*it == 3);
            it++;
            CHECK(*it == 4);
        }

        SECTION("operator-, operator-= and operator--") {
            CHECK(*(end-2) == 5);
            CHECK(*(end-5) == 2);
            end -= 2;
            CHECK(*end == 5);
            --end;
            CHECK(*end == 4);
        }

        SECTION("operator[]"){
            CHECK(it[3] == 4);
            it += 4;
            CHECK(it[-2] == 3);
        }

        SECTION("difference between iterators") {
            CHECK((end - it) == 6);
            CHECK((end - (it+2)) == 4);
            CHECK((it - end) == -6);
        }

        SECTION("comparison") {
            auto it2 = it + 2;
            CHECK(it < it2);
            CHECK(it2 > it);
            CHECK(it <= it2);
            CHECK(it2 >= it);
            CHECK(it <= it);
            CHECK(it >= it);
        }
        SECTION("boundary behavior") {
            CHECK(it + seq.size() == end);
            CHECK(end - seq.size() == it);
        }
    }
}