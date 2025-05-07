#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <nlohmann/json.hpp>

namespace mcc
{
    template<typename T>
    class ResourceMap
    {
    public:
        T &operator[](ResourceLocation location)
        {
            return m_Data[location.Namespace][location.Path];
        }

        const T &operator[](ResourceLocation location) const
        {
            return m_Data.at(location.Namespace).at(location.Path);
        }

        void ForEach(std::function<void(const ResourceLocation &, const T &)> consumer) const
        {
            for (auto &[namespace_, namespace_value_]: m_Data)
                for (auto &[id_, id_value_]: namespace_value_)
                    consumer({namespace_, id_}, id_value_);
        }

    private:
        std::map<std::string, std::map<std::string, T>> m_Data;
    };

    struct FunctionInfo
    {
        ResourceLocation Location;
        std::vector<CommandT> Commands;
    };

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
        /**
         * namespace:function -> function
         */
        ResourceMap<FunctionInfo> Functions;
        /**
         * namespace:tag -> tag...
         */
        ResourceMap<TagInfo> Tags;
    };

    void to_json(nlohmann::json &json, const ResourceLocation &location);
    void from_json(const nlohmann::json &json, ResourceLocation &location);
    void to_json(nlohmann::json &json, const Tag &tag);
    void from_json(const nlohmann::json &json, Tag &tag);
    void to_json(nlohmann::json &json, const TagInfo &info);
    void from_json(const nlohmann::json &json, TagInfo &info);
    void to_json(nlohmann::json &json, const PackageInfo &info);
    void from_json(const nlohmann::json &json, PackageInfo &info);
}
