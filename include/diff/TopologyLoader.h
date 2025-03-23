#pragma once

/**
 * @file TopologyLoader.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines TopologyLoader class used to load Topology from Json.
 * @version 0.1
 * @date 2025-03-03
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Exception.h>
#include <diff/TopologyBuilder.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace diff {

using namespace std::string_literals;

/**
 * @brief Allows to initialize Topology object from Json.
 */
class TopologyLoader final {
public:
    TopologyLoader() = delete;
    TopologyLoader(const TopologyLoader&) = delete;
    TopologyLoader(TopologyLoader&&) = delete;

    /**
     * @brief Construct TopologyLoader object. Load Topology metadata from a Json file.
     * @exception TopologyLoaderException If loading fails.
     *
     * @param path Json file path.
     */
    TopologyLoader(const std::string& path) : json_(loadFile(path)) {}

    /**
     * @brief Construct TopologyLoader object. Load Topology metadata from Json object.
     *
     * @param jsonTopology Json object.
     */
    TopologyLoader(const nlohmann::json& jsonTopology) : json_(jsonTopology) {}

    ~TopologyLoader() = default;

    TopologyLoader& operator=(const TopologyLoader&) = delete;
    TopologyLoader& operator=(TopologyLoader&&) = delete;

    /**
     * @brief Load Json data into Topology object.
     * @exception TopologyLoaderException If loading fails.
     *
     * @param topology Topology object to be initialized.
     */
    void load(Topology& topology) {
        TopologyBuilder topologyBuilder{topology};

        if (!json_.is_array()) {
            throw TopologyLoaderException{"Topology json shall be an array."s};
        }

        for (const auto& kv : json_.items()) {
            const std::string componentIndex = kv.key();
            const nlohmann::json componentJson = kv.value();

            if (!componentJson.is_object()) {
                throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component shall be an object."s};
            }

            const std::string componentType = loadType(componentIndex, componentJson);
            const std::string componentId = loadId(componentIndex, componentJson);

            TopologyBuilder::TopologyEntryBuilder topologyEntryBuilder = topologyBuilder.component(componentType, componentId);

            loadDependencies(componentIndex, componentType, componentId, componentJson, topologyEntryBuilder);
            loadConfig(componentIndex, componentType, componentId, componentJson, topologyEntryBuilder);
        }
    }

private:
    static const std::string KEY_TYPE;
    static const std::string KEY_ID;
    static const std::string KEY_DEPENDENCIES;
    static const std::string KEY_CONFIG;

    static const std::string TYPE_UINT8;
    static const std::string TYPE_UINT16;
    static const std::string TYPE_UINT32;
    static const std::string TYPE_UINT64;
    static const std::string TYPE_INT8;
    static const std::string TYPE_INT16;
    static const std::string TYPE_INT32;
    static const std::string TYPE_INT64;

    nlohmann::json loadFile(const std::string& path) const {
        std::ifstream file(path);
        if (!file) {
            throw TopologyLoaderException{"Topology file not accessible. Path: \""s + path + "\"."s};
        }

        try {
            return nlohmann::json::parse(file, nullptr, true, true);
        } catch (const nlohmann::json::parse_error& e) {
            throw TopologyLoaderException{"Topology json syntax error. Details: \n"s + e.what()};
        }
    }

    std::string loadType(const std::string& componentIndex, const nlohmann::json& componentJson) {
        const nlohmann::json::const_iterator it = componentJson.find(KEY_TYPE);
        if (it == componentJson.cend()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component type shall be specified."s};
        }

        const nlohmann::json& json = *it;
        if (!json.is_string()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component type shall be a string."s};
        }

        const std::string result = json.get<std::string>();
        if (result.empty()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component type shall not be empty."s};
        }

        return result;
    }

