#include <fstream>
#include <iostream>
#include <mcc/error.hpp>
#include <mcc/package.hpp>

mcc::Package::Package(const PackageInfo &info)
    : Info(info)
{
}

void mcc::Package::Write(const std::filesystem::path &path) const
{
    if (std::filesystem::exists(path))
        std::filesystem::remove_all(path);

    const auto data = path / "data";
    create_directories(data);

    for (auto &[namespace_, functions_] : Functions)
        for (auto &[path_, function_] : functions_)
        {
            const auto file = data
                              / namespace_
                              / "function"
                              / std::vector(path_.begin(), path_.end() - 1)
                              / (path_.back() + ".mcfunction");
            const auto directory = file.parent_path();
            create_directories(directory);

            std::ofstream stream(file);
            Assert(stream.is_open(), "failed to open file {}", file.string());

            for (auto &command : function_)
                stream << command << std::endl;

            stream.close();
        }

    for (auto &[namespace_, tags_] : Tags)
        for (auto &[path_, tag_] : tags_)
        {
            const auto file = data
                              / namespace_
                              / "tags"
                              / "function"
                              / std::vector(path_.begin(), path_.end() - 1)
                              / (path_.back() + ".json");
            const auto directory = file.parent_path();
            create_directories(directory);

            std::ofstream stream(file);
            Assert(stream.is_open(), "failed to open file {}", file.string());

            stream << std::setw(2) << nlohmann::json(tag_);

            stream.close();
        }

    const auto package = path / "pack.mcmeta";
    std::ofstream stream(package);
    Assert(stream.is_open(), "failed to open file {}", package.string());

    nlohmann::json json{
        { "pack", { { "description", Info.Description }, { "pack_format", Info.Version } } }
    };
    stream << std::setw(2) << json;
    stream.close();
}

mcc::PackageInfo mcc::PackageInfo::Deserialize(const std::filesystem::path &path)
{
    std::ifstream stream(path);
    Assert(stream.is_open(), "failed to open file {}", path.string());

    nlohmann::json json;
    stream >> json;
    return json;
}

void mcc::PackageInfo::Serialize(const std::filesystem::path &path) const
{
    std::ofstream stream(path);
    Assert(stream.is_open(), "failed to open file {}", path.string());

    const nlohmann::json json = *this;
    stream << std::setw(2) << json;
}

void mcc::to_json(nlohmann::json &json, const ResourceLocation &location)
{
    json = location.String();
}

void mcc::to_json(nlohmann::json &json, const Tag &tag)
{
    json = {
        { "id", tag.Location },
        { "required", tag.Required },
    };
}

void mcc::to_json(nlohmann::json &json, const TagInfo &info)
{
    json = {
        { "replace", info.Replace },
        { "values", info.Values },
    };
}

void mcc::to_json(nlohmann::json &json, const PackageInfo &info)
{
    json = {
        { "name", info.Name },
        { "description", info.Description },
        { "version", info.Version },
    };
}

void mcc::from_json(const nlohmann::json &json, PackageInfo &info)
{
    json.at("name").get_to(info.Name);
    json.at("description").get_to(info.Description);
    json.at("version").get_to(info.Version);
}
