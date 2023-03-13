/*
   Copyright 2023 Aedan Kearns

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "corned_beef.hpp"

#include <limits>
#include <cstdint>
#include <type_traits>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <functional>
#include <vector>
#include <iostream>
#include <tuple>
#include <vector>

#include "gtest/gtest.h"

namespace cb = corned_beef;

TEST(BasicHashTest, IntegralHash)
{
    EXPECT_NE(cb::HashThis(0), cb::HashThis(1));
    EXPECT_NE(cb::HashThis(1), cb::HashThis(2));
    EXPECT_NE(cb::HashThis(-1), cb::HashThis(2));
    EXPECT_NE(cb::HashThis(-1u), cb::HashThis(2u));
    EXPECT_NE(cb::HashThis(1ll), cb::HashThis(2ll));
    EXPECT_NE(cb::HashThis(-1ll), cb::HashThis(2ll));
    EXPECT_NE(cb::HashThis(-1ull), cb::HashThis(2ull));
    EXPECT_NE(cb::HashThis(std::numeric_limits<std::uint64_t>::max()), cb::HashThis(std::uint64_t(0)));
    EXPECT_NE(cb::HashThis(std::numeric_limits<std::uint64_t>::max()), cb::HashThis(std::uint64_t(1)));
    EXPECT_NE(cb::HashThis(std::numeric_limits<std::uint32_t>::max()), cb::HashThis(std::uint32_t(0)));
    EXPECT_NE(cb::HashThis(std::numeric_limits<std::uint32_t>::max()), cb::HashThis(std::uint32_t(1)));
}

TEST(BasicHashTest, FloatHash)
{
    EXPECT_NE(cb::HashThis(0.0f), cb::HashThis(1.0f));
    EXPECT_NE(cb::HashThis(1.0f), cb::HashThis(2.0f));
    EXPECT_NE(cb::HashThis(-1.0f), cb::HashThis(1.0f));
    EXPECT_NE(cb::HashThis(0.0), cb::HashThis(1.0));
    EXPECT_NE(cb::HashThis(1.0), cb::HashThis(2.0));
    EXPECT_NE(cb::HashThis(-1.0), cb::HashThis(1.0));
}

struct TestStruct
{
    char datas[5];
};

static_assert(std::is_trivially_copyable_v<TestStruct>, "Test struct must be trivially copyable");
static_assert(sizeof(TestStruct) == 5, "Sanity check for struct size failed");

TEST(BasicHashTest, TrivialHash)
{
    TestStruct a = { { 1, 2, 3, 4, 5 } };
    TestStruct b = { { 2, 3, 4, 5, 6 } };

    auto aHash = cb::HashThis(a), bHash = cb::HashThis(b);

    EXPECT_NE(aHash, bHash) << aHash << " != " << bHash;
}

TEST(BasicHashTest, UnorderedSet)
{
    std::unordered_set<int, cb::Hash<int>> testSet;

    EXPECT_NO_THROW(testSet.emplace(1));
    EXPECT_NO_THROW(testSet.emplace(5));
    EXPECT_NO_THROW(testSet.emplace(105));
    EXPECT_NO_THROW(testSet.emplace(1029487));
    EXPECT_NO_THROW(testSet.emplace(-12947));

    EXPECT_NE(testSet.find(5), testSet.end());
    EXPECT_NE(testSet.find(105), testSet.end());
    EXPECT_NE(testSet.find(1029487), testSet.end());
    EXPECT_NE(testSet.find(-12947), testSet.end());

    EXPECT_EQ(testSet.find(185892), testSet.end());
}

TEST(BasicHashTest, String)
{
    EXPECT_NE(cb::HashThis(std::string("test1\U0001f643")), cb::HashThis(std::string("test2\U0001f643")));
}

TEST(BasicHashTest, ASCII)
{
    EXPECT_NE((cb::HashThis<std::string, cb::HashASCII>(std::string("test1"))), (cb::HashThis<std::string, cb::HashASCII>(std::string("test2"))));
}

TEST(BasicHashTest, CStr)
{
    EXPECT_NE((cb::HashThis<const char*, cb::HashCStr>("test1\U0001f643")), (cb::HashThis<const char*, cb::HashCStr>("test2\U0001f643")));
}

TEST(BasicHashTest, CStrASCII)
{
    EXPECT_NE((cb::HashThis<const char*, cb::HashCStrASCII>("test1")), (cb::HashThis<const char*, cb::HashCStrASCII>("test2")));
}

TEST(BasicHashTest, Range)
{
    EXPECT_NO_THROW((cb::HashRange<std::vector<int>>{}({ 1, 539, 1230981, 123, 5873, 18346, 1577710 })));
}

class HashStringTest : public ::testing::Test
{
    protected:
    std::string str;
    std::vector<std::string> wordlist;

    void SetUp() override
    {
        std::ifstream f("large_text.txt");
        std::stringstream buffer;
        buffer << f.rdbuf();
        str = buffer.str();

        #ifdef CB_COMPILE_COLLISION_TESTS
        wordlist.reserve(69903);
        f = std::ifstream("wordlist.txt");
        while(f)
        {
            std::string s;
            f >> s;
            wordlist.emplace_back(s);
        }
        #endif
    }

    template<typename HashF>
    void testCollisions(const char* name, std::function<std::size_t(const std::string&)> stringHash)
    {
        std::unordered_set<std::size_t, HashF> prevSeen;
        std::size_t countSeen = 0;
        for (const auto& s : wordlist)
        {
            bool seen = false;
            std::tie(std::ignore, seen) = prevSeen.insert(stringHash(str));
            if (seen)
                ++countSeen;
        }

        std::cout << name << " had " << countSeen << " collisions out of " << wordlist.size() << " entries for a collision rate of " << static_cast<double>(countSeen) / wordlist.size() << std::endl;
    }
};

TEST_F(HashStringTest, STLHash)
{
    EXPECT_NO_THROW(std::hash<std::string>{}(str));
}

TEST_F(HashStringTest, CBHash)
{
    EXPECT_NO_THROW(cb::HashThis(str));
}

TEST_F(HashStringTest, CBHashASCII)
{
    EXPECT_NO_THROW((cb::HashThis<std::string, cb::HashASCII>(str)));
}
TEST_F(HashStringTest, CBHashCStr)
{
    EXPECT_NO_THROW((cb::HashThis<const char*, cb::HashCStr>(str.c_str())));
}

TEST_F(HashStringTest, CBHashCStrASCII)
{
    EXPECT_NO_THROW((cb::HashThis<const char*, cb::HashCStrASCII>(str.c_str())));
}

#ifdef CB_COMPILE_COLLISION_TESTS
TEST_F(HashStringTest, STLCollisions)
{
    testCollisions<std::hash<std::size_t>>("STL Hash", [](const std::string& s) { return std::hash<std::string>{}(s); });
}

TEST_F(HashStringTest, CBCollisions)
{
    testCollisions<std::hash<std::size_t>>("CB Hash", cb::HashThis<std::string, cb::Hash<std::string>>);
}

TEST_F(HashStringTest, CBCollisionsASCII)
{
    testCollisions<std::hash<std::size_t>>("CB ASCII Hash", cb::HashThis<std::string, cb::HashASCII>);
}
#endif

