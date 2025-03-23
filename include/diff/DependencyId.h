#pragma once

/**
 * @file DependencyId.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines identifiers of dependencies to be injected.
 * @version 0.1
 * @date 2025-12-06
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <string>
#include <vector>

namespace diff {

/**
 * @brief Defines the identifier of a dependency to be injected. The type of injected dependency is determined by the constructor argument type of the
 * constructed component.
 */
using DependencyId = std::string;

/**
 * @brief Ordered collection of dependency identifiers. Defines the set of dependencies to be injected when constructing an instance of a component.
 */
using DependencyIds = std::vector<DependencyId>;

}   // namespace diff
