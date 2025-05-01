#pragma once

#include <map>
#include <memory>
#include <vector>

namespace mcc
{
    using StatementPtr = std::unique_ptr<struct Statement>;
    using ExpressionPtr = std::unique_ptr<struct Expression>;
    using FormatNodePtr = std::unique_ptr<struct FormatNode>;

    struct Context;

    struct ResourceLocation
    {
        std::ostream &Print(std::ostream &stream) const;
        [[nodiscard]] std::string Gen(const ResourceLocation &location) const;

        std::string Namespace;
        std::string Path;
    };

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
        [[nodiscard]] virtual bool IsConstant() const = 0;
        [[nodiscard]] virtual std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const = 0;
    };

    struct FormatNode
    {
        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual bool IsText() const = 0;
        virtual bool IsConstant() const = 0;
        virtual std::string Gen(const ResourceLocation &location, std::vector<std::string> &commands) const = 0;
    };

    struct StringNode final : FormatNode
    {
        explicit StringNode(std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        bool IsText() const override;
        bool IsConstant() const override;
        std::string Gen(const ResourceLocation &location, std::vector<std::string> &commands) const override;

        std::string Value;
    };

    struct ExpressionNode final : FormatNode
    {
        explicit ExpressionNode(ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;
        bool IsText() const override;
        bool IsConstant() const override;
        std::string Gen(const ResourceLocation &location, std::vector<std::string> &commands) const override;

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

    struct BoolExpression final : Expression
    {
        explicit BoolExpression(bool value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        bool Value;
    };

    struct IntegerExpression final : Expression
    {
        explicit IntegerExpression(int64_t value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        int64_t Value;
    };

    struct FloatExpression final : Expression
    {
        explicit FloatExpression(double value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        double Value;
    };

    struct StringExpression final : Expression
    {
        explicit StringExpression(std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::string Value;
    };

    struct ResourceExpression final : Expression
    {
        ResourceExpression(std::string ns, std::string id);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        ResourceLocation Location;
    };

    struct TargetExpression final : Expression
    {
        explicit TargetExpression(std::string specifier);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::string Specifier;
    };

    struct ArrayExpression final : Expression
    {
        explicit ArrayExpression(std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::vector<ExpressionPtr> Elements;
    };

    struct ObjectExpression final : Expression
    {
        explicit ObjectExpression(std::map<std::string, ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::map<std::string, ExpressionPtr> Elements;
    };

    struct RelativeOffsetExpression final : Expression
    {
        explicit RelativeOffsetExpression(double offset);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        double Offset;
    };

    struct LocalOffsetExpression final : Expression
    {
        explicit LocalOffsetExpression(double offset);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        double Offset;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::string Operator;
        ExpressionPtr Left;
        ExpressionPtr Right;
    };

    struct CallExpression final : Expression
    {
        CallExpression(std::string callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::string Callee;
        std::vector<ExpressionPtr> Arguments;
    };

    struct FormatExpression final : Expression
    {
        explicit FormatExpression(std::vector<FormatNodePtr> nodes);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::vector<FormatNodePtr> Nodes;
    };

    struct IfExpression final : Expression
    {
        explicit IfExpression(ExpressionPtr condition, ExpressionPtr then, ExpressionPtr else_);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        ExpressionPtr Condition;
        ExpressionPtr Then;
        ExpressionPtr Else;
    };

    struct ReturnExpression final : Expression
    {
        explicit ReturnExpression(ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        ExpressionPtr Value;
    };

    struct SymbolExpression final : Expression
    {
        explicit SymbolExpression(std::string id);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] bool IsConstant() const override;
        [[nodiscard]] std::string Gen(
            const ResourceLocation &location,
            std::vector<std::string> &commands,
            bool inlined) const override;

        std::string ID;
    };
}
