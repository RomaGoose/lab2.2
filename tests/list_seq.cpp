#include "catch2/catch_test_macros.hpp"
#include "list_sequence.hpp"
#include <algorithm>
#include <initializer_list>

template<class T>
static bool operator==(list_sequence<T>& seq, std::initializer_list<T> arr) {
    if (arr.size() != seq.size()) return false;
    return std::equal(arr.begin(), arr.end(), seq.begin());
}

template<class T>
static std::ostream& operator<<(std::ostream& os, const list_sequence<T>& seq) {
    os << "seq: {";
    for (const auto& el : seq) {
        os << el << ", ";
    }
    return os << "}";
}

TEST_CASE("list_sequence constructor", "[list_sequence]") {
    SECTION("default constructor") {
        list_sequence<int> seq;
        CHECK(seq.size() == 0);
    }
    
    SECTION("initializer_list constructor") {
        list_sequence<int> seq = {1, 2, 3, 4, 5};
        REQUIRE(seq.size() == 5);
        CHECK(seq == std::initializer_list<int>{1, 2, 3, 4, 5});
    }
    
    SECTION("empty initializer_list") {
        list_sequence<int> seq = {};
        CHECK(seq.size() == 0);
    }
}

TEST_CASE("list_sequence access", "[list_sequence]") {
    list_sequence<int> seq = {1, 2, 3, 4, 5};
    const list_sequence<int>& cref = seq;
    
    SECTION("at") {
        CHECK(seq.at(0) == 1);
        CHECK(seq.at(4) == 5);
        CHECK(cref.at(2) == 3);
        
        seq.at(2) = 10;
        CHECK(seq.at(2) == 10);
        CHECK(seq == std::initializer_list<int>{1, 2, 10, 4, 5});
        
        CHECK_THROWS(seq.at(5));
        CHECK_THROWS(seq.at(-1));
    }
    
    SECTION("first") {
        CHECK(seq.first() == 1);
        CHECK(cref.first() == 1);
        
        seq.first() = 234;
        CHECK(seq.first() == 234);
        CHECK(seq == std::initializer_list<int>{234, 2, 3, 4, 5});
    }
    
    SECTION("last") {
        CHECK(seq.last() == 5);
        CHECK(cref.last() == 5);
        
        seq.last() = 234;
        CHECK(seq.last() == 234);
        CHECK(seq == std::initializer_list<int>{1, 2, 3, 4, 234});
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

TEST_CASE("list_sequence adding elements", "[list_sequence]") {
    list_sequence<int> seq = {1, 2, 3};
    list_sequence<int> empty;
    list_sequence<std::string> empty_str;

    SECTION("append"){
        SECTION("to empty") {
            empty.append(12);
            CHECK(empty.size() == 1);
            CHECK(empty.first() == 12);
            CHECK(empty.last() == 12);
            CHECK(empty == std::initializer_list<int>{12});
        }
        
        SECTION("to existing") {
            seq.append(4);
            CHECK(seq.size() == 4);
            CHECK(seq.last() == 4);
            CHECK(seq == std::initializer_list<int>{1, 2, 3, 4});
        }
        SECTION("with move") {
            std::string s = "hello";
            empty_str.append(std::move(s));
            CHECK(empty_str.size() == 1);
            CHECK(empty_str.first() == "hello");
            CHECK(s.empty());  
            CHECK(s == "");  
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
            list_sequence<int> seq = {2, 3, 4};
            seq.prepend(1);
            CHECK(seq.size() == 4);
            CHECK(seq.first() == 1);
            CHECK(seq == std::initializer_list<int>{1, 2, 3, 4});
        }
        
        SECTION("with move") {
            std::string s = "hello";
            empty_str.prepend(std::move(s));
            CHECK(empty_str.size() == 1);
            CHECK(empty_str.first() == "hello");
            CHECK(s.empty());  
        }
    }

    SECTION("insert"){
        list_sequence<int> seq = {1, 2, 3, 4};

        SECTION("at beginning") {
            seq.insert_at(0, 1);
            CHECK(seq == std::initializer_list<int>{1, 1, 2, 3, 4});
        }
        
        SECTION("at middle") {
            seq.insert_at(2, 3);
            CHECK(seq == std::initializer_list<int>{1, 2, 3, 3, 4});
        }
        
        SECTION("at end") {
            seq.insert_at(4, 5);
            CHECK(seq == std::initializer_list<int>{1, 2, 3, 4, 5});
        }
        
        SECTION("to empty") {
            list_sequence<int> seq;
            seq.insert_at(0, 42);
            CHECK(seq.size() == 1);
            CHECK(seq.first() == 42);
        }
        SECTION("with move") {
            std::string s = "hello";
            empty_str.insert_at(0, std::move(s));
            CHECK(empty_str.size() == 1);
            CHECK(empty_str.first() == "hello");
            CHECK(s.empty());  
        }
    }

    SECTION("chaining"){
        empty.append(1).append(2).prepend(0).append(3).insert_at(3, 10);
        
        CHECK(empty == std::initializer_list<int>{0, 1, 2, 10, 3});
    }
}

TEST_CASE("list_sequence iterators", "[list_sequence]") {
    list_sequence<int> seq = {1, 2, 3, 4, 5};
    SECTION("range-based for const") {
        int expected[] = {1, 2, 3, 4, 5};
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
        CHECK(seq == std::initializer_list<int>{0,0,0,0,0});
    }

    SECTION("range-based copy") {
        int i = 0;
        for (auto el : seq) {
            el = 0;
            ++i;
        }
        CHECK(i == 5);
        CHECK(seq == std::initializer_list<int>{1, 2, 3, 4, 5});
    }
    
    SECTION("empty sequence iteration") {
        list_sequence<int> seq;
        for (const auto& el : seq) {
            FAIL("iterating empty sequence");
        }
        SUCCEED();
    }
}

TEST_CASE("list_sequence clear", "[list_sequence]") {
    list_sequence<int> seq = {1, 2, 3, 4};
    seq.clear();
    CHECK(seq.size() == 0);
    CHECK_THROWS(seq.first());
    CHECK_THROWS(seq.last());
}

TEST_CASE("list_sequence exceptions", "[list_sequence]") {
    list_sequence<int> seq = {1, 2, 3};
    list_sequence<int> empty;
    
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
