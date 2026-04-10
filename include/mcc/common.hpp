#pragma once

#include <mcc/enums.hpp>

#include <filesystem>
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

    struct ParameterRef
    {
        TypePtr Type;
        FieldType_ FieldType;
    };

    using ParameterList    = std::vector<Parameter>;
    using ParameterRefList = std::vector<ParameterRef>;

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

template<>
struct std::formatter<mcc::Parameter>
{
    template<typename FormatContext>
    constexpr auto parse(FormatContext &ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(
            const mcc::Parameter &parameter,
            FormatContext &ctx) const
    {
        auto is_constant = false, is_reference = false;
        switch (parameter.FieldType)
        {
        case mcc::FieldType_::ImmutableReference:
            is_constant  = true;
            is_reference = true;
            break;
        case mcc::FieldType_::MutableReference:
            is_reference = true;
            break;
        case mcc::FieldType_::Value:
            break;
        }

        return std::format_to(
                ctx.out(),
                "{}{}{}",
                is_constant ? "const " : "",
                is_reference ? "&" : "",
                parameter.Name);
    }
};

template<>
struct std::formatter<mcc::SourceLocation>
{
    template<typename FormatContext>
    constexpr auto parse(FormatContext &ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(
            const mcc::SourceLocation &where,
            FormatContext &ctx) const
    {
        return std::format_to(
                ctx.out(),
                "{}:{}:{}",
                std::filesystem::canonical(where.Filename).string(),
                where.Row,
                where.Col);
    }
};

template<typename T>
struct std::formatter<std::vector<T>>
{
    template<typename FormatContext>
    constexpr auto parse(FormatContext &ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(
            const std::vector<T> &elements,
            FormatContext &ctx) const
    {
        std::format_to(ctx.out(), "[ ");
        for (unsigned i = 0; i < elements.size(); ++i)
        {
            if (i)
                std::format_to(ctx.out(), ", ");
            std::format_to(ctx.out(), "{}", elements[i]);
        }
        return std::format_to(ctx.out(), " ]");
    }
};
