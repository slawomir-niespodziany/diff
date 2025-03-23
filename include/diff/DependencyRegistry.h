#pragma once

/**
 * @file DependencyRegistry.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines DependencyRegistry class used to keep track of the dependencies available during component instantiation.
 * @version 0.1
 * @date 2025-03-03
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Demangler.h>
#include <diff/Exception.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>

namespace diff {

template <typename T = void>
class DependencyRegister;

/**
 * @brief Abstract base class, a common interface for instantiations of DependencyRegister<T != void>.
 */
template <>
class DependencyRegister<void> {
public:
    virtual ~DependencyRegister() = default;

    /**
     * @brief Return type name of dependencies registered within this register.
     *
     * @return Type name reference.
     */
    const std::string &type() const noexcept { return type_; }

    /**
     * @brief Return number of dependencies registered within this register.
     *
     * @return Number of registered dependencies.
     */
    virtual std::size_t size() const noexcept = 0;

    /**
     * @brief Return a vector of identifiers of all dependencies registered within this register.
     *
     * @return Vector of identifiers.
     */
    virtual std::vector<std::reference_wrapper<const std::string>> all() const noexcept = 0;

    /**
     * @brief Print register content.
     *
     * @param os Output stream to be printed on.
     * @param dependencyRegister Register to be printed out.
     * @return Reference to os.
     */
    friend std::ostream &operator<<(std::ostream &os, DependencyRegister<> &dependencyRegister) {
        const std::string &type = dependencyRegister.type();
        const auto all = dependencyRegister.all();

        auto it0 = all.cbegin();
        auto it1 = std::next(it0);

        while (all.cend() != it0) {
            os << type << "{"s << it0->get() << "}"s;

            if (all.cend() != it1) {
                os << '\n';
            }

            it0 = std::move(it1);
            it1 = std::next(it0);
        }

        return os;
    }

protected:
    DependencyRegister(const std::string &type) : type_{type} {}

    const std::string &type_;
};

/**
 * @brief Keeps a record of a dependencies of a single type. Does not own the registered dependencies.
 *
 * @tparam T Dependency type.
 */
template <typename T>
class DependencyRegister final : public DependencyRegister<> {   // TODO Abstract class? int? array? etc??
public:
    /**
     * @brief Instantiate empty register.
     */
    DependencyRegister() : DependencyRegister<>(Demangler::of<T>()) {}
    ~DependencyRegister() = default;

    /**
     * @brief Register dependency.
     * @exception DependencyDuplicated If dependency with the given id is already registered.
     *
     * @param id Dependency id.
     * @param dependency Dependency reference.
     */
    void add(const std::string &id, T &dependency) {
        if (0u != dependencies_.count(id)) {
            throw DependencyDuplicated(Demangler::of<T>(), id);
        }
        dependencies_.emplace(id, dependency);
    }

    /**
     * @brief @see DependencyRegister<void>
     */
    std::size_t size() const noexcept override { return dependencies_.size(); }

    /**
     * @brief @see DependencyRegister<void>
     */
    std::vector<std::reference_wrapper<const std::string>> all() const noexcept override {
        std::vector<std::reference_wrapper<const std::string>> result;
        result.reserve(dependencies_.size());

        for (const auto &kv : dependencies_) {
            result.emplace_back(kv.first);
        }

        return result;
    }

    /**
     * @brief Indicate whether a dependency of the given id is registered.
     *
     * @param id Dependency id.
     * @return True if dependency is registered, false otherwise.
     */
    bool has(const std::string &id) const noexcept { return (0u != dependencies_.count(id)); }

    /**
     * @brief Return references to all registered dependencies.
     *
     * @return Vector of references.
     */
    std::vector<std::reference_wrapper<T>> get() const noexcept {
        std::vector<std::reference_wrapper<T>> result;
        result.reserve(dependencies_.size());

        for (const auto &kv : dependencies_) {
            result.emplace_back(kv.second);
        }

        return result;
    }

    /**
     * @brief Return reference to the dependency of a given id.
     * @exception DependencyNotFound If no dependency of the requested id is registered.
     *
     * @param id Dependency id.
     * @return Dependency reference.
     */
    T &get(const std::string &id) const {
        const auto it = dependencies_.find(id);
        if (dependencies_.cend() == it) {
            throw DependencyNotFound(Demangler::of<T>(), id);
        }
        return it->second;
    }

private:
    std::map<std::reference_wrapper<const std::string>, std::reference_wrapper<T>, std::less<std::string>> dependencies_;
};

