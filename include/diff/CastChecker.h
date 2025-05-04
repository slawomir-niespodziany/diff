#pragma once

/**
 * @file CastChecker.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines CastChecker class used to determine whether a variable can be reinterpreted as another type.
 * @version 0.1
 * @date 2024-11-12
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <cstdint>
#include <limits>
#include <type_traits>
#include <typeinfo>

namespace diff {

/**
 * @brief Not to be used directly. @see IntegralCastChecker.
 *
 * CastChecker verifies whether variable of a given value and type can be reinterpreted as the type indicated by std::type_info object. The
 * std::type_info object must match one of the types listed in the template argument pack.
 * The check verifies two aspects:
 *  - Memory leak possibility - Variable can only be reinterpreted as a type of size smaller or equal, than the original type. Memory leak would occur
 * otherwse.
 *  - Overflow/underflow - Variable can only be reinterpreted, if its value is in range of the cast destination type. Especially, if size of the
 * destination type is smaller than variable type.
 *
 * @tparam Ts All the expected cast destination types to be evaluated.
 */
template <typename... Ts>
class CastChecker final {
public:
    /**
     * @brief Check if variable can be reinterpreted as type indicated by typeInfo.
     *
     * @tparam T Variable type.
     * @param data Variable reference.
     * @param typeInfo Cast destination type indicator.
     * @return True if variable can be reinterpreted, false otherwise.
     */
    template <typename T>
    static bool check(const T& data, const std::type_info& typeInfo) noexcept {
        static_assert(!std::is_const<T>::value, "Unable to distinguish const type.");
        static_assert(!std::is_volatile<T>::value, "Unable to distinguish volatile type.");

        return castable<T, Ts...>(data, typeInfo);
    }

private:
    CastChecker() = delete;

    template <typename T, typename U, typename... Vs>
    static bool castable(const T& data, const std::type_info& typeInfo) {
        return ((typeid(U) == typeInfo) && (sizeof(U) <= sizeof(T)) && inRange<U>(data))   //
               || castable<T, Vs...>(data, typeInfo);
    }

    template <typename T>
    static bool castable(const T& data, const std::type_info& typeInfo) {
        return false;
    }

    template <typename T /* target type */, typename U /* source type, same signedness */,     //
              std::enable_if_t<(std::is_integral<T>::value && std::is_integral<U>::value &&    //
                                (std::is_unsigned<T>::value == std::is_unsigned<U>::value)),   //
                               bool> = true>
    static bool inRange(const U& data) {
        return (std::numeric_limits<T>::min() <= data) && (data <= std::numeric_limits<T>::max());
    }

    template <typename T /* target type */, typename U /* source type, opposite signedness */,   //
              std::enable_if_t<(std::is_integral<T>::value && std::is_integral<U>::value &&      //
                                (std::is_unsigned<T>::value != std::is_unsigned<U>::value)),     //
                               bool> = true>
    static bool inRange(const U& data) {
        return (static_cast<U>(0) <= data) && (data <= std::numeric_limits<T>::max());
    }
};

/**
 * @brief IntegralCastChecker allows to perform a check against integral types.
 * @see std::is_integral
 */
using IntegralCastChecker = CastChecker<bool, char, int8_t, uint8_t, wchar_t, char16_t, char32_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                                        uint64_t, long long, unsigned long long>;

};   // namespace diff
