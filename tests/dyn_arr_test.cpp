#include "dynamic_array.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

TEST_CASE("dyn_arr constructors", "[dyn_arr]"){

    int arr[] = {1,2,3};

    SECTION("default constructor"){
        dynamic_array<int> darr_def;
        CHECK(darr_def.size() == 0);
    }

    SECTION("size constructor"){
        dynamic_array<int> darr_size(5);
        CHECK(darr_size.size() == 5);
    }
    
    SECTION("full constructor"){
        dynamic_array<int> darr_full(arr, 3);

        REQUIRE(darr_full.size() == 3);
        size_t i = 0;
        int arr_full[3];
        for(auto el: darr_full){
            arr_full[i++] = el;
        }

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_full)));
    }
    
    SECTION("copy constructor"){
        dynamic_array<int> darr_full(arr, 3);
        dynamic_array<int> darr_copy(darr_full);

        REQUIRE(darr_copy.size() == 3);
        REQUIRE(darr_full.size() == 3);

        
        size_t i = 0;
        int arr_copy[3];
        for(auto el: darr_copy){
            arr_copy[i++] = el;
        }

        darr_full[0] = 342;
        darr_full[2] = 4;
        i = 0;
        int arr_copied_from[3];
        for(auto el: darr_full){
            arr_copied_from[i++] = el;
        }
        int new_arr[] = {342, 2, 4};

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_copy)));
        CHECK(std::equal(std::begin(new_arr), std::end(new_arr), std::begin(arr_copied_from)));
    }

    SECTION("move constructor"){
        dynamic_array<std::string> darr = {"123", "asdad", "r123r1"};
        auto old = {"123", "asdad", "r123r1"};
        auto darr_move(std::move(darr));

        CHECK(darr.size() == 0);
        CHECK(std::equal(std::begin(old), std::end(old), std::begin(darr_move)));
    }

    SECTION("initializer list"){
        dynamic_array<int> darr = {1,2,3,4};
        int check_arr[] = {1,2,3,4};

        CHECK(std::equal(std::begin(check_arr), std::end(check_arr), darr.begin()));
    }
}

TEST_CASE("dyn_arr operator=", "[dyn_arr]"){

    float arr[] = {1.123,2.4214,3.412};
    dynamic_array<float> darr(arr, 3);
        
    SECTION("copy assignment"){
        SECTION("same sizes"){
            dynamic_array<float> darr_copy = {1,2,3};
            darr_copy = darr;
    
            REQUIRE(darr_copy.size() == 3);
            REQUIRE(darr.size() == 3);
    
            darr[0] = -2;
            float new_arr[] = {-2, arr[1], arr[2]};
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr_copy)));
            CHECK(std::equal(std::begin(new_arr), std::end(new_arr), std::begin(darr)));
        }
        SECTION("different sizes"){
            dynamic_array<float> darr_copy;
            darr_copy = darr;
    
            REQUIRE(darr_copy.size() == 3);
            REQUIRE(darr.size() == 3);
    
            darr[0] = -2;
            float new_arr[] = {-2, arr[1], arr[2]};
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr_copy)));
            CHECK(std::equal(std::begin(new_arr), std::end(new_arr), std::begin(darr)));
        }
    }
    SECTION("move assignment"){
        SECTION("same sizes"){
            dynamic_array<float> darr_copy = {1,2,3};
            darr_copy = std::move(darr);
    
            REQUIRE(darr_copy.size() == 3);
            REQUIRE(darr.size() == 0);
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr_copy)));
            CHECK_THROWS_AS(darr.at(0), std::out_of_range);
        }
        SECTION("different sizes"){
            dynamic_array<float> darr_copy;
            darr_copy = std::move(darr);
    
            REQUIRE(darr_copy.size() == 3);
            REQUIRE(darr.size() == 0);
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr_copy)));
            CHECK_THROWS_AS(darr.at(0), std::out_of_range);
        }
    }

    SECTION("self-assignment"){
        SECTION("copy"){
            darr = darr;
    
            REQUIRE(darr.size() == 3);        
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr)));
        }
        SECTION("move"){
            darr = std::move(darr);
    
            REQUIRE(darr.size() == 3);        
    
            CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(darr)));
        }
    }
}

TEST_CASE("dyn_arr at method exceptions", "[dyn_arr][exceptions]"){
    int arr[] = {1,3,3,7};
    dynamic_array<int> darr(arr, 4);
    const dynamic_array<int>& darrref = darr;

    SECTION("at out_of_range"){
        CHECK_THROWS_AS(darr.at(4), std::out_of_range);
        CHECK_THROWS_AS(darr.at(-1), std::out_of_range);
    }
    SECTION("cosnt at out_of_range"){
        CHECK_THROWS_AS(darrref.at(4), std::out_of_range);
        CHECK_THROWS_AS(darrref.at(-1), std::out_of_range);
    }
}

TEST_CASE("dyn_arr resize", "[dyn_arr]"){
    int arr[] = {1,23,41,54};
    dynamic_array<int> darr = dynamic_array(arr, 4);

    SECTION("reduce"){
        size_t new_size = 2;
        darr.resize(new_size);

        CHECK(darr.size() == new_size);

        CHECK(darr[0] == arr[0]);        
        CHECK(darr[1] == arr[1]);        
    }
    

    SECTION("expand"){
        size_t new_size = 6;
        darr.resize(new_size);

        CHECK(darr.size() == new_size);

        CHECK(darr.at(0) == arr[0]);        
        CHECK(darr.at(1) == arr[1]);        
        CHECK(darr.at(2) == arr[2]);        
        CHECK(darr.at(3) == arr[3]);        
    }

    SECTION("same size"){
        size_t new_size = 4;
        darr.resize(new_size);

        CHECK(darr.size() == new_size);

        CHECK(darr.at(0) == arr[0]);        
        CHECK(darr.at(1) == arr[1]);        
        CHECK(darr.at(2) == arr[2]);        
        CHECK(darr.at(3) == arr[3]);  
    }
}

TEST_CASE("dyna_arr operator[]/at", "[dyn_arr]"){
    char arr[] = {1,2,3,4};
    dynamic_array<char> darr(arr, 4);
    const auto& crdarr = darr;

    SECTION("read-only"){
        SECTION("operator[]"){
            CHECK(crdarr[0]==1);
            CHECK(crdarr[1]==2);
            CHECK(crdarr[3]==4);
        }
        SECTION("at"){
            CHECK(crdarr.at(0)==1);
            CHECK(crdarr.at(1)==2);
            CHECK(crdarr.at(3)==4);
        }
    }
    SECTION("rw"){
        SECTION("operator[]"){
            CHECK(darr[0] == 1);
            CHECK(darr[1] == 2);
            CHECK(darr[3] == 4);
    
            darr[0] = 'p';
            darr[1] = 'q';
            darr[3] = 'r';
    
            CHECK(darr[0] == 'p');
            CHECK(darr[1] == 'q');
            CHECK(darr[3] == 'r');
        }
        SECTION("at"){
            CHECK(darr.at(0) == 1);
            CHECK(darr.at(1) == 2);
            CHECK(darr.at(3) == 4);
    
            darr.at(0) = 'p';
            darr.at(1) = 'q';
            darr.at(3) = 'r';
    
            CHECK(darr.at(0) == 'p');
            CHECK(darr.at(1) == 'q');
            CHECK(darr.at(3) == 'r');
        }
    }
}