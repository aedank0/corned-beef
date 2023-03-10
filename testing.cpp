//See LICENSE file for licensing info

#include "corned_beef.hpp"

#include <limits>
#include <cstdint>
#include <type_traits>

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
