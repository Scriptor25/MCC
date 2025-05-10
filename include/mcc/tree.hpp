#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>

namespace mcc
{
    struct Statement
    {
        explicit Statement(SourceLocation where);

        virtual ~Statement() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual void Generate(Builder &builder) const = 0;

        SourceLocation Where;
    };

    struct Expression : Statement
    {
        explicit Expression(SourceLocation where);

        void Generate(Builder &builder) const override;

        [[nodiscard]] virtual ValuePtr GenerateValue(Builder &builder) const = 0;
    };

    struct FormatNode
    {
        explicit FormatNode(SourceLocation where);

        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        [[nodiscard]] virtual ValuePtr Generate(Builder &builder) const = 0;

        SourceLocation Where;
    };

    struct StringNode final : FormatNode
    {
        StringNode(SourceLocation where, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder) const override;

        std::string Value;
    };

    struct ExpressionNode final : FormatNode
    {
        ExpressionNode(SourceLocation where, ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder) const override;

        ExpressionPtr Expression;
    };

    struct NamespaceStatement final : Statement
    {
        NamespaceStatement(SourceLocation where, std::string namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        std::string Namespace;
    };

    struct DefineStatement final : Statement
    {
        DefineStatement(
            SourceLocation where,
            ResourceLocation location,
            std::vector<std::string> parameters,
            std::vector<ResourceLocation> tags,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        ResourceLocation Location;
        std::vector<std::string> Parameters;
        std::vector<ResourceLocation> Tags;
        StatementPtr Body;
    };

    struct IfUnlessStatement final : Statement
    {
        IfUnlessStatement(
            SourceLocation where,
            bool unless,
            ExpressionPtr condition,
            StatementPtr then,
            StatementPtr else_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        bool Unless;
        ExpressionPtr Condition;
        StatementPtr Then, Else;
    };

    struct ForStatement final : Statement
    {
        ForStatement(
            SourceLocation where,
            StatementPtr prefix,
            ExpressionPtr condition,
            StatementPtr suffix,
            StatementPtr do_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        StatementPtr Prefix;
        ExpressionPtr Condition;
        StatementPtr Suffix;
        StatementPtr Do;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(SourceLocation where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        ExpressionPtr Value;
    };

    struct MultiStatement final : Statement
    {
        MultiStatement(SourceLocation where, std::vector<StatementPtr> statements);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;

        std::vector<StatementPtr> Statements;
    };

    struct ConstantExpression final : Expression
    {
        ConstantExpression(SourceLocation where, ConstantPtr value, std::string view);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        ConstantPtr Value;
        std::string View;
    };

    struct ResourceExpression final : Expression
    {
        ResourceExpression(SourceLocation where, ResourceLocation location, ExpressionPtr state, ExpressionPtr data);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        ResourceLocation Location;
        ExpressionPtr State;
        ExpressionPtr Data;
    };

    struct ArrayExpression final : Expression
    {
        ArrayExpression(SourceLocation where, std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::vector<ExpressionPtr> Elements;
    };

    struct ObjectExpression final : Expression
    {
        ObjectExpression(SourceLocation where, std::map<std::string, ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::map<std::string, ExpressionPtr> Elements;
    };

    struct VectorExpression final : Expression
    {
        VectorExpression(SourceLocation where, std::string operator_, std::vector<ExpressionPtr> operands);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::string Operator;
        std::vector<ExpressionPtr> Operands;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(SourceLocation where, std::string operator_, ExpressionPtr left, ExpressionPtr right);

        ExpressionPtr Merge();

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::string Operator;
        ExpressionPtr Left;
        ExpressionPtr Right;
    };

    struct FormatExpression final : Expression
    {
        FormatExpression(SourceLocation where, std::vector<FormatNodePtr> nodes);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::vector<FormatNodePtr> Nodes;
    };

    struct IfUnlessExpression final : Expression
    {
        IfUnlessExpression(
            SourceLocation where,
            bool unless,
            ExpressionPtr condition,
            ExpressionPtr then,
            ExpressionPtr else_);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        bool Unless;
        ExpressionPtr Condition, Then, Else;
    };

    struct SymbolExpression final : Expression
    {
        SymbolExpression(SourceLocation where, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::string Name;
    };

    struct CallExpression final : Expression
    {
        CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        ExpressionPtr Callee;
        std::vector<ExpressionPtr> Arguments;
    };

    struct CommandExpression final : Expression
    {
        CommandExpression(SourceLocation where, CommandT command);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        CommandT Command;
    };

    struct UnaryExpression final : Expression
    {
        UnaryExpression(SourceLocation where, std::string operator_, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder) const override;

        std::string Operator;
        ExpressionPtr Operand;
    };
}
