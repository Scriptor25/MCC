#pragma once

#include <format>
#include <memory>
#include <string>
#include <vector>
#include <mcc/enums.hpp>

namespace mcc
{
    using IntegerT = long long;
    using FloatT   = long double;
    using IndexT   = unsigned long long;
    using CommandT = std::string;

    using TreeNodePtr   = std::unique_ptr<struct TreeNode>;
    using StatementPtr  = std::unique_ptr<struct Statement>;
    using ExpressionPtr = std::unique_ptr<struct Expression>;
    using FormatNodePtr = std::unique_ptr<struct FormatNode>;

    using ValuePtr       = std::shared_ptr<struct Value>;
    using ConstantPtr    = std::shared_ptr<struct Constant>;
    using InstructionPtr = std::shared_ptr<struct Instruction>;
    using BlockPtr       = std::shared_ptr<struct Block>;
    using FunctionPtr    = std::shared_ptr<struct Function>;

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
        std::ostream &Print(std::ostream &stream) const
        {
            return stream << (TAG ? "#" : "") << Namespace << ':' << Path;
        }

        [[nodiscard]] std::string String() const
        {
            return (TAG ? "#" : "") + Namespace + ':' + Path;
        }

        bool operator==(const Resource &other) const
        {
            return Namespace == other.Namespace && Path == other.Path;
        }

        std::string Namespace;
        std::string Path;
    };

    using ResourceLocation = Resource<false>;
    using ResourceTag      = Resource<true>;

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

    std::ostream &operator<<(std::ostream &stream, const TypePtr &type);
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
