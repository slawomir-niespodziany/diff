#pragma once

/**
 * @file Exception.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines diff exception base type.
 * @version 0.1
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 */

#include <stdexcept>

using namespace std::string_literals;

namespace diff {

/**
 * @brief Base exception type used across diff.
 */
class Exception : public std::logic_error {
protected:
    Exception(const std::string& what) : std::logic_error{what} {}
};

struct DependencyRegisterNotFound : public Exception {
    DependencyRegisterNotFound(const std::string& type, const std::string& id) : Exception{"Dependency "s + type + "{} with id=\""s + id + "\" not found."s} {}
};
struct DependencyDuplicated : public Exception {
    DependencyDuplicated(const std::string& type, const std::string& id)
        : Exception{"Dependency "s + type + "{} already registered with id=\""s + id + "\"."s} {}
};

struct DependencyNotFound : public Exception {
    DependencyNotFound(const std::string& type, const std::string& id) : Exception{"Dependency "s + type + "{} with id=\""s + id + "\" not found."s} {}
};

/**
 * @brief Thrown if factory for a certain component type name is requested, but no such factory has been registered.
 */
struct FactoryNotFound : public Exception {
    FactoryNotFound(const std::string& type) : Exception{"Factory of "s + type + "{} not registered."s} {}
};

/**
 * @brief Thrown if config already consist of an entry with the given key.
 */
struct ConfigEntryKeyDuplicated : public Exception {
    ConfigEntryKeyDuplicated(const std::string& key) : Exception{key} {}
};

/**
 * @brief Thrown if config consists of no entry with the given key.
 */
struct ConfigEntryNotFound : public Exception {
    ConfigEntryNotFound(const std::string& type, const std::string& id, const std::string& key)
        : Exception{"TODO CONFIG ENTRY NOT FOUND FOR "s + type + id + key} {}
};

/**
 * @brief Thrown if an entry is requested from the config, but the underlying value can not be represented by the requested type.
 * Applies to two scenarios - @see CastChecker:
 * - The underlying type is too short to be reinterpreted as the requested type (e.g. value is an uint8_t and the user attempts to read it as
 * uint32_t - a memory leak would occur).
 * - The underlying value is out of range of the requested type (e.g. value is an uint32_t{1024u}, but the user attempts to read it as uint8_t).
 */
struct ConfigEntryCastError : public Exception {
    ConfigEntryCastError(const std::string& key, const std::string& value, const std::string& sourceType, const std::string& targetType)
        : Exception{"Could not cast config entry \""s + key + "\" from "s + sourceType + "{"s + value + "} to "s + targetType + "."s} {}
};

/**
 * @brief Thrown if factory for a certain component type name is requested, but no such factory has been registered.
 */
struct ComponentIdDuplicated : public Exception {
    ComponentIdDuplicated(const std::string& type, const std::string& id)
        : Exception{"Component id duplicated for component "s + type + "{\""s + id + "\"}."s} {}
};

class TopologyLoaderException : public diff::Exception {
public:
    TopologyLoaderException(const std::string& what) : diff::Exception{what} {}
};

}   // namespace diff