inline bool operator<(const std::unique_ptr<DependencyRegister<>> &pFirst, const std::unique_ptr<DependencyRegister<>> &pSecond) {
    return pFirst->type() < pSecond->type();
}
inline bool operator<(const std::unique_ptr<DependencyRegister<>> &pDependencyRegister, const std::string &type) {
    return pDependencyRegister->type() < type;
}
inline bool operator<(const std::string &type, const std::unique_ptr<DependencyRegister<>> &pDependencyRegister) {
    return type < pDependencyRegister->type();
}

/**
 * @brief Keeps a record of dependency of multiple types. Aggregates multiple DependencyRegisters and provides an interface for accessing any
 * previously registered dependency. Does not own the registered dependencies.
 */
class DependencyRegistry final {
public:
    /**
     * @brief Instantiate empty registry.
     */
    DependencyRegistry() = default;
    ~DependencyRegistry() = default;

    /**
     * @brief Register dependency.
     * @exception DependencyDuplicated If dependency with the given type and id is already registered.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @param id Dependency id.
     * @param dependency Dependency reference.
     */
    template <typename T, std::enable_if_t<!std::is_const<T>::value && !std::is_volatile<T>::value, bool> = true>   // TODO is pointer, reference,
                                                                                                                    // array? ABSTRACT CLASS?
    void add(const std::string &id, T &dependency) {
        auto it = dependencyRegisters_.find(Demangler::of<T>());
        if (dependencyRegisters_.cend() == it) {
            std::tie(it, std::ignore) = dependencyRegisters_.emplace(std::make_unique<DependencyRegister<T>>());
        }

        DependencyRegister<T> &dependencyRegister = static_cast<DependencyRegister<T> &>(**it);
        dependencyRegister.add(id, dependency);
    }

    /**
     * @brief Return information (component type name and instance id) about all registered dependencies.
     *
     * @return Vector of pairs of std::string references. One pair for each dependency. Each pair consists of a reference to dependency type name and
     * dependency instance id.
     */
    std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<const std::string>>> all() const {
        std::size_t n = 0;
        for (const auto &dependencyRegister : dependencyRegisters_) {
            n += dependencyRegister->size();
        }

        std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<const std::string>>> result;
        result.reserve(n);

        for (const auto &dependencyRegister : dependencyRegisters_) {
            const std::string &type = dependencyRegister->type();
            for (const std::string &id : dependencyRegister->all()) {
                result.emplace_back(type, id);
            }
        }

        return result;
    }

    /**
     * @brief Indicate whether a dependency of the given type and id is registered.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @param id Dependency id.
     * @return True if dependency is registered, false otherwise.
     */
    template <typename T>
    bool has(const std::string &id) const noexcept {
        const auto it = dependencyRegisters_.find(Demangler::of<T>());
        if (dependencyRegisters_.cend() == it) {
            return false;
        }

        return (**it).has(id);
    }

    /**
     * @brief Return references to all registered dependencies of the given type.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @return Vector of references.
     */
    template <typename T /* TODO */>
    std::vector<std::reference_wrapper<T>> get() const noexcept {
        const auto it = dependencyRegisters_.find(Demangler::of<T>());
        if (dependencyRegisters_.cend() == it) {
            return {};
        }

        return static_cast<DependencyRegister<T> &>(**it).get();
    }

    /**
     * @brief Return reference to the dependency of a given type and id.
     * @exception DependencyNotFound If no dependency of the requested id is registered.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @param id Dependency id.
     * @return T& Dependency reference.
     */
    template <typename T, std::enable_if_t<!std::is_const<T>::value && !std::is_volatile<T>::value, bool> =
                              true>   // TODO chek everywhere else? check for pointer/array etxc?
    T &get(const std::string &id) const {
        const auto it = dependencyRegisters_.find(Demangler::of<T>());
        if (dependencyRegisters_.cend() == it) {
            throw DependencyRegisterNotFound(Demangler::of<T>(), id);
        }

        return static_cast<DependencyRegister<T> &>(**it).get(id);
    }

    /**
     * @brief Print registry content.
     *
     * @param os Output stream to be printed on.
     * @param dependencyRegistry Registry to be printed out.
     * @return Reference to os.
     */
    friend std::ostream &operator<<(std::ostream &os, DependencyRegistry &dependencyRegistry) {
        const auto &dependencyRegisters = dependencyRegistry.dependencyRegisters_;

        auto it0 = dependencyRegisters.cbegin();
        auto it1 = std::next(it0);

        while (dependencyRegisters.cend() != it0) {
            os << **it0;

            if (dependencyRegisters.cend() != it1) {
                os << '\n';
            }

            it0 = std::move(it1);
            it1 = std::next(it0);
        }

        return os;
    }

private:
    std::set<std::unique_ptr<DependencyRegister<>>, std::less<>> dependencyRegisters_;
};

}   // namespace diff
