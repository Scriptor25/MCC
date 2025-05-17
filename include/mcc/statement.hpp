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
        explicit Statement(const SourceLocation &where);

        virtual ~Statement() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual void Generate(Builder &builder, const Frame &frame) const = 0;

        SourceLocation Where;
    };

    struct BreakStatement final : Statement
    {
        explicit BreakStatement(const SourceLocation &where);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;
    };

    struct ContinueStatement final : Statement
    {
        explicit ContinueStatement(const SourceLocation &where);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;
    };

    struct DefineStatement final : Statement
    {
        DefineStatement(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ParameterList &parameters,
            TypePtr type,
            const std::vector<ResourceLocation> &tags,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        ResourceLocation Location;
        ParameterList Parameters;
        TypePtr Type;
        std::vector<ResourceLocation> Tags;
        StatementPtr Body;
    };

    struct ForStatement final : Statement
    {
        ForStatement(
            const SourceLocation &where,
            StatementPtr prefix,
            ExpressionPtr condition,
            StatementPtr suffix,
            StatementPtr do_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        ExpressionPtr Condition;
        StatementPtr Prefix, Suffix, Do;
    };

    struct IfUnlessStatement final : Statement
    {
        IfUnlessStatement(
            const SourceLocation &where,
            bool unless,
            ExpressionPtr condition,
            StatementPtr then,
            StatementPtr else_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        bool Unless;
        ExpressionPtr Condition;
        StatementPtr Then, Else;
    };

    struct MultiStatement final : Statement
    {
        MultiStatement(const SourceLocation &where, std::vector<StatementPtr> statements);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        std::vector<StatementPtr> Statements;
    };

    struct NamespaceStatement final : Statement
    {
        NamespaceStatement(const SourceLocation &where, const std::string &namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        std::string Namespace;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(const SourceLocation &where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        ExpressionPtr Value;
    };

    struct SwitchStatement final : Statement
    {
        SwitchStatement(
            const SourceLocation &where,
            ExpressionPtr condition,
            StatementPtr default_,
            std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        ExpressionPtr Condition;
        StatementPtr Default;
        std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> Cases;
    };

    struct ThrowStatement final : Statement
    {
        ThrowStatement(const SourceLocation &where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        ExpressionPtr Value;
    };

    struct TryCatchStatement final : Statement
    {
        TryCatchStatement(
            const SourceLocation &where,
            StatementPtr try_,
            StatementPtr catch_,
            const std::string &variable,
            TypePtr error_type);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        StatementPtr Try, Catch;
        std::string Variable;
        TypePtr ErrorType;
    };

    struct VariableStatement final : Statement
    {
        VariableStatement(const SourceLocation &where, bool constant, const std::string &name, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, const Frame &frame) const override;

        bool Constant;
        std::string Name;
        ExpressionPtr Value;
    };
}
