#pragma once

/**
 * @file Factory.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines Factory class used to instantiate components.
 * @version 0.1
 * @date 2025-02-25
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Component.h>
#include <diff/Demangler.h>
#include <diff/DependencyId.h>
#include <diff/DependencyRegistry.h>
#include <iostream>
#include <memory>

namespace diff {

template <typename T = void>
class Factory;

/**
 * @brief Abstract base class, a common interface for instantiations of Factory<T != void>.
 */
template <>
class Factory<void> {
public:
    virtual ~Factory() = default;

    /**
     * @brief Return constructed component type name.
     *
     * @return Type name.
     */
    const std::string &type() const noexcept { return type_; }

    /**
     * @brief Construct component of the underlying type.
     *
     * @param id Id to be assigned to the constructed component instance.
     * @param dependencyIds Instance dependency ids.
     * @param config Instance config.
     * @param dependencyRegistry Registry of dependencies
     * @return std::unique_ptr<Component<>>
     */
    virtual std::unique_ptr<Component<>> build(std::string &id, const DependencyIds &dependencyIds, Config &config,
                                               DependencyRegistry &dependencyRegistry) = 0;

protected:
    Factory(const std::string &type) : type_{type} {}

    const std::string &type_;
};

/**
 * @brief Specialization allowing component instantiation.
 *
 * @tparam T Type of component to be instantiated.
 */
template <typename T>
class Factory : public Factory<> {
public:
    static_assert(std::is_class<T>::value, "Component type shall be a class.");
    static_assert(!std::is_const<T>::value, "Component type shall not be const.");
    static_assert(!std::is_volatile<T>::value, "Component type shall not be volatile.");
    static_assert(!std::is_abstract<T>::value, "Component type shall not be abstract.");
    static_assert(std::is_base_of<Component<T>, T>::value, "Component type shall be a descendant of Component<T> type.");

    Factory() : Factory<>(Demangler::of<T>()) {}
    virtual ~Factory() = default;

    /**
     * @brief @see Factory<void>
     */
    virtual std::unique_ptr<Component<>> build(std::string &id, const DependencyIds &dependencyIds, Config &config,
                                               DependencyRegistry &dependencyRegistry) {
        Component<T>::initializer_.first = std::move(id);
        Component<T>::initializer_.second = std::move(config);

        std::unique_ptr<T> p_ = Constructor{dependencyRegistry, dependencyIds};

        p_->registerAs(dependencyRegistry);

        return std::move(p_);
    }

private:
    class Injector final {
    public:
        Injector(const DependencyRegistry &dependencyRegistry, const DependencyId &dependencyId)
            : dependencyRegistry_{dependencyRegistry}, dependencyId_{dependencyId} {}
        ~Injector() = default;

        template <typename U, typename V = std::remove_cv_t<U>,
                  std::enable_if_t<!std::is_same<T, V>::value, bool> = true /* Don`t match for implicit copy/move constructor. */>
        operator U &() const {
            static_assert(std::is_abstract<V>::value, "Dependency type shall be abstract.");

            return dependencyRegistry_.get<V>(dependencyId_);
        }

    private:
        const DependencyRegistry &dependencyRegistry_;
        const DependencyId &dependencyId_;
    };

    class Constructor {
    public:
        Constructor(const DependencyRegistry &dependencyRegistry, const DependencyIds &dependencyIds)
            : dependencyRegistry_{dependencyRegistry}, dependencyIds_{dependencyIds} {}

        operator std::unique_ptr<T>() const noexcept { return construct(); }

    private:
        template <int... Ints>
        constexpr std::enable_if_t<std::is_constructible<T, decltype((Ints, Injector{DependencyRegistry{}, DependencyId{}}))...>::value,
                                   std::unique_ptr<T>>
        construct() const noexcept {
            return std::make_unique<T>((Injector{dependencyRegistry_, dependencyIds_[Ints]})...);
        }

        template <int... Ints>
        constexpr std::enable_if_t<!std::is_constructible<T, decltype((Ints, Injector{DependencyRegistry{}, DependencyId{}}))...>::value,
                                   std::unique_ptr<T>>
        construct() const noexcept {
            return construct<Ints..., sizeof...(Ints)>();
        }

        const DependencyRegistry &dependencyRegistry_;
        const DependencyIds &dependencyIds_;
    };
};

}   // namespace diff
