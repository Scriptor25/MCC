#pragma once

#include <mcc/enums.hpp>

#include <format>
#include <memory>
#include <string>
#include <vector>

namespace mcc
{
    using IntegerT = long long;
    using FloatT   = long double;
    using IndexT   = unsigned long long;
    using CommandT = std::string;

    class TypeContext;
    class Parser;
    class Builder;
    class CommandVector;

    struct TreeNode;
    struct Statement;
    struct Expression;
    struct FormatNode;

    using TreeNodePtr   = std::unique_ptr<TreeNode>;
    using StatementPtr  = std::unique_ptr<Statement>;
    using ExpressionPtr = std::unique_ptr<Expression>;
    using FormatNodePtr = std::unique_ptr<FormatNode>;

    struct ValueBase;
    struct Constant;
    struct Instruction;
    struct Block;
    struct Function;

    template<typename T = ValueBase>
    struct Value;

    using ValuePtr       = std::shared_ptr<ValueBase>;
    using WeakValuePtr   = std::weak_ptr<ValueBase>;
    using ConstantPtr    = std::shared_ptr<Constant>;
    using InstructionPtr = std::shared_ptr<Instruction>;
    using BlockPtr       = std::shared_ptr<Block>;
    using FunctionPtr    = std::shared_ptr<Function>;

    struct Type;

    using TypePtr = std::shared_ptr<Type>;

    struct Parameter
    {
        std::string Name;
        TypePtr Type;
        FieldType_ FieldType;
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

    using CaseTarget    = std::pair<ConstantPtr, BlockPtr>;
    using CaseTargetMap = std::map<ConstantPtr, BlockPtr>;
}

template<typename T>
struct std::formatter<std::vector<T>> final : std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(
            const std::vector<T> &elements,
            FormatContext &ctx) const
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
