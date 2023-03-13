#ifndef CORNED_BEEF_HPP
#define CORNED_BEEF_HPP
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

#include <cstdint>
#include <concepts>
#include <bit>
#include <type_traits>
#include <cstring>
#include <string>
#include <limits>

//CB_XOR_OPERAND should be an integer if it is defined. If it is defined, the output of all hashes will be XOR'd by the value.
#ifdef CB_XOR_OPERAND
#define CB_XOR_VALUE(x) x ^ CB_XOR_OPERAND
#else
#define CB_XOR_VALUE(x) x
#endif

namespace corned_beef
{
    /**
     * @brief Hash struct
     * 
     * @tparam T Type
     */
    template<typename T>
    struct Hash
    {
        constexpr Hash() = default;
    };

    template<typename T>
    concept BasicValue = std::integral<T> || std::is_pointer_v<T>;

    /**
     * @brief Squishes a 64-bit number into a 32-bit one
     * 
     * @param v Value to be squished
     * @return std::uint32_t Result
     */
    constexpr std::uint32_t Squish64To32Bit(std::uint64_t v)
    {
        return (v & std::numeric_limits<std::uint32_t>::max()) ^ (v >> 32);
    }

    /**
     * @brief Hash for integral types
     * 
     * @tparam T Integral type
     */
    template<BasicValue T>
    struct Hash<T>
    {
        /**
         * @brief Hash function
         * 
         * @param val Value to be hashed
         * @return constexpr std::size_t Hashing result
         */
        constexpr std::size_t operator()(T val) const
        {
            if constexpr (sizeof(T) > sizeof(std::size_t))
                return CB_XOR_VALUE(static_cast<std::size_t>(Squish64To32Bit(static_cast<std::uint64_t>(val))));
            else
                return CB_XOR_VALUE(static_cast<std::size_t>(val));
        }
    };
    
    /**
     * @brief Hash for double
     */
    template<>
    struct Hash<double>
    {
        /**
         * @brief Hash function
         * 
         * @param val Value to be hashed
         * @return constexpr std::size_t Hashing result
         */
        constexpr std::size_t operator()(double val) const
        {
            if constexpr (sizeof(double) > sizeof(std::size_t))
                return CB_XOR_VALUE(static_cast<std::size_t>(Squish64To32Bit(std::bit_cast<std::uint64_t>(val))));
            else
                return CB_XOR_VALUE(std::bit_cast<std::size_t>(val));
        }
    };

    /**
     * @brief Hash for float
     */
    template<>
    struct Hash<float>
    {
        /**
         * @brief Hash function
         * 
         * @param val Value to be hashed
         * @return constexpr std::size_t Hashing result
         */
        constexpr std::size_t operator()(float val) const
        {
            return CB_XOR_VALUE(std::bit_cast<std::uint32_t>(val));
        }
    };

    /**
     * @brief Trivially copyable concept
     * 
     * @tparam T Type
     */
    template<typename T>
    concept TrivialCopy = std::is_trivially_copyable_v<T> && (sizeof(T) > 0) && !std::floating_point<T> && !std::integral<T>;

    //CONSTANT ZONE --- Use these instead of assuming 64-bit

    static constexpr std::size_t ONE_LESS_SIZE = sizeof(std::size_t) - 1;
    static constexpr std::size_t LOG2_SIZE = 2 | (sizeof(std::size_t) >> 3);
    static constexpr std::size_t SIZE_BITS = sizeof(std::size_t) >> LOG2_SIZE;
    static constexpr std::size_t SIZE_BITS_HALF = sizeof(std::size_t) >> 1;
    static constexpr std::size_t SIZE_BITS_SUB_ONE = SIZE_BITS - 1;
    static constexpr std::size_t SIZE_BITS_SUB_ONE_LSH_TWO = SIZE_BITS_SUB_ONE << 2;

    //END CONSTANT ZONE

    /**
     * @brief Calculates next number divisible by the size of std::size_t
     * 
     * @param val Value
     * @return consteval std::size_t Calculated value
     */
    static constexpr std::size_t NextNDivBySize(std::size_t val)
    {
        return (val + ONE_LESS_SIZE) & ~ONE_LESS_SIZE;
    }

    /**
     * @brief Combines two hashes together after first doing a circular rotation on b by rotAmt bits
     * 
     * @param a First hash
     * @param b Second hash
     * @param rotAmt Amount to rotate second hash before combining (in bits)
     * @return constexpr std::size_t Resulting hash
     */
    static constexpr std::size_t CombineHashes(std::size_t a, std::size_t b, int rotAmt = static_cast<int>(SIZE_BITS_HALF))
    {
        return a ^ std::rotl(b, rotAmt);
    }

    /**
     * @brief Hash for trivially copyable types.
     * 
     * @tparam T Trivially copyable type
     */
    template<TrivialCopy T>
    struct Hash<T>
    {
        /**
         * @brief Hash function
         * @warning Because this function must copy the entire contents of val to follow aliasing rules, it is more efficient to create a custom hash class, hash members individually, and use CombineHash to combine them.
         * @param val Value to be hashed
         * @return std::size_t Hashing result
         */
        constexpr std::size_t operator()(const T& val) const
        {
            const std::size_t size = NextNDivBySize(sizeof(T));
            const std::size_t count = size >> LOG2_SIZE;
            std::size_t data[count] = {0};

            std::memcpy(data, &val, sizeof(T));

            std::size_t retVal = data[0];

            for (size_t i = 1; i < count; ++i)
            {
                retVal = CombineHashes(retVal, data[i], i & SIZE_BITS);
            }

            return CB_XOR_VALUE(retVal);
        }
    };

    /**
     * @brief Hash for std::string
     */
    template<>
    struct Hash<std::string>
    {
        /**
         * @brief Hash fucntion
         * 
         * @param str String input
         * @return std::size_t Hashing result
         */
        constexpr std::size_t operator()(const std::string& str) const
        {
            std::size_t retVal = 0;

            for (std::size_t i = 0; i < str.size(); ++i)
            {
                retVal ^= std::rotl(static_cast<std::size_t>(str[i]), ((static_cast<std::uint_fast8_t>(i & ONE_LESS_SIZE) << 3) + (static_cast<std::uint_fast8_t>(i & SIZE_BITS_SUB_ONE_LSH_TWO) >> 2)));
            }

            return CB_XOR_VALUE(retVal);
        }
    };

    /**
     * @brief Hash for ascii only strings
     * 
     */
    struct HashASCII
    {
        /**
         * @brief Hash fucntion
         * @attention For use with strings only containing ASCII characters. Can be used with strings containing non-ASCII characters but theoretically may increase collisions.
         * @param str String input
         * @return std::size_t Hashing result
         */
        constexpr std::size_t operator()(const std::string& str) const
        {
            std::size_t retVal = 0;

            std::uint_fast8_t offset = 0;
            for (std::size_t i = 0; i < str.size(); ++i)
            {
                retVal ^= std::rotl(static_cast<std::size_t>(str[i]), offset);
                offset = (offset + 7) & SIZE_BITS_SUB_ONE;
            }

            return CB_XOR_VALUE(retVal);
        }
    };

    /**
     * @brief Hash for c-strings
     * 
     */
    struct HashCStr
    {
        /**
         * @brief Hash fucntion
         * 
         * @param str C-String input
         * @return std::size_t Hashing result
         */
        constexpr std::size_t operator()(const char* str) const
        {
            std::size_t retVal = 0;

            for (std::size_t i = 0; str[i]; ++i)
            {
                retVal ^= std::rotl(static_cast<std::size_t>(str[i]), ((static_cast<std::uint_fast8_t>(i & ONE_LESS_SIZE) << 3) + (static_cast<std::uint_fast8_t>(i & SIZE_BITS_SUB_ONE_LSH_TWO) >> 2)));
            }

            return CB_XOR_VALUE(retVal);
        }
    };

    /**
     * @brief Hash for ascii only c-strings
     * 
     */
    struct HashCStrASCII
    {
        /**
         * @brief Hash fucntion
         * @attention For use with strings only containing ASCII characters. Can be used with strings containing non-ASCII characters but theoretically may increase collisions.
         * @param str C-string input
         * @return std::size_t Hashing result
         */
        constexpr std::size_t operator()(const char* str) const
        {
            std::size_t retVal = 0;

            std::uint_fast8_t offset = 0;
            for (std::size_t i = 0; str[i]; ++i)
            {
                retVal ^= std::rotl(static_cast<std::size_t>(str[i]), offset);
                offset = (offset + 7) & SIZE_BITS_SUB_ONE;
            }

            return CB_XOR_VALUE(retVal);
        }
    };

    /**
     * @brief Hashes given value of type T with hasher of type HashType
     * 
     * @tparam T Type of value to be hashed
     * @tparam HashType Type of hasher
     * @param x value to be hashed
     * @return std::size_t Hash value
     */
    template<typename T, typename HashType = Hash<T>>
        requires requires (T x)
        {
            { HashType{}(x) } -> std::same_as<std::size_t>;
        }
    static constexpr std::size_t HashThis(const T& val)
    {
        return HashType{}(val);
    }
}

#endif