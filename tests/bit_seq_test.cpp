
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "bit_sequence.hpp"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

using std::ranges::equal;

template<class Buf1, class Buf2>
static bool verbose_equal(bit_sequence<Buf1> range1, bit_sequence<Buf2> range2) {
    REQUIRE(range1.size() == range2.size());

    size_t i = 0;
    auto it1 = range1.begin();
    for(auto el2: range2){
        CAPTURE(i++);
        CHECK(*(it1++) == el2);
    }
    return true;
}

TEST_CASE("bit_seq ctors", "[bit_seq]"){

    SECTION("default ctor") {
        bit_sequence<uint8_t> bs;
        CHECK(bs.size() == 0);
        CHECK(bs.begin() == bs.end());
    }

    SECTION("size ctor"){
        bit_sequence<uint8_t> bs(9);
        bit_sequence<uint8_t> bs_false(9, false);
        bit_sequence<uint8_t> bs_true(9, true);

        REQUIRE(bs.size() == 9);
        REQUIRE(bs_false.size() == 9);
        REQUIRE(bs_true.size() == 9);

        for(size_t i = 0; i < 9; ++i){
            CHECK(!bs_false[i]);
            CHECK(bs_true[i]);
        }
    }

    SECTION("init list of bools") {
        bit_sequence<uint8_t> bs{true, true, true, false, true};
        
        REQUIRE(bs.size() == 5);

        CHECK(bs[0] == true);
        CHECK(bs[1] == true);
        CHECK(bs[2] == true);
        CHECK(bs[3] == false);
        CHECK(bs[4] == true);
    }

    SECTION("init list of buffers") {
        bit_sequence<uint8_t> bs{(uint8_t)0b00110011, (uint8_t)0b00001111};

        REQUIRE(bs.size() == 16);

        CHECK(bs[0] == false); 
        CHECK(bs[1] == false);  
        CHECK(bs[2] == true);
        CHECK(bs[3] == true);
        CHECK(bs[4] == false);
        CHECK(bs[5] == false);
        CHECK(bs[6] == true);
        CHECK(bs[7] == true);

        CHECK(bs[8] == false);  
        CHECK(bs[9] == false);
        CHECK(bs[10] == false);
        CHECK(bs[11] == false);
        CHECK(bs[12] == true);
        CHECK(bs[13] == true);
        CHECK(bs[14] == true);
        CHECK(bs[15] == true);
    }


}

TEST_CASE("bit_seq access", "[bit_seq]"){
    bit_sequence<uint8_t> bs(10, false);

    const bit_sequence<uint8_t>& cref = bs;
    SECTION("at") {
        bs.at(2) = true;
        bs.at(3) = true;
        bs.at(9) = true;

        CHECK(bs.at(2));
        CHECK(bs.at(3));
        CHECK(bs.at(9));
        CHECK(cref.at(2));
        CHECK(cref.at(3));
        CHECK(cref.at(9));
        
        bs.at(3) = false;
        bs.at(7) = true;
        
        CHECK(!bs.at(3));
        CHECK(bs.at(7));
        CHECK(!cref.at(3));
        CHECK(cref.at(7));
    }

    SECTION("operator[]") {
        bs[2]= true;
        bs[3] = true;
        bs[9] = true;

        CHECK(bs[2]);
        CHECK(bs[3]);
        CHECK(bs[9]);
        CHECK(cref[2]);
        CHECK(cref[3]);
        CHECK(cref[9]);
        
        bs[3] = false;
        bs[7] = true;
        
        CHECK(!bs[3]);
        CHECK(bs[7]);
        CHECK(!cref[3]);
        CHECK(cref[7]);
    }
    
    SECTION("first"){
        CHECK(bs.first() == 0);
        CHECK(cref.first() == 0);

        bs.first() = 1;

        CHECK(bs.first() == 1);
        CHECK(cref.first() == 1);

        bs.at(0) = 0;

        CHECK(bs.first() == 0);
        CHECK(cref.first() == 0);
    }
    
    SECTION("last"){
        CHECK(bs.last() == 0);
        CHECK(cref.last() == 0);

        bs.last() = 1;

        CHECK(bs.last() == 1);
        CHECK(cref.last() == 1);

        bs.at(bs.size() - 1) = 0;

        CHECK(bs.last() == 0);
        CHECK(cref.last() == 0);
    }
}

