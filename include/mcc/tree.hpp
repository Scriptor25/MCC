#pragma once

#include <map>
#include <memory>
#include <vector>
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

    struct TargetAttribute
    {
        explicit TargetAttribute(bool invert);

        virtual ~TargetAttribute() = default;
        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual std::string String() const = 0;

        bool Invert;
    };

    struct IntegerAttribute final : TargetAttribute
    {
        IntegerAttribute(bool invert, IntegerT value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        IntegerT Value;
    };

    struct FloatAttribute final : TargetAttribute
    {
        FloatAttribute(bool invert, FloatT value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        FloatT Value;
    };

    struct RangeAttribute final : TargetAttribute
    {
        RangeAttribute(bool invert, std::optional<IntegerT> beg, std::optional<IntegerT> end);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::optional<IntegerT> Beg, End;
    };

    struct StringAttribute final : TargetAttribute
    {
        StringAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::string Value;
    };

    struct EnumAttribute final : TargetAttribute
    {
        EnumAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::string Value;
    };

    struct NameAttribute final : TargetAttribute
    {
        NameAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::string Value;
    };

    struct MapAttribute final : TargetAttribute
    {
        MapAttribute(bool invert, std::map<std::string, TargetAttributePtr> values);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::map<std::string, TargetAttributePtr> Values;
    };

    struct ResourceMapAttribute final : TargetAttribute
    {
        ResourceMapAttribute(bool invert, std::vector<std::pair<ResourceLocation, TargetAttributePtr>> values);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        std::vector<std::pair<ResourceLocation, TargetAttributePtr>> Values;
    };

    struct NBTAttribute final : TargetAttribute
    {
        NBTAttribute(bool invert, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        ExpressionPtr Value;
    };

    struct ResourceAttribute final : TargetAttribute
    {
        ResourceAttribute(bool invert, ResourceLocation value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        ResourceLocation Value;
    };

    struct TagAttribute final : TargetAttribute
    {
        TagAttribute(bool invert, ResourceTag value);

        std::ostream &Print(std::ostream &stream) const override;
        std::string String() const override;

        ResourceTag Value;
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

    struct BooleanExpression final : Expression
    {
        explicit BooleanExpression(bool value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        bool Value;
    };

    struct IntegerExpression final : Expression
    {
        explicit IntegerExpression(int64_t value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        int64_t Value;
    };

    struct FloatExpression final : Expression
    {
        explicit FloatExpression(FloatT value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        FloatT Value;
    };

    struct StringExpression final : Expression
    {
        explicit StringExpression(std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        std::string Value;
    };

    struct ResourceExpression final : Expression
    {
        ResourceExpression(std::string namespace_, std::string path);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        ResourceLocation Location;
    };

    struct TargetExpression final : Expression
    {
        TargetExpression(TargetSelectorE selector, std::map<std::string, std::vector<TargetAttributePtr>> attributes);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        TargetSelectorE Selector;
        std::map<std::string, std::vector<TargetAttributePtr>> Attributes;
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

    struct RelativeOffsetExpression final : Expression
    {
        explicit RelativeOffsetExpression(double offset);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        double Offset;
    };

    struct LocalOffsetExpression final : Expression
    {
        explicit LocalOffsetExpression(double offset);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Gen(Builder &builder, bool inline_) const override;

        double Offset;
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
