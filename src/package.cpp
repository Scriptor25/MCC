#include <mcc/error.hpp>
#include <mcc/package.hpp>

#include <fstream>

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

            stream << std::setw(2) << json::Node(tag_);

            stream.close();
        }

    const auto package = path / "pack.mcmeta";
    std::ofstream stream(package);
    Assert(stream.is_open(), "failed to open file {}", package.string());

    json::Node node
    {
        json::Object
        {
            std::pair<json::Key, json::Node>
            {
                "pack",
                json::Object
                {
                    std::pair<json::Key, json::Node>
                    {
                        "description",
                        Info.Description,
                    },
                    std::pair<json::Key, json::Node>
                    {
                        "pack_format",
                        Info.Version,
                    },
                },
            },
        }
    };
    stream << std::setw(2) << node;

    stream.close();
}

mcc::PackageInfo mcc::PackageInfo::Deserialize(const std::filesystem::path &path)
{
    std::ifstream stream(path);
    Assert(stream.is_open(), "failed to open file {}", path.string());

    json::Node node;
    stream >> node;

    if (PackageInfo value; node >> value)
        return value;

    Error("failed to deserialize package info json");
}

void mcc::PackageInfo::Serialize(const std::filesystem::path &path) const
{
    std::ofstream stream(path);
    Assert(stream.is_open(), "failed to open file {}", path.string());

    stream << std::setw(2) << json::Node(*this);
}

void json::serializer<mcc::ResourceLocation>::to_json(Node &node, const mcc::ResourceLocation &value)
{
    node = value.String();
}

void json::serializer<mcc::Tag>::to_json(Node &node, const mcc::Tag &value)
{
    node = Object
    {
        { "id", value.Location },
        { "required", value.Required },
    };
}

void json::serializer<mcc::TagInfo>::to_json(Node &node, const mcc::TagInfo &value)
{
    node = Object
    {
        { "replace", value.Replace },
        { "values", value.Values },
    };
}

bool json::serializer<mcc::PackageInfo>::from_json(const Node &node, mcc::PackageInfo &value)
{
    if (!node.Is<Object>())
        return false;

    auto ok = true;

    ok &= node["name"] >> value.Name;
    ok &= node["description"] >> value.Description;
    ok &= node["version"] >> value.Version;

    return ok;
}

void json::serializer<mcc::PackageInfo>::to_json(Node &node, const mcc::PackageInfo &value)
{
    node = Object
    {
        { "name", value.Name },
        { "description", value.Description },
        { "version", value.Version },
    };
}
