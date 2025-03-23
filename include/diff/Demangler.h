#pragma once

/**
 * @file Demangler.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines Demangler class used to determine demangled type names.
 * @version 0.1
 * @date 2024-11-15
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <cxxabi.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std::string_literals;

namespace diff {

/**
 * @brief Demangler provides demangling functionality for type names. It is not able to distinguish for <b>const</b> and <b>volatile</b> specifiers.
 */
class Demangler {
public:
    /**
     * @brief Return demangled name of the type specified in function template parameter. The string is constant, statically allocated and initialized
     * at the first call to this function. Consecutive calls return a reference to the same string object. The function does not distinguish const and
     * volatile specifiers.
     *
     * @tparam T Requested type.
     * @return Reference to a string with demangled name of the requested type (or the mangled name if demangling fails).
     */
    template <typename T>
    static const std::string &of() {
        static_assert(!std::is_const<T>::value, "Unable to distinguish const type.");
        static_assert(!std::is_volatile<T>::value, "Unable to distinguish volatile type.");

        static const std::string demangled = demangle(typeid(T).name());
        return demangled;
    }

    /**
     * @brief Return string corresponding to demangled name of the given mangled type name. The function does not distinguish const and volatile
     * specifiers.
     *
     * @param mangled Mangled type name.
     * @return Demangled type name, or the original mangled name if demangling fails.
     */
    static std::string from(const std::string &mangled) { return demangle(mangled); }

private:
    static std::string demangle(const std::string &mangled) {
        std::string demangled = mangled;

        int status = 0;
        char *pResult = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
        if (nullptr != pResult) {
            if (0 == status) {
                demangled = std::string(pResult);
            }
            free(pResult);
        }

        return demangled;
    }
};

}   // namespace diff
