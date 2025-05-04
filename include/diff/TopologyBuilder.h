#pragma once

/**
 * @file TopologyBuilder.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines TopologyBuilder class used to initialize Topology objects.
 * @version 0.1
 * @date 2025-02-25
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Exception.h>
#include <diff/Topology.h>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace diff {

/**
 * @brief Provides an API for configuring Topology object.
 */
class TopologyBuilder final {
public:
    /**
     * @brief Provides fluent interface for single TopologyEntry configuration.
     */
    class TopologyEntryBuilder final {
    public:
        TopologyEntryBuilder() = delete;
        TopologyEntryBuilder(const TopologyEntryBuilder &) = delete;
        TopologyEntryBuilder &operator=(const TopologyEntryBuilder &) = delete;
        TopologyEntryBuilder &operator=(TopologyEntryBuilder &&) = delete;

        /**
         * @brief Move constructor required for the fluent interface.
         *
         * @param other Instance to be moved from.
         */
        TopologyEntryBuilder(TopologyEntryBuilder &&other) : topologyEntry_{other.topologyEntry_} {}
        ~TopologyEntryBuilder() = default;

        /**
         * @brief Add dependency with the given id.
         *
         * @param id Dependency id.
         * @return Reference to *this.
         */
        TopologyEntryBuilder &dependency(const std::string &id) {
            topologyEntry_.dependencyIds.emplace_back(id);
            return *this;
        }

        /**
         * @brief Add ConfigEntry with the given key, type and value.
         * @exception ConfigEntryKeyDuplicated If ConfigEntry with the given key is already defined.
         *
         * @tparam T Value type.
         * @param key ConfigEntry key.
         * @param value ConfigEntry value.
         * @return Reference to *this.
         */
        template <typename T>
        TopologyEntryBuilder &config(const std::string &key, const T &value) {
            if (0 != topologyEntry_.config.count(key)) {
                throw ConfigEntryKeyDuplicated(key);
            }

            std::unique_ptr<ConfigEntry<>> pEntry = std::make_unique<ConfigEntry<T>>(key, value);
            topologyEntry_.config.emplace(std::move(pEntry));

            return *this;
        }

    private:
        friend class TopologyBuilder;

        TopologyEntryBuilder(TopologyEntry &topologyEntry) : topologyEntry_{topologyEntry} {}

        TopologyEntry &topologyEntry_;
    };

    /**
     * @brief Construct TopologyBuilder object. Topology object is cleared if not empty.
     *
     * @param topology Topology object to be configured.
     */
    TopologyBuilder(Topology &topology) : topology_{topology} { topology_.clear(); }
    ~TopologyBuilder() = default;

    /**
     * @brief Create new TopologyEntry and initialize it with the given component type name and instance id.
     * @exception ComponentIdDuplicated If component with the given instance id is already defined.
     *
     * @param type Component type name.
     * @param id Component instance id.
     * @return TopologyEntryBuilder object.
     */
    TopologyEntryBuilder component(const std::string &type, const std::string &id) {
        if (std::any_of(topology_.cbegin(), topology_.cend(), [&id](const TopologyEntry &topologyEntry) { return topologyEntry.id == id; })) {
            throw ComponentIdDuplicated(type, id);
        }

        topology_.emplace_back(TopologyEntry{type, id, {}, {}});

        return TopologyEntryBuilder{topology_.back()};
    }

private:
    Topology &topology_;
};

}   // namespace diff
