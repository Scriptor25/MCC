#include <fstream>
#include <mcc/package.hpp>

mcc::Package::Package(const PackageInfo &info)
    : Info(info)
{
}

void mcc::Package::Write(const std::filesystem::path &root) const
{
    if (std::filesystem::exists(root))
        std::filesystem::remove_all(root);

    const auto data = root / "data";
    create_directories(data);

    Functions.ForEach(
        [&](const ResourceLocation &key, const FunctionInfo &value)
        {
            const auto directory = data / key.Namespace / "function";
            create_directories(directory);

            const auto file = directory / (key.Path + ".mcfunction");
            std::ofstream stream(file);
            if (!stream)
                return;

            for (auto &command: value.Commands)
                stream << command << std::endl;

            stream.close();
        });

    Tags.ForEach(
        [&](const ResourceLocation &key, const TagInfo &value)
        {
            const auto directory = data / key.Namespace / "tags" / "function";
            create_directories(directory);

            const auto file = directory / (key.Path + ".json");
            std::ofstream stream(file);
            if (!stream)
                return;

            stream << std::setw(2) << nlohmann::json(value);

            stream.close();
        });

    std::ofstream stream(root / "pack.mcmeta");
    if (!stream)
        return;
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
    if (!stream)
        return {};

    nlohmann::json json;
    stream >> json;
    stream.close();

    return json;
}

void mcc::PackageInfo::Serialize(const std::filesystem::path &path) const
{
    std::ofstream stream(path);
    if (!stream)
        return;

    const nlohmann::json json = *this;
    stream << std::setw(2) << json;
    stream.close();
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
