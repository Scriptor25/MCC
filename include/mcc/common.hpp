#pragma once

#include <filesystem>
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <mcc/enums.hpp>

namespace mcc
{
    using IntegerT = long long;
    using FloatT = long double;
    using IndexT = unsigned long long;
    using CommandT = std::string;

    using TreeNodePtr = std::unique_ptr<struct TreeNode>;
    using StatementPtr = std::unique_ptr<struct Statement>;
    using ExpressionPtr = std::unique_ptr<struct Expression>;
    using FormatNodePtr = std::unique_ptr<struct FormatNode>;

    using ValuePtr = std::shared_ptr<struct Value>;
    using ConstantPtr = std::shared_ptr<struct Constant>;
    using InstructionPtr = std::shared_ptr<struct Instruction>;
    using BlockPtr = std::shared_ptr<struct Block>;
    using FunctionPtr = std::shared_ptr<struct Function>;

    using TypePtr = std::shared_ptr<struct Type>;

    struct Parameter
    {
        std::string Name;
        TypePtr Type;
    };

    using ParameterList = std::vector<Parameter>;

    struct Frame
    {
        BlockPtr Head;
        BlockPtr Tail;
        BlockPtr LandingPad;

        unsigned Flags = 0;
    };

    struct SourceLocation
    {
        std::string Filename;
        unsigned Row = 0;
        unsigned Col = 0;
    };

    class TypeContext;
    class Parser;
    class Builder;

    class CommandVector
    {
    public:
        explicit CommandVector(std::vector<CommandT> &commands)
            : m_Commands(commands)
        {
        }

        CommandVector &Append(CommandT command)
        {
            m_Commands.emplace_back(std::move(command));
            return *this;
        }

        template<typename... Args>
        CommandVector &Append(std::string_view format, Args &&... args)
        {
            return Append(std::vformat(std::move(format), std::make_format_args(args...)));
        }

    private:
        std::vector<CommandT> &m_Commands;
    };

    template<bool TAG>
    struct Resource
    {
        Resource() = default;

        Resource(const std::vector<std::string> &path)
            : Path(path)
        {
        }

        Resource(const std::string &namespace_, const std::vector<std::string> &path)
            : Namespace(namespace_),
              Path(path)
        {
        }

        Resource(const Resource &other)
            : Namespace(other.Namespace),
              Path(other.Path)
        {
        }

        Resource &operator=(const Resource &other)
        {
            Namespace = other.Namespace;
            Path = other.Path;
            return *this;
        }

        Resource(Resource &&other) noexcept
            : Namespace(std::move(other.Namespace)),
              Path(std::move(other.Path))
        {
        }

        Resource &operator=(Resource &&other) noexcept
        {
            Namespace = std::move(other.Namespace);
            Path = std::move(other.Path);
            return *this;
        }

        Resource(const Resource<!TAG> &other)
            : Namespace(other.Namespace),
              Path(other.Path)
        {
        }

        Resource(Resource<!TAG> &&other)
            : Namespace(std::move(other.Namespace)),
              Path(std::move(other.Path))
        {
        }

        Resource &operator=(Resource<!TAG> &&other)
        {
            Namespace = std::move(other.Namespace);
            Path = std::move(other.Path);
            return *this;
        }

        std::ostream &Print(std::ostream &stream) const
        {
            stream << (TAG ? "#" : "") << Namespace << ':';
            for (unsigned i = 0; i < Path.size(); ++i)
            {
                if (i)
                    stream << '/';
                stream << Path[i];
            }
            return stream;
        }

        [[nodiscard]] std::string String() const
        {
            std::stringstream stream;
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
            path.emplace_back(path_segment);
            return { Namespace, path };
        }

        std::string Namespace;
        std::vector<std::string> Path;
    };

    using ResourceLocation = Resource<false>;
    using ResourceTag = Resource<true>;

    struct Result
    {
        bool operator==(const Result &result) const;

        ResultTypeE Type;

        bool WithArgument = false;

        /* Value */
        std::string Value;
        bool NotNull;

        /* Reference */
        ReferenceTypeE ReferenceType;
        std::string Target;
        std::string Path;

        /* Argument */
        std::string Name;
    };

    template<bool TAG>
    std::ostream &operator<<(std::ostream &stream, const Resource<TAG> &resource)
    {
        return resource.Print(stream);
    }

    std::ostream &operator<<(std::ostream &stream, const TypePtr &type);

    template<typename T>
    std::filesystem::path operator/(std::filesystem::path first, const std::vector<T> &second)
    {
        for (const auto &segment : second)
            first /= segment;
        return first;
    }
}

namespace std
{
    template<>
    struct formatter<mcc::ResourceLocation> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::ResourceLocation &location, FormatContext &ctx) const
        {
            return formatter<string>::format(location.String(), ctx);
        }
    };

    template<typename T>
    struct formatter<vector<T>> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const vector<T> &elements, FormatContext &ctx) const
        {
            string s = "[ ";

            for (unsigned i = 0; i < elements.size(); ++i)
            {
                if (i > 0)
                    s += ", ";
                s += std::format("{}", elements[i]);
            }

            s += " ]";

            return formatter<string>::format(s, ctx);
        }
    };
}
