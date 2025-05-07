#pragma once

#include <format>
#include <memory>
#include <string>
#include <vector>
#include <mcc/enums.hpp>

namespace mcc
{
    using IntegerT = long long;
    using FloatT = long double;
    using IndexT = unsigned long long;
    using CommandT = std::string;

    using StatementPtr = std::unique_ptr<struct Statement>;
    using ExpressionPtr = std::unique_ptr<struct Expression>;
    using FormatNodePtr = std::unique_ptr<struct FormatNode>;

    using TargetAttributePtr = std::unique_ptr<struct TargetAttribute>;

    using ValuePtr = std::shared_ptr<struct Value>;
    using ConstantPtr = std::shared_ptr<struct Constant>;
    using InstructionPtr = std::shared_ptr<struct Instruction>;

    struct Context;
    class Parser;
    class Builder;

    struct CommandNode;
    struct FixedNode;
    struct EnumNode;
    struct SwitchNode;
    struct ValueNode;
    struct RuleNode;
    struct GreedyNode;

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

        std::string String() const
        {
            return (TAG ? "#" : "") + Namespace + ':' + Path;
        }

        std::string Namespace;
        std::string Path;
    };

    using ResourceLocation = Resource<false>;
    using ResourceTag = Resource<true>;
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
