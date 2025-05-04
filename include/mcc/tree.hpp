#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    struct Statement
    {
        virtual ~Statement() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual void Gen(Context &context) const = 0;
    };

    struct Expression
    {
        virtual ~Expression() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        [[nodiscard]] virtual ValuePtr Gen(Builder &builder, bool inline_) const = 0;
    };

    struct FormatNode
    {
        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        [[nodiscard]] virtual ValuePtr Gen(Builder &builder, bool inline_) const = 0;
    };

    struct StringNode final : FormatNode
    {
        explicit StringNode(std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::string Value;
    };

    struct ExpressionNode final : FormatNode
    {
        explicit ExpressionNode(ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        ExpressionPtr Expression;
    };

    struct NamespaceStatement final : Statement
    {
        explicit NamespaceStatement(std::string namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Gen(Context &context) const override;

        std::string Namespace;
    };

    struct DefineStatement final : Statement
    {
        DefineStatement(
            ResourceLocation location,
            std::vector<std::string> parameters,
            std::vector<ResourceLocation> tags,
            std::vector<ExpressionPtr> expressions);

        std::ostream &Print(std::ostream &stream) const override;
        void Gen(Context &context) const override;

        ResourceLocation Location;
        std::vector<std::string> Parameters;
        std::vector<ResourceLocation> Tags;
        std::vector<ExpressionPtr> Expressions;
    };

    struct ConstantExpression final : Expression
    {
        ConstantExpression(ConstantPtr value, std::string view);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        ConstantPtr Value;
        std::string View;
    };

    struct ResourceExpression final : Expression
    {
        ResourceExpression(std::string namespace_, std::string path);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        ResourceLocation Location;
    };

    struct ArrayExpression final : Expression
    {
        explicit ArrayExpression(std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::vector<ExpressionPtr> Elements;
    };

    struct ObjectExpression final : Expression
    {
        explicit ObjectExpression(std::map<std::string, ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::map<std::string, ExpressionPtr> Elements;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::string Operator;
        ExpressionPtr Left;
        ExpressionPtr Right;
    };

    struct CallExpression final : Expression
    {
        CallExpression(std::string callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::string Callee;
        std::vector<ExpressionPtr> Arguments;
    };

    struct FormatExpression final : Expression
    {
        explicit FormatExpression(std::vector<FormatNodePtr> nodes);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::vector<FormatNodePtr> Nodes;
    };

    struct IfUnlessExpression final : Expression
    {
        IfUnlessExpression(bool unless, ExpressionPtr condition, ExpressionPtr then, ExpressionPtr else_);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        bool Unless;
        ExpressionPtr Condition, Then, Else;
    };

    struct ReturnExpression final : Expression
    {
        explicit ReturnExpression(ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        ExpressionPtr Value;
    };

    struct SymbolExpression final : Expression
    {
        explicit SymbolExpression(std::string id);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::string ID;
    };
}
