#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/statement.hpp>

namespace mcc
{
    struct Expression : Statement
    {
        explicit Expression(SourceLocation where);

        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        [[nodiscard]] virtual ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const = 0;
    };

    struct ArrayExpression final : Expression
    {
        ArrayExpression(SourceLocation where, std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::vector<ExpressionPtr> Elements;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(SourceLocation where, std::string operator_, ExpressionPtr left, ExpressionPtr right);

        ExpressionPtr Merge();

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::string Operator;
        ExpressionPtr Left;
        ExpressionPtr Right;
    };

    struct CallExpression final : Expression
    {
        CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Callee;
        std::vector<ExpressionPtr> Arguments;
    };

    struct CommandExpression final : Expression
    {
        CommandExpression(SourceLocation where, CommandT command);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        CommandT Command;
    };

    struct ConstantExpression final : Expression
    {
        ConstantExpression(SourceLocation where, ConstantPtr value, std::string view);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        ConstantPtr Value;
        std::string View;
    };

    struct FormatExpression final : Expression
    {
        FormatExpression(SourceLocation where, std::vector<FormatNodePtr> nodes);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

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
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        bool Unless;
        ExpressionPtr Condition, Then, Else;
    };

    struct ResourceExpression final : Expression
    {
        ResourceExpression(SourceLocation where, ResourceLocation location, ExpressionPtr state, ExpressionPtr data);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        ResourceLocation Location;
        ExpressionPtr State;
        ExpressionPtr Data;
    };

    struct ObjectExpression final : Expression
    {
        ObjectExpression(SourceLocation where, std::map<std::string, ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::map<std::string, ExpressionPtr> Elements;
    };

    struct SubscriptExpression final : Expression
    {
        SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Array, Index;
    };

    struct SwitchExpression final : Expression
    {
        SwitchExpression(
            SourceLocation where,
            ExpressionPtr condition,
            ExpressionPtr default_,
            std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Condition, Default;
        std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> Cases;
    };

    struct SymbolExpression final : Expression
    {
        SymbolExpression(SourceLocation where, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::string Name;
    };

    struct UnaryExpression final : Expression
    {
        UnaryExpression(SourceLocation where, std::string operator_, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::string Operator;
        ExpressionPtr Operand;
    };

    struct VectorExpression final : Expression
    {
        VectorExpression(SourceLocation where, std::string operator_, std::vector<ExpressionPtr> operands);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr GenerateValue(Builder &builder, BlockPtr landing_pad) const override;

        std::string Operator;
        std::vector<ExpressionPtr> Operands;
    };
}
