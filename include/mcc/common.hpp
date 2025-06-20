#pragma once

#include <filesystem>
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

    class TypeContext;
    class Parser;
    class Builder;
    class CommandVector;

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
        E_FieldType FieldType;
    };

    using ParameterList = std::vector<Parameter>;

    struct Frame
    {
        BlockPtr Head;
        BlockPtr Tail;
        BlockPtr LandingPad;

        TypePtr ResultType;

        unsigned Flags = 0;
    };

    struct SourceLocation
    {
        std::string Filename;
        unsigned Row = 0;
        unsigned Col = 0;
    };
}

template<typename T>
struct std::formatter<std::vector<T>> final : std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(const std::vector<T> &elements, FormatContext &ctx) const
    {
        std::string s = "[ ";

        for (unsigned i = 0; i < elements.size(); ++i)
        {
            if (i)
                s += ", ";
            s += std::format("{}", elements[i]);
        }

        s += " ]";

        return std::formatter<std::string>::format(s, ctx);
    }
};