TEST_CASE("bit_seq adding an element", "[bit_seq]"){
    bit_sequence<uint16_t> bs{true, false, true, true};
    bit_sequence<uint16_t> empty;


    SECTION("append"){
        SECTION("to empty") {
            empty.append(true);

            CHECK(empty.size() == 1);
            CHECK(empty.first() == 1);
            CHECK(empty.last() == 1);
        }
        
        SECTION("to existing") {
            bs.append(false);

            CHECK(bs.size() == 5);
            CHECK(bs.last() == false);

            CHECK(verbose_equal(bs, bit_sequence<uint8_t>{true, false, true, true, false}));
        }

        SECTION("fold expr") {
            bs.append(true,false,true);

            CHECK(bs.size() == 7);
            CHECK(verbose_equal(bs, bit_sequence<uint8_t>{true, false, true, true, true, false, true}));
        }
    }

    SECTION("prepend") {
        SECTION("to empty") {
            empty.prepend(true);

            CHECK(empty.size() == 1);
            CHECK(empty.first() == true);
            CHECK(empty.last() == true);
        }
        
        SECTION("to existing") {
            bs.prepend(false);

            CHECK(bs.size() == 5);
            CHECK(bs.first() == false);
            CHECK(equal(bs, bit_sequence<uint8_t>{false, true, false, true, true}));
        }

        SECTION("fold expr") {
            bs.prepend(true, false, false);

            CHECK(bs.size() == 7);
            CHECK(equal(bs, bit_sequence<uint8_t>{false, false, true, true, false, true, true}));
        }
    }

    SECTION("insert") {
        SECTION("at beginning") {
            bs.insert_at(0, false);

            CHECK(bs.size() == 5);
            CHECK(equal(bs, bit_sequence<uint8_t>{false, true, false, true, true}));
        }
        
        SECTION("at middle") {
            bs.insert_at(2, false);
            bs.insert_at(2, true);

            CHECK(bs.size() == 6);
            CHECK(equal(bs, bit_sequence<uint8_t>{true, false, true, false, true, true}));
        }
        
        SECTION("at end") {
            bs.insert_at(4, false);

            CHECK(bs.size() == 5);
            CHECK(equal(bs, bit_sequence<uint8_t>{true, false, true, true, false}));
        }
        
        SECTION("to empty") {
            empty.insert_at(0, true);

            CHECK(empty.size() == 1);
            CHECK(empty.first() == true);
            CHECK(empty.last() == true);
        }
    }

    SECTION("chaining") {
        bit_sequence<uint8_t> bs;

        bs.append(true).append(false).prepend(true).append(true).insert_at(2, false);
        CHECK(verbose_equal(bs, bit_sequence<uint8_t>{true, true, false, false, true}));
    }
}

