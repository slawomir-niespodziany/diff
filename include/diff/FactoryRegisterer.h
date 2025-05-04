#pragma once

/**
 * @file FactoryRegisterer.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines FactoryRegisterer class used to instantiate component factory and register it in FactoryRegistry.
 * @version 0.1
 * @date 2024-02-17
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <diff/Factory.h>
#include <diff/FactoryRegistry.h>

namespace diff {

/**
 * @brief FactoryRegisterer instantiates a factory for the given component type and registers this factory in FactoryRegistry.
 *
 * @tparam T Type of the component to be constructed by the instantiated factory.
 */
template <typename T>
class FactoryRegisterer final {
public:
    /**
     * Instantiate and register factory for the given type.
     */
    FactoryRegisterer() { registered_ = FactoryRegistry::getInstance().add(factory_); }

    /**
     * Unregister and destruct the factory.
     */
    ~FactoryRegisterer() {
        if (registered_) {
            FactoryRegistry::getInstance().remove(factory_);
        }
    }

private:
    Factory<T> factory_;
    bool registered_;
};

}   // namespace diff
