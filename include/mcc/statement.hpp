#pragma once

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <mcc/tree.hpp>

namespace mcc
{
    struct Statement : TreeNode
    {
        explicit Statement(const SourceLocation &where);

        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const override;

        virtual void Generate(Builder &builder, Frame &frame) const = 0;
    };

    struct BreakStatement final : Statement
    {
        explicit BreakStatement(const SourceLocation &where);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;
    };

    struct ContinueStatement final : Statement
    {
        explicit ContinueStatement(const SourceLocation &where);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;
    };

    struct DeleteStatement final : Statement
    {
        DeleteStatement(const SourceLocation &where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        ExpressionPtr Value;
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
        void Generate(Builder &builder, Frame &frame) const override;

        ExpressionPtr Condition;
        StatementPtr Prefix, Suffix, Do;
    };

    struct ForEachStatement final : Statement
    {
        ForEachStatement(
            const SourceLocation &where,
            bool is_constant,
            std::string name,
            ExpressionPtr iterable,
            StatementPtr do_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        bool IsConstant;
        std::string Name;
        ExpressionPtr Iterable;
        StatementPtr Do;
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
        void Generate(Builder &builder, Frame &frame) const override;

        bool Unless;
        ExpressionPtr Condition;
        StatementPtr Then, Else;
    };

    struct MultiStatement final : Statement
    {
        MultiStatement(const SourceLocation &where, std::vector<StatementPtr> statements);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        std::vector<StatementPtr> Statements;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(const SourceLocation &where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

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
        void Generate(Builder &builder, Frame &frame) const override;

        ExpressionPtr Condition;
        StatementPtr Default;
        std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> Cases;
    };

    struct ThrowStatement final : Statement
    {
        ThrowStatement(const SourceLocation &where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        ExpressionPtr Value;
    };

    struct TryCatchStatement final : Statement
    {
        TryCatchStatement(
            const SourceLocation &where,
            StatementPtr try_,
            StatementPtr catch_,
            const std::string &variable,
            const TypePtr &error_type);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        StatementPtr Try, Catch;
        std::string Variable;
        TypePtr ErrorType;
    };

    struct VariableStatement final : Statement
    {
        VariableStatement(
            const SourceLocation &where,
            DeclaratorE declarator,
            bool is_reference,
            std::vector<std::string> names,
            TypePtr type,
            ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder, Frame &frame) const override;

        DeclaratorE Declarator;
        bool IsReference;
        std::vector<std::string> Names;
        TypePtr Type;
        ExpressionPtr Value;
    };
}
