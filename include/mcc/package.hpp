#pragma once

#include <mcc/common.hpp>
#include <mcc/resource.hpp>

#include <json/json.hxx>

namespace mcc
{
    using FunctionInfo = std::vector<CommandT>;

    struct Tag
    {
        ResourceLocation Location;
        bool Required = true;
    };

    struct TagInfo
    {
        bool Replace = false;
        std::vector<Tag> Values;
    };

    struct PackageInfo
    {
        static PackageInfo Deserialize(const std::filesystem::path &path);

        void Serialize(const std::filesystem::path &path) const;

        std::string Name;
        json::Node Description;
        unsigned long Version;
    };

    struct Package
    {
        explicit Package(const PackageInfo &info);

        void Write(const std::filesystem::path &path) const;

        const PackageInfo &Info;
        std::map<std::string, std::map<std::vector<std::string>, FunctionInfo>> Functions;
        std::map<std::string, std::map<std::vector<std::string>, TagInfo>> Tags;
    };
}

template<>
struct data::serializer<mcc::ResourceLocation>
{
    static void to_data(
            json::Node &node,
            const mcc::ResourceLocation &value);
};

template<>
struct data::serializer<mcc::Tag>
{
    static void to_data(
            json::Node &node,
            const mcc::Tag &value);
};

template<>
struct data::serializer<mcc::TagInfo>
{
    static void to_data(
            json::Node &node,
            const mcc::TagInfo &value);
};

template<>
struct data::serializer<mcc::PackageInfo>
{
    static bool from_data(
            const json::Node &node,
            mcc::PackageInfo &value);
    static void to_data(
            json::Node &node,
            const mcc::PackageInfo &value);
};
