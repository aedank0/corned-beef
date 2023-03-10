#ifndef CORNED_BEEF_HPP
#define CORNED_BEEF_HPP

#include <cstdint>
#include <concepts>
#include <bit>
#include <type_traits>
#include <cstring>

static_assert(sizeof(std::size_t) == 8, "Not on a 64-bit system");

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

    /**
     * @brief Hash for integral types
     * 
     * @tparam T Integral type
     */
    template<std::integral T>
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
            return static_cast<std::size_t>(val);
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
            return std::bit_cast<std::size_t>(val);
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
            return std::bit_cast<std::uint32_t>(val);
        }
    };

    /**
     * @brief Trivially copyable concept
     * 
     * @tparam T Type
     */
    template<typename T>
    concept TrivialCopy = std::is_trivially_copyable_v<T> && (sizeof(T) > 0) && !std::floating_point<T> && !std::integral<T>;

    /**
     * @brief Calculates next number divisible by 8
     * 
     * @param val Value
     * @return consteval std::size_t Calculated value
     */
    static constexpr std::size_t NextNDivBy8(std::size_t val)
    {
        return (val + 7) & ~7;
    }

    /**
     * @brief Combines two hashes together
     * 
     * @param a First hash
     * @param b Second hash
     * @param rotAmt Amount to rotate second hash before combining
     * @return constexpr std::size_t Resulting hash
     */
    static constexpr std::size_t CombineHashes(std::size_t a, std::size_t b, int rotAmt = 32)
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
        constexpr std::size_t operator()(T val) const
        {
            const std::size_t size = NextNDivBy8(sizeof(T));
            const std::size_t count = size >> 3;
            std::size_t data[count] = {0};

            std::memcpy(data, &val, sizeof(T));

            std::size_t retVal = data[0];

            for (size_t i = 1; i < count; ++i)
            {
                retVal = CombineHashes(retVal, data[i], i & 64);
            }

            return retVal;
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