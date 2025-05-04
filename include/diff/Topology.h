#pragma once

/**
 * @file Topology.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines Topology used to define components to be instantiated, their dependencies and configuration.
 * @version 0.1
 * @date 2025-02-25
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Config.h>
#include <diff/DependencyId.h>
#include <ostream>
#include <string>
#include <vector>

namespace diff {

/**
 * @brief Defines component instance to be constructed.
 */
struct TopologyEntry {
    /**
     * @brief Component type (demangled type name).
     */
    std::string type;

    /**
     * @brief Component instance identifier (shall be unique within a Topology). Interfaces exposed by the component are exposed under the same
     * identifier.
     */
    std::string id;

    /**
     * @brief Ordered collection of identifiers of dependencies to be injected upon component instantiation.
     */
    DependencyIds dependencyIds;

    /**
     * @brief Component instance configuration.
     */
    Config config;
};

/**
 * @brief Ordered collection of TopologyEntry objects. Defines component instances to be constructed.
 */
using Topology = std::vector<TopologyEntry>;

/**
 * @brief Print topology content.
 *
 * @param os Output stream to be printed on.
 * @param topology Topology to be printed out.
 * @return Reference to os.
 */
std::ostream &operator<<(std::ostream &os, const Topology &topology) {
    for (const TopologyEntry &topologyEntry : topology) {
        os << "topologyBuilder //\n    .component(\""s << topologyEntry.type << "\"s, \""s << topologyEntry.id << "\"s)"s;

        for (const DependencyId &dependencyId : topologyEntry.dependencyIds) {
            os << "\n    .dependency(\""s << dependencyId << "\"s)"s;
        }

        for (const auto &pConfigEntry : topologyEntry.config) {
            const std::string &key = pConfigEntry->key();
            const std::string &type = pConfigEntry->type();
            const std::string value = pConfigEntry->toString();

            os << "\n    .config<"s;
            if (Demangler::of<std::string>() == type) {
                os << "std::string>(\""s << key << "\"s, \""s << value << "\"s)"s;
            } else {
                os << type << ">(\""s << key << "\"s, "s << value << ")"s;
            }
        }

        os << ";\n";
    }

    return os;
}

}   // namespace diff
