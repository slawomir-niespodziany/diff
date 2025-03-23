#pragma once

/**
 * @file Config.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines Config used configure component instances.
 * @version 0.1
 * @date 2024-12-06
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <diff/CastChecker.h>
#include <diff/Demangler.h>
#include <diff/Exception.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

using namespace std::string_literals;

namespace diff {

template <typename T = void>
class ConfigEntry;

/**
 * @brief Abstract base class, a common interface for instantiations of ConfigEntry<T != void>. Multiple ConfigEntry objects compose a Config.
 */
template <>
class ConfigEntry<void> {
public:
    virtual ~ConfigEntry() = default;

    /**
     * @brief Return entry key.
     *
     * @return Entry key reference.
     */
    const std::string& key() const noexcept { return key_; }

    /**
     * @brief Return entry value. Value is casted to a type determined by T.
     * @exception ConfigEntryCastError If value can not be casted to the given type (dependent on the actual entry type and value, @see
     * ConfigEntryCastError).
     *
     * @tparam T Requested type.
     * @return Value reference.
     */
    template <typename T>
    const T& value() const {
        static_assert(!std::is_const<T>::value, "Unable to distinguish const type.");   // TODO
        static_assert(!std::is_volatile<T>::value, "Unable to distinguish volatile type.");

        return *static_cast<const T*>(value(typeid(T)));
    }

    /**
     * @brief Return entry type name.
     *
     * @return Reference to entry type name.
     */
    virtual const std::string& type() const noexcept = 0;

    /**
     * @brief Return string representation of entry value.
     * For std::string type it is the value itsself. For integral types it is the equivalent of std::to_string().
     *
     * @return String representation of entry value.
     */
    virtual std::string toString() const noexcept = 0;

protected:
    ConfigEntry(const std::string& key) : key_{key} {}

    /**
     * @brief Return const void pointer to entry value variable.
     * @exception ConfigEntryCastError If the underlying variable can not be reinterpreted as the type indicated by typeInfo.
     *
     * @param typeInfo Indicator of the type for the variable to be reinterpreted as.
     * @return Variable pointer.
     */
    virtual const void* value(const std::type_info& typeInfo) const = 0;

    const std::string key_;
};

/**
 * @brief Consists of key and value for std::string type entry.
 */
template <>
class ConfigEntry<std::string> : public ConfigEntry<> {
public:
    /**
     * @brief Instantiate an entry for type std::string.
     *
     * @param key Entry key.
     * @param value Entry value.
     */
    ConfigEntry(const std::string& key, const std::string& value) : ConfigEntry<>(key), value_{value} {}
    virtual ~ConfigEntry() = default;

    /**
     * @brief @see ConfigEntry<void>
     */
    virtual const std::string& type() const noexcept override { return Demangler::of<std::string>(); }

    /**
     * @brief @see ConfigEntry<void>
     */
    virtual std::string toString() const noexcept override { return value_; }

protected:
    /**
     * @brief @see ConfigEntry<void>
     */
    virtual const void* value(const std::type_info& typeInfo) const override {
        if (typeid(std::string) != typeInfo) {
            throw ConfigEntryCastError(key_, toString(), Demangler::of<std::string>(), Demangler::from(typeInfo.name()));
        }

        return static_cast<const void*>(&value_);
    }

private:
    const std::string value_;
};

/**
 * @brief Consists of key and value for integral type entry - @see std::is_integral.
 */
template <typename T>
class ConfigEntry : public ConfigEntry<> {
public:
    static_assert(std::is_integral<T>::value, "ConfigEntry template parameter T shall be integral.");
    static_assert(!std::is_const<T>::value, "ConfigEntry template parameter T shall not be const.");
    static_assert(!std::is_volatile<T>::value, "ConfigEntry template parameter T shall not be volatile.");

    /**
     * @brief Instantiate an entry for integral type.
     *
     * @param key Entry key.
     * @param value Entry value.
     */
    ConfigEntry(const std::string& key, const T& value) : ConfigEntry<>(key), value_{value} {}
    virtual ~ConfigEntry() = default;

    /**
     * @brief @see ConfigEntry<void>
     */
    virtual const std::string& type() const noexcept override { return Demangler::of<T>(); }

    /**
     * @brief @see ConfigEntry<void>
     */
    virtual std::string toString() const noexcept override {
        if (std::is_same<bool, T>::value) {
            return value_ ? ("true"s) : ("false"s);
        } else {
            return std::to_string(value_);
        }
    }

protected:
    /**
     * @brief @see ConfigEntry<void>
     */
    virtual const void* value(const std::type_info& typeInfo) const override {
        if (!IntegralCastChecker::check(value_, typeInfo)) {
            throw ConfigEntryCastError(key_, toString(), Demangler::of<T>(), Demangler::from(typeInfo.name()));
        }

        return static_cast<const void*>(&value_);
    }

private:
    const T value_;
};

inline bool operator<(const std::unique_ptr<const ConfigEntry<>>& pFirst, const std::unique_ptr<const ConfigEntry<>>& pSecond) {
    return pFirst->key() < pSecond->key();
}
inline bool operator<(const std::unique_ptr<const ConfigEntry<>>& pEntry, const std::string& key) { return pEntry->key() < key; }
inline bool operator<(const std::string& key, const std::unique_ptr<const ConfigEntry<>>& pEntry) { return key < pEntry->key(); }

/**
 * @brief Config for an instance of a component. Collection of ConfigEntry<T> objects. Entry keys are unique within a Config object.
 */
using Config = std::set<std::unique_ptr<const ConfigEntry<>>, std::less<>>;

}   // namespace diff
