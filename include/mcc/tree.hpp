#pragma once

#include <map>
#include <memory>
#include <vector>

namespace mcc
{
    using StatementPtr = std::unique_ptr<class Statement>;
    using ExpressionPtr = std::unique_ptr<class Expression>;
    using FormatNodePtr = std::unique_ptr<class FormatNode>;

    struct ResourceLocation
    {
        std::ostream &Print(std::ostream &stream) const;

        std::string NS;
        std::string ID;
    };

    class Statement
    {
    public:
        virtual ~Statement() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };

    class Expression
    {
    public:
        virtual ~Expression() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };

    class NamespaceStatement final : public Statement
    {
    public:
        explicit NamespaceStatement(std::string id);

        std::ostream &Print(std::ostream &stream) const override;

        std::string GetID() const;

    private:
        std::string m_ID;
    };

    class DefineStatement final : public Statement
    {
    public:
        DefineStatement(
            ResourceLocation location,
            std::vector<std::string> parameters,
            std::vector<ResourceLocation> tags,
            std::vector<ExpressionPtr> expressions);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ResourceLocation m_Location;
        std::vector<std::string> m_Parameters;
        std::vector<ResourceLocation> m_Tags;
        std::vector<ExpressionPtr> m_Expressions;
    };

    class BoolExpression final : public Expression
    {
    public:
        explicit BoolExpression(bool value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        bool m_Value;
    };

    class IntegerExpression final : public Expression
    {
    public:
        explicit IntegerExpression(int64_t value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        int64_t m_Value;
    };

    class FloatExpression final : public Expression
    {
    public:
        explicit FloatExpression(double value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        double m_Value;
    };

    class StringExpression final : public Expression
    {
    public:
        explicit StringExpression(std::string value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::string m_Value;
    };

    class SymbolExpression final : public Expression
    {
    public:
        explicit SymbolExpression(std::string id);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::string m_ID;
    };

    class TargetExpression final : public Expression
    {
    public:
        explicit TargetExpression(std::string id);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::string m_ID;
    };

    class ArrayExpression final : public Expression
    {
    public:
        explicit ArrayExpression(std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<ExpressionPtr> m_Elements;
    };

    class ObjectExpression final : public Expression
    {
    public:
        explicit ObjectExpression(std::map<std::string, ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::map<std::string, ExpressionPtr> m_Elements;
    };

    class RelativeOffsetExpression final : public Expression
    {
    public:
        RelativeOffsetExpression();

        std::ostream &Print(std::ostream &stream) const override;
    };

    class LocalOffsetExpression final : public Expression
    {
    public:
        LocalOffsetExpression();

        std::ostream &Print(std::ostream &stream) const override;
    };

    class BinaryExpression final : public Expression
    {
    public:
        BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::string m_Operator;
        ExpressionPtr m_Left;
        ExpressionPtr m_Right;
    };

    class CallExpression final : public Expression
    {
    public:
        CallExpression(ExpressionPtr callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ExpressionPtr m_Callee;
        std::vector<ExpressionPtr> m_Arguments;
    };

    class FormatNode
    {
    public:
        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };

    class StringNode final : public FormatNode
    {
    public:
        explicit StringNode(std::string value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::string m_Value;
    };

    class ExpressionNode final : public FormatNode
    {
    public:
        explicit ExpressionNode(ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        ExpressionPtr m_Expression;
    };

    class FormatExpression final : public Expression
    {
    public:
        explicit FormatExpression(std::vector<FormatNodePtr> nodes);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<FormatNodePtr> m_Nodes;
    };
}
