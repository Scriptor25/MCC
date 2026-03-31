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

    void to_json(json::Node &node, const ResourceLocation &value);

    void to_json(json::Node &node, const Tag &value);

    void to_json(json::Node &node, const TagInfo &value);

    void to_json(json::Node &node, const PackageInfo &value);

    template<json::node N>
    bool from_json(N &&node, PackageInfo &value)
    {
        if (!node.template Is<json::Object>())
            return false;

        auto ok = true;

        ok &= node["name"] >> value.Name;
        ok &= node["description"] >> value.Description;
        ok &= node["version"] >> value.Version;

        return ok;
    }
}
