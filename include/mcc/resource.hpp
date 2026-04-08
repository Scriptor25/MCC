#pragma once

#include <mcc/common.hpp>

#include <filesystem>
#include <sstream>

namespace mcc
{
    template<bool TAG>
    struct Resource
    {
        Resource() = default;

        Resource(const std::vector<std::string> &path)
            : Path(path)
        {
        }

        Resource(
                std::string namespace_,
                const std::vector<std::string> &path)
            : Namespace(std::move(namespace_)),
              Path(path)
        {
        }

        Resource(const Resource &other)            = default;
        Resource &operator=(const Resource &other) = default;

        Resource(Resource &&other) noexcept
            : Namespace(std::move(other.Namespace)),
              Path(std::move(other.Path))
        {
        }

        Resource &operator=(Resource &&other) noexcept
        {
            std::swap(Namespace, other.Namespace);
            std::swap(Path, other.Path);
            return *this;
        }

        Resource(const Resource<!TAG> &other)
            : Namespace(other.Namespace),
              Path(other.Path)
        {
        }

        Resource(Resource<!TAG> &&other) noexcept
            : Namespace(std::move(other.Namespace)),
              Path(std::move(other.Path))
        {
        }

        Resource &operator=(Resource<!TAG> &&other) noexcept
        {
            std::swap(Namespace, other.Namespace);
            std::swap(Path, other.Path);
            return *this;
        }

        std::ostream &Print(std::ostream &stream) const
        {
            stream << (TAG ? "#" : "") << Namespace << ':';
            for (auto it = Path.begin(); it != Path.end(); ++it)
            {
                if (it != Path.begin())
                    stream << '/';
                stream << *it;
            }
            return stream;
        }

        [[nodiscard]] std::string String() const
        {
            std::ostringstream stream;
            Print(stream);
            return stream.str();
        }

        bool operator==(const Resource &other) const
        {
            return Namespace == other.Namespace && Path == other.Path;
        }

        [[nodiscard]] Resource Child(const std::string &path_segment) const
        {
            auto path = Path;
            path.push_back(path_segment);
            return { Namespace, path };
        }

        std::string Namespace;
        std::vector<std::string> Path;
    };

    using ResourceLocation = Resource<false>;
    using ResourceTag      = Resource<true>;

    template<bool TAG>
    std::ostream &operator<<(
            std::ostream &stream,
            const Resource<TAG> &resource)
    {
        return resource.Print(stream);
    }

    std::ostream &operator<<(
            std::ostream &stream,
            const TypePtr &type);

    template<typename T>
    std::filesystem::path operator/(
            std::filesystem::path first,
            const std::vector<T> &second)
    {
        for (const auto &segment : second)
            first /= segment;
        return first;
    }
}

namespace std
{
    template<>
    struct formatter<mcc::ResourceLocation> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::ResourceLocation &location,
                FormatContext &ctx) const
        {
            return formatter<string>::format(location.String(), ctx);
        }
    };
}
