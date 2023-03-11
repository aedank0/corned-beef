//See LICENSE file for licensing info

#include "corned_beef.hpp"

#include <limits>
#include <cstdint>
#include <type_traits>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <functional>

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

TEST(BasicHashTest, StringHash)
{
    EXPECT_NE(cb::HashThis(std::string("test1\U0001f643")), cb::HashThis(std::string("test2\U0001f643")));
}

TEST(BasicHashTest, ASCII)
{
    EXPECT_NE((cb::HashThis<std::string, cb::HashASCII>(std::string("test1"))), (cb::HashThis<std::string, cb::HashASCII>(std::string("test2"))));
}

class HashCompareTest : public ::testing::Test
{
    protected:
    std::string str;

    void SetUp() override
    {
        std::ifstream f("large_text.txt");
        std::stringstream buffer;
        buffer << f.rdbuf();
        str = buffer.str();
    }
};

TEST_F(HashCompareTest, STLHash)
{
    EXPECT_NO_THROW(std::hash<std::string>{}(str));
}

TEST_F(HashCompareTest, CBHash)
{
    EXPECT_NO_THROW(cb::HashThis(str));
}
