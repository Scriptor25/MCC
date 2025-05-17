#pragma once

#include <iosfwd>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>

namespace mcc
{
    struct Statement
    {
        explicit Statement(SourceLocation where);

        virtual ~Statement() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual void Generate(Builder &builder, BlockPtr landing_pad) const = 0;

        SourceLocation Where;
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
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        ResourceLocation Location;
        std::vector<std::string> Parameters;
        std::vector<ResourceLocation> Tags;
        StatementPtr Body;
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
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Condition;
        StatementPtr Prefix, Suffix, Do;
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
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        bool Unless;
        ExpressionPtr Condition;
        StatementPtr Then, Else;
    };

    struct MultiStatement final : Statement
    {
        MultiStatement(SourceLocation where, std::vector<StatementPtr> statements);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        std::vector<StatementPtr> Statements;
    };

    struct NamespaceStatement final : Statement
    {
        NamespaceStatement(SourceLocation where, std::string namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        std::string Namespace;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(SourceLocation where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Value;
    };

    struct SwitchStatement final : Statement
    {
        SwitchStatement(
            SourceLocation where,
            ExpressionPtr condition,
            StatementPtr default_,
            std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Condition;
        StatementPtr Default;
        std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> Cases;
    };

    struct ThrowStatement final : Statement
    {
        ThrowStatement(SourceLocation where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Value;
    };

    struct TryCatchStatement final : Statement
    {
        TryCatchStatement(SourceLocation where, StatementPtr try_, StatementPtr catch_, std::string variable);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, BlockPtr landing_pad) const override;

        StatementPtr Try, Catch;
        std::string Variable;
    };
}