TEST_CASE("bit_seq iterators", "bit_seq"){
    bit_sequence<uint8_t> bs{(uint8_t)0b00110011, (uint8_t)0b00001111};
    const bit_sequence<uint8_t>& cref = bs;
    bool expected[] = {0,0,1,1,0,0,1,1, 0,0,0,0,1,1,1,1};

    SECTION("range-based for const") {
        int i = 0;
        for (const auto& el : bs) {
            CHECK(el == expected[i++]);
        }
        CHECK(i == 16);
        
        
        i = 0;

        for (const auto& el : cref) {
            CHECK(el == expected[i++]);
        }
        CHECK(i == 16);
    }

    SECTION("range-based for mutable") {
        int i = 0;
        for (auto el : bs) {
            el = false;
            ++i;
        }
        CHECK(i == 16);
        CHECK(equal(bs, bit_sequence<uint8_t>(16, false)));
    }

    SECTION("range-based copy") {
        int i = 0;
        for (auto el : cref) {
            el = false;
            ++i;
        }
        CHECK(i == 16);
        CHECK(equal(bs, expected));
    }

    SECTION("empty sequence iteration") {
        bit_sequence<uint8_t> empty;
        for (const auto& el : empty) {
            FAIL("iterating empty sequence");
        }
        SUCCEED();
    }

    SECTION("RA iterator"){

        auto it = bs.begin();
        auto end = bs.end();

        SECTION("operator+, operator+= and operator++") {
            CHECK(*(it+7) == 1);
            CHECK(*(it+8) == 0);
            it += 3;
            CHECK(*it);
            it++;
            CHECK(!*it);
            it+=8;
            CHECK(*it);
        }

        SECTION("operator-, operator-= and operator--") {
            CHECK(*(end-1));
            CHECK(!*(end-5));
            CHECK(!*(end-8));
            CHECK(*(end-9));
            end -= 4;
            CHECK(*end);
            end -= 5;
            CHECK(*end);

            --end; --end;
            CHECK(!*end);
        }

        SECTION("operator[]"){
            CHECK(it[3]);
            CHECK(!it[4]);
            CHECK(it[7]);
            CHECK(!it[8]);

            it += 10;
            
            CHECK(!it[-2]);
            CHECK(it[-3]);
            CHECK(!it[-5]);
            CHECK(it[-4]);
        }

        SECTION("difference between iterators") {
            CHECK((end - it) == 16);
            CHECK((end - (it+9)) == 7);
            CHECK((it - end) == -16);
        }

        SECTION("comparison") {
            auto it2 = it + 8;
            CHECK(it < it2);
            CHECK(it2 > it);
            CHECK(it <= it2);
            CHECK(it2 >= it);
            CHECK(it <= it);
            CHECK(it >= it);
        }
        SECTION("boundary behavior") {
            CHECK(it + bs.size() == end);
            CHECK(end - bs.size() == it);
            CHECK((++it + 7) == it + 8);
        }
    }
}

TEST_CASE("bit_seq clear", "[bit_seq]") {
    bit_sequence<uint64_t> bs{true, false, true, false, true};

    bs.clear();
    
    CHECK(bs.size() == 0);
    CHECK_THROWS(bs.first());
    CHECK_THROWS(bs.last());
}

TEST_CASE("bit_seq exceptions", "[bit_seq]") {    
    bit_sequence<uint64_t> bs{true, false, true, false, true};
    bit_sequence<uint64_t> empty;
    
    SECTION("at out of range") {
        CHECK_THROWS(bs.at(5));
        CHECK_THROWS(bs.at(234));
    }
    SECTION("insert out of range") {
        CHECK_THROWS(bs.insert_at(6, 1));
        CHECK_THROWS(bs.insert_at(123, 0));
    }
    SECTION("first empty") {
        CHECK_THROWS(empty.first());
    }
    SECTION("last empty") {
        CHECK_THROWS(empty.last());
    }
}

TEST_CASE("sequence get_subsequence", "[linked_list]"){    
    bit_sequence<uint8_t> bs{(uint8_t)0b00110110, (uint8_t)0b00110110};
    bit_sequence<uint8_t> sub;

    SECTION("middle sublist"){
        sub = bs.get_subsequence(5, 11);

        CHECK(verbose_equal(sub, bit_sequence<uint32_t>{true, true, false, false, false, true}));
    }
    SECTION("start sublist"){
        sub = bs.get_subsequence(0, 9);

        CHECK(verbose_equal(sub, bit_sequence<uint32_t>{false, false, true, true, false, true, true, false, false}));
    }
    SECTION("end sublist"){
        sub = bs.get_subsequence(6, 16);

        CHECK(verbose_equal(sub, bit_sequence<uint32_t>{true, false, false, false, true, true, false, true, true, false}));
    }
    SECTION("full sublist"){
        sub = bs.get_subsequence(0, 16);

        CHECK(verbose_equal(sub, bs));
    }
    SECTION("empty sublist"){
        auto sub1 = bs.get_subsequence(4, 4);
        auto sub2 = bs.get_subsequence(16, 16);
        auto sub3 = bs.get_subsequence(0, 0);

        CHECK(sub1.size() == 0);
        CHECK(sub2.size() == 0);
        CHECK(sub3.size() == 0);
    }
}