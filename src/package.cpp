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

    Functions.ForEach(
        [&](const ResourceLocation &key, const FunctionInfo &value)
        {
            const auto file = data / key.Namespace / "function" / (key.Path + ".mcfunction");
            const auto directory = file.parent_path();
            create_directories(directory);

            std::ofstream stream(file);
            Assert(stream.is_open(), "failed to open function file {}", file.string());

            for (auto &command: value.Commands)
                stream << command << std::endl;

            stream.close();
        });

    Tags.ForEach(
        [&](const ResourceLocation &key, const TagInfo &value)
        {
            const auto file = data / key.Namespace / "tags" / "function" / (key.Path + ".json");
            const auto directory = file.parent_path();
            create_directories(directory);

            std::ofstream stream(file);
            Assert(stream.is_open(), "failed to open tag file {}", file.string());

            stream << std::setw(2) << nlohmann::json(value);

            stream.close();
        });

    const auto package = path / "pack.mcmeta";
    std::ofstream stream(package);
    Assert(stream.is_open(), "failed to open package file {}", package.string());

    stream << std::setw(2) << nlohmann::json(
        {
            {
                "pack",
                {
                    {"description", Info.Description},
                    {"pack_format", Info.Version}
                }
            }
        });
    stream.close();
}

mcc::PackageInfo mcc::PackageInfo::Deserialize(const std::filesystem::path &path)
{
    std::ifstream stream(path);
    Assert(stream.is_open(), "failed to open info file {}", path.string());

    nlohmann::json json;
    stream >> json;
    return json;
}

void mcc::PackageInfo::Serialize(const std::filesystem::path &path) const
{
    std::ofstream stream(path);
    Assert(stream.is_open(), "failed to open info file {}", path.string());

    const nlohmann::json json = *this;
    stream << std::setw(2) << json;
}

void mcc::to_json(nlohmann::json &json, const ResourceLocation &location)
{
    json = location.String();
}

void mcc::from_json(const nlohmann::json &json, ResourceLocation &location)
{
    const auto value = std::string(json);
    const auto split = value.find(':');
    location.Namespace = value.substr(0, split);
    location.Path = value.substr(split + 1);
}

void mcc::to_json(nlohmann::json &json, const Tag &tag)
{
    json = {{"id", tag.Location}, {"required", tag.Required}};
}

void mcc::from_json(const nlohmann::json &json, Tag &tag)
{
    json.at("id").get_to(tag.Location);
    json.at("required").get_to(tag.Required);
}

void mcc::to_json(nlohmann::json &json, const TagInfo &info)
{
    json = {{"replace", info.Replace}, {"values", info.Values}};
}

void mcc::from_json(const nlohmann::json &json, TagInfo &info)
{
    json.at("replace").get_to(info.Replace);
    json.at("values").get_to(info.Values);
}

void mcc::to_json(nlohmann::json &json, const PackageInfo &info)
{
    json = {{"name", info.Name}, {"description", info.Description}, {"version", info.Version}};
}

void mcc::from_json(const nlohmann::json &json, PackageInfo &info)
{
    json.at("name").get_to(info.Name);
    json.at("description").get_to(info.Description);
    json.at("version").get_to(info.Version);
}
