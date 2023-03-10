//See LICENSE file for licensing info

#include "corned-beef.hpp"

#include <limits>
#include <cstdint>

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