    std::string loadId(const std::string& componentIndex, const nlohmann::json& componentJson) {
        const nlohmann::json::const_iterator it = componentJson.find(KEY_ID);
        if (it == componentJson.cend()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component id shall be specified."s};
        }

        const nlohmann::json& json = *it;
        if (!json.is_string()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component id shall be a string."s};
        }

        const std::string result = json.get<std::string>();
        if (result.empty()) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + "} - Component id shall not be empty."s};
        }

        return result;
    }

    void loadDependencies(const std::string& componentIndex, const std::string& componentType, const std::string& componentId,
                          const nlohmann::json& componentJson, TopologyBuilder::TopologyEntryBuilder& topologyEntryBuilder) {
        const nlohmann::json::const_iterator it = componentJson.find(KEY_DEPENDENCIES);
        if (it != componentJson.cend()) {
            const nlohmann::json& json = *it;
            if (!json.is_array()) {
                throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                              "\"} - Dependencies shall be an array."s};
            }

            for (const auto& kv : json.items()) {
                const std::string& dependencyIndex = kv.key();
                const nlohmann::json& dependencyJson = kv.value();

                loadDependency(componentIndex, componentType, componentId, dependencyIndex, dependencyJson, topologyEntryBuilder);
            }
        }
    }

    void loadDependency(const std::string& componentIndex, const std::string& componentType, const std::string& componentId,
                        const std::string& dependencyIndex, const nlohmann::json& dependencyJson,
                        TopologyBuilder::TopologyEntryBuilder& topologyEntryBuilder) {
        if (dependencyJson.is_string()) {
            const std::string id = dependencyJson.get<std::string>();
            if (id.empty()) {
                throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                              "\"} : Dependency{#"s + dependencyIndex + "} - Dependency id shall not be empty."s};
            }
            topologyEntryBuilder.dependency(id);

        } else {
            throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                          "\"} : Dependency{#"s + dependencyIndex + "} - Dependency type shall be a string."s};
        }
    }

    void loadConfig(const std::string& componentIndex, const std::string& componentType, const std::string& componentId,
                    const nlohmann::json& componentJson, TopologyBuilder::TopologyEntryBuilder& topologyEntryBuilder) {
        const nlohmann::json::const_iterator it = componentJson.find(KEY_CONFIG);
        if (it != componentJson.cend()) {
            const nlohmann::json& json = *it;
            if (!json.is_object()) {
                throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                              "\"} - Config shall be an object."s};
            }

            for (const auto& kv : json.items()) {
                const std::string& entryKey = kv.key();
                const nlohmann::json& entryJson = kv.value();

                if (entryKey.empty()) {
                    throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                                  "\"} - Config shall not consist of empty keys."s};
                }

                loadConfigEntry(componentIndex, componentType, componentId, entryKey, entryJson, topologyEntryBuilder);
            }
        }
    }

    void loadConfigEntry(const std::string& componentIndex, const std::string& componentType, const std::string& componentId,
                         const std::string& entryKey, const nlohmann::json& entryJson, TopologyBuilder::TopologyEntryBuilder& topologyEntryBuilder) {
        if (entryJson.is_boolean()) {
            topologyEntryBuilder.config<bool>(entryKey, entryJson.get<bool>());

        } else if (entryJson.is_number_unsigned()) {
            topologyEntryBuilder.config<uint64_t>(entryKey, entryJson.get<uint64_t>());

        } else if (entryJson.is_number_integer()) {
            topologyEntryBuilder.config<int64_t>(entryKey, entryJson.get<int64_t>());

        } else if (entryJson.is_string()) {
            topologyEntryBuilder.config<std::string>(entryKey, entryJson.get<std::string>());

        } else if (entryJson.is_object()) {
            if (1u != entryJson.size()) {
                throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                              "\"} : Config{\""s + entryKey + "\"} - Config entry object shall be of size 1."s};
            }

            const nlohmann::json::const_iterator it = entryJson.cbegin();
            const std::string type = it.key();
            const nlohmann::json& json = it.value();

            if ((TYPE_UINT8 == type) || (TYPE_UINT16 == type) || (TYPE_UINT32 == type) || (TYPE_UINT64 == type)) {
                if (!json.is_number_unsigned()) {
                    throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                                  "\"} : Config{\""s + entryKey + "\", "s + type +
                                                  "} - Config entry value type shall be unsigned integer."s};
                }

                const uint64_t value = json.get<uint64_t>();
                if (TYPE_UINT8 == type) {
                    loadConfigEntry<uint8_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else if (TYPE_UINT16 == type) {
                    loadConfigEntry<uint16_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else if (TYPE_UINT32 == type) {
                    loadConfigEntry<uint32_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else {   // (TYPE_UINT64 == type)
                    loadConfigEntry<uint64_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);
                }
            } else if ((TYPE_INT8 == type) || (TYPE_INT16 == type) || (TYPE_INT32 == type) || (TYPE_INT64 == type)) {
                if (!json.is_number_integer()) {
                    throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId +
                                                  "\"} : Config{\""s + entryKey + "\", "s + type + "} - Config entry value type shall be integer."s};
                }

                const int64_t value = json.get<uint64_t>();
                if (TYPE_INT8 == type) {
                    loadConfigEntry<int8_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else if (TYPE_INT16 == type) {
                    loadConfigEntry<int16_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else if (TYPE_INT32 == type) {
                    loadConfigEntry<int32_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);

                } else {   // (TYPE_INT64 == type)
                    loadConfigEntry<int64_t>(componentIndex, componentType, componentId, entryKey, type, value, topologyEntryBuilder);
                }
            } else {
                throw TopologyLoaderException{
                    "Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId + "\"} : Config{\""s + entryKey +
                    "\"} - Config entry object type shall be one of {uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t}."s};
            }
        } else {
            throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId + "\"} : Config{\""s +
                                          entryKey + "\"} - Config entry type shall be one of {bool, ungigned int, signed int, string, object}."s};
        }
    }

    template <typename T, typename U>
    void loadConfigEntry(const std::string& componentIndex, const std::string& componentType, const std::string& componentId,
                         const std::string& entryKey, const std::string& entryType, const U& entryValue,
                         TopologyBuilder::TopologyEntryBuilder& topologyEntryBuilder) {
        static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||     //
                      std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value ||   //
                      std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||   //
                      std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value);
        static_assert(std::is_same<U, uint64_t>::value || std::is_same<U, int64_t>::value);

        if ((entryValue < static_cast<U>(std::numeric_limits<T>::min())) || (static_cast<U>(std::numeric_limits<T>::max()) < entryValue)) {
            throw TopologyLoaderException{"Component{#"s + componentIndex + ", \""s + componentType + "\" : \""s + componentId + "\"} : Config{\""s +
                                          entryKey + "\", "s + entryType + "{"s + std::to_string(entryValue) +
                                          "}} - Config entry value shall be in range of its declared type."s};
        }

        topologyEntryBuilder.config<T>(entryKey, entryValue);
    }

    const nlohmann::json json_;
};

const std::string TopologyLoader::KEY_TYPE{"type"s};
const std::string TopologyLoader::KEY_ID{"id"s};
const std::string TopologyLoader::KEY_DEPENDENCIES{"dependencies"s};
const std::string TopologyLoader::KEY_CONFIG{"config"s};

const std::string TopologyLoader::TYPE_UINT8{"uint8_t"s};
const std::string TopologyLoader::TYPE_UINT16{"uint16_t"s};
const std::string TopologyLoader::TYPE_UINT32{"uint32_t"s};
const std::string TopologyLoader::TYPE_UINT64{"uint64_t"s};

const std::string TopologyLoader::TYPE_INT8{"int8_t"s};
const std::string TopologyLoader::TYPE_INT16{"int16_t"s};
const std::string TopologyLoader::TYPE_INT32{"int32_t"s};
const std::string TopologyLoader::TYPE_INT64{"int64_t"s};

}   // namespace diff