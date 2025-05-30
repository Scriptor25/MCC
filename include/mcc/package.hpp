#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <nlohmann/json.hpp>

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
        nlohmann::json Description;
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

    void to_json(nlohmann::json &json, const ResourceLocation &location);
    void to_json(nlohmann::json &json, const Tag &tag);
    void to_json(nlohmann::json &json, const TagInfo &info);

    void to_json(nlohmann::json &json, const PackageInfo &info);
    void from_json(const nlohmann::json &json, PackageInfo &info);
}
