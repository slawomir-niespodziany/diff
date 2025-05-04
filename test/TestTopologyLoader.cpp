#include <diff/TopologyLoader.h>
#include <gtest/gtest.h>

using namespace diff;

TEST(TestTopologyLoader, NonExistentFile) {
    EXPECT_THROW(
        try { TopologyLoader{"fake_path"s}; } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Topology file not accessible. Path: \"fake_path\".", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, TopologyNotAnArray) {
    TopologyLoader topologyLoader{R"( { "object": 123 } )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Topology json shall be an array.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentNotAnObject) {
    TopologyLoader topologyLoader{R"( [ 123 ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component shall be an object.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentTypeMissing) {
    TopologyLoader topologyLoader{R"( [ {} ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component type shall be specified.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentTypeInteger) {
    TopologyLoader topologyLoader{R"( [ { "type" : 123 } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component type shall be a string.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentTypeEmpty) {
    TopologyLoader topologyLoader{R"( [ { "type" : "" } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component type shall not be empty.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentIdMissing) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType" } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component id shall be specified.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentIdInteger) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : 123 } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component id shall be a string.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ComponentIdEmpty) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "" } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0} - Component id shall not be empty.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, DependenciesNotAnArray) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "dependencies" : "myDep" } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} - Dependencies shall be an array.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, DependencyEmptyString) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "dependencies" : [ "myDep", "" ] } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Dependency{#1} - Dependency id shall not be empty.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, DependencyNoString) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "dependencies" : [ "myDep", 123 ] } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Dependency{#1} - Dependency type shall be a string.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigNoObject) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : [ 123 ] } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} - Config shall be an object.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigKeyEmpty) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "" : "value" } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} - Config shall not consist of empty keys.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryTypeFloat) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : 1.1 } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ(
                "Component{#0, \"MyType\" : \"myId\"} : Config{\"key\"} - Config entry type shall be one of {bool, ungigned int, signed int, string, "
                "object}.",
                e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectSizeNot1) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "uint8_t" : 1, "uint32_t" : 2 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Config{\"key\"} - Config entry object shall be of size 1.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectTypeUnknown) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "uint10_t" : 1 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ(
                "Component{#0, \"MyType\" : \"myId\"} : Config{\"key\"} - Config entry object type shall be one of {uint8_t, int8_t, uint16_t, "
                "int16_t, uint32_t, int32_t, uint64_t, int64_t}.",
                e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectValueNotUnsigned) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "uint8_t" : -10 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Config{\"key\", uint8_t} - Config entry value type shall be unsigned integer.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectValueNotInteger) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "int16_t" : 1.1 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Config{\"key\", int16_t} - Config entry value type shall be integer.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectValueOutOfRange0) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "int8_t" : 511 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ("Component{#0, \"MyType\" : \"myId\"} : Config{\"key\", int8_t{511}} - Config entry value shall be in range of its declared type.", e.what());
            throw;
        },
        TopologyLoaderException);
}

TEST(TestTopologyLoader, ConfigEntryObjectValueOutOfRange1) {
    TopologyLoader topologyLoader{R"( [ { "type" : "MyType", "id" : "myId", "config" : { "key" : { "uint16_t" : 70000 } } } ] )"_json};
    Topology topology;

    EXPECT_THROW(
        try { topologyLoader.load(topology); } catch (const TopologyLoaderException &e) {
            EXPECT_STREQ(
                "Component{#0, \"MyType\" : \"myId\"} : Config{\"key\", uint16_t{70000}} - Config entry value shall be in range of its declared "
                "type.",
                e.what());
            throw;
        },
        TopologyLoaderException);
}

template <typename T>
static const T &configCheckTypeAndGetValue(const Config &config, const std::string &key) {
    const auto it = config.find(key);
    EXPECT_NE(config.cend(), it);
    EXPECT_EQ(Demangler::of<T>(), (*it)->type());
    return (*it)->value<T>();
}

TEST(TestTopologyLoader, NoException) {
    TopologyLoader topologyLoader{R"( 
    [
        {
            "type" : "type0",
            "id" : "id0"
        },
        {
            "type" : "type1",
            "id" : "id1"
        },
        {
            "type" : "type1",
            "id" : "id2",
            "dependencies" : [ "id0" ]
        },
        {
            "type" : "type2",
            "id" : "id3",
            "dependencies" : [ "id0", "id2" ],
            "config" : {
                "key0" : 1,
                "key1" : { "uint8_t" : 255 },
                "key2" : "stringValue",
                "key3" : -1
                }
        }
    ]
    )"_json};

    Topology topology;
    EXPECT_NO_THROW(topologyLoader.load(topology));

    EXPECT_EQ(topology.size(), 4u);

    EXPECT_EQ(topology[0].type, "type0"s);
    EXPECT_EQ(topology[0].id, "id0"s);
    EXPECT_EQ(topology[0].dependencyIds.size(), 0u);
    EXPECT_EQ(topology[0].config.size(), 0u);

    EXPECT_EQ(topology[1].type, "type1"s);
    EXPECT_EQ(topology[1].id, "id1"s);
    EXPECT_EQ(topology[1].dependencyIds.size(), 0u);
    EXPECT_EQ(topology[1].config.size(), 0u);

    EXPECT_EQ(topology[2].type, "type1"s);
    EXPECT_EQ(topology[2].id, "id2"s);
    EXPECT_EQ(topology[2].dependencyIds.size(), 1u);
    EXPECT_EQ(topology[2].dependencyIds[0], "id0"s);
    EXPECT_EQ(topology[2].config.size(), 0u);

    EXPECT_EQ(topology[3].type, "type2"s);
    EXPECT_EQ(topology[3].id, "id3"s);
    EXPECT_EQ(topology[3].dependencyIds.size(), 2u);
    EXPECT_EQ(topology[3].dependencyIds[0], "id0"s);
    EXPECT_EQ(topology[3].dependencyIds[1], "id2"s);
    EXPECT_EQ(topology[3].config.size(), 4u);
    EXPECT_EQ(configCheckTypeAndGetValue<uint64_t>(topology[3].config, "key0"s), 1u);
    EXPECT_EQ(configCheckTypeAndGetValue<uint8_t>(topology[3].config, "key1"s), 255u);
    EXPECT_EQ(configCheckTypeAndGetValue<std::string>(topology[3].config, "key2"s), "stringValue"s);
    EXPECT_EQ(configCheckTypeAndGetValue<int64_t>(topology[3].config, "key3"s), -1);
}