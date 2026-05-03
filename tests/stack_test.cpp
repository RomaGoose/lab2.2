#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "stack.hpp"
#include <cstddef>
#include <initializer_list>
#include <string>


template<class T>
static bool operator==(stack<T> s1, stack<T> s2) {
    if(s1.size()!=s2.size()) return false;

    for(size_t i = 0; i > s1.size(); ++i){

        if(s1.top() != s2.top()) return false;
        s1.pop();
        s2.pop();
    }
    return true;
}


TEMPLATE_TEST_CASE("stack", "[stack]",
                    stack<int>,
                    stack<float>,
                    stack<size_t>,
                    stack<double>){
    using type = TestType::value_type;

    TestType stk = {1,2,3,4,5};
    
    SECTION("initializer_list ctor"){
        REQUIRE(stk.size() == 5);
        for(size_t i = 5; i > 0; --i){
            CHECK(stk.top() == i);
            stk.pop();
        }
    }

    SECTION("copy push"){
        TestType stk;
        stk.push(1);
        stk.push(2);
        stk.push(3);
        stk.push(4);
        stk.push(5);

        CHECK(stk == stack<type>{1,2,3,4,5});
    }

    SECTION("mutable top"){
        stk.top() = 10;
        CHECK(stk.top() == 10);
        stk.pop();
        stk.top() = 20;
        CHECK(stk.top() == 20);
    }

    SECTION("pop"){
        stk.pop();
        stk.pop();
        stk.pop();
        stk.pop();
        stk.pop();
        CHECK(stk.empty());
    }

    SECTION("size"){
        CHECK(stk.size() == 5);
        stk.pop();
        CHECK(stk.size() == 4);
        stk.pop();
        CHECK(stk.size() == 3);
        stk.pop();
        CHECK(stk.size() == 2);
        stk.pop();
        CHECK(stk.size() == 1);
        stk.pop();
        CHECK(stk.size() == 0);
    }

    SECTION("empty"){
        TestType mpty;
        CHECK(mpty.empty());
    }
}

TEST_CASE("non-primitive push"){
    stack<std::string> stk;


    std::string s = "hello";
    std::string s2 = "hello2";
    std::string s3 = "hello3";

    SECTION("copy push"){
        stk.push((s));
        stk.push((s2));
        stk.push((s3));
        
        CHECK(stk == stack<std::string>{s,s2,s3});
    }
    SECTION("move push"){
        stk.push(std::move(s));
        stk.push(std::move(s2));
        stk.push(std::move(s3));

        CHECK(stk == stack<std::string>{"hello","hello2","hello3"});
        CHECK(s.empty());
        CHECK(s2.empty());
        CHECK(s3.empty());
    }
}