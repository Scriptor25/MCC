#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/resource.hpp>

namespace mcc
{
    class Parser
    {
    public:
        Parser(TypeContext &context, std::istream &stream, const std::string &filename);
        Parser(TypeContext &context, std::istream &stream, SourceLocation location);

        [[nodiscard]] size_t Count() const;

        explicit operator bool() const;
        TreeNodePtr operator()();

    private:
        void Get();
        Token &Next();

        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;
        [[nodiscard]] bool AtAny(const std::vector<TokenType> &types) const;
        [[nodiscard]] bool AtEnum(const std::vector<const char *> &values) const;

        template<typename... Args>
        [[nodiscard]] bool AtEnum(Args... args) const
        {
            return AtEnum({ args... });
        }

        template<typename... Args>
        [[nodiscard]] bool AtAny(Args... args) const
        {
            return AtAny({ args... });
        }

        bool SkipIf(TokenType type, const std::string &value = {});

        Token Skip();
        Token Expect(TokenType type, std::string value = {});
        Token ExpectAny(const std::vector<TokenType> &types);
        Token ExpectEnum(const std::vector<const char *> &values);

        template<typename... Args>
        Token ExpectEnum(Args... args)
        {
            return ExpectEnum({ args... });
        }

        template<typename... Args>
        Token ExpectAny(Args... args)
        {
            return ExpectAny({ args... });
        }

        ResourceLocation ParseResourceLocation(bool simple_path = false);

        TypePtr ParseType();
        TypePtr ParseBaseType();
        TypePtr ParseArrayType();
        TypePtr ParseUnionType();

        TreeNodePtr ParseTreeNode();
        TreeNodePtr ParseDefineNode();
        TreeNodePtr ParseGlobalNode();
        TreeNodePtr ParseIncludeNode();
        TreeNodePtr ParseNamespaceNode();
        TreeNodePtr ParseTypeNode();

        StatementPtr ParseStatement();
        StatementPtr ParseBreakStatement();
        StatementPtr ParseContinueStatement();
        StatementPtr ParseDeleteStatement();
        StatementPtr ParseForStatement();
        StatementPtr ParseForEachStatement();
        StatementPtr ParseIfUnlessStatement();
        StatementPtr ParseMultiStatement();
        StatementPtr ParseReturnStatement();
        StatementPtr ParseSwitchStatement();
        StatementPtr ParseThrowStatement();
        StatementPtr ParseTryCatchStatement();
        StatementPtr ParseVariableStatement();

        ExpressionPtr ParseExpression();
        ExpressionPtr ParseArrayExpression();
        ExpressionPtr ParseBinaryExpression(ExpressionPtr left, unsigned min_pre);
        ExpressionPtr ParseCommandExpression();
        ExpressionPtr ParseRefExpression();
        ExpressionPtr ParseFormatExpression();
        ExpressionPtr ParseIfUnlessExpression();
        ExpressionPtr ParseNumberExpression();
        ExpressionPtr ParseMacroExpression();
        ExpressionPtr ParseObjectExpression();
        ExpressionPtr ParseOperandExpression();
        ExpressionPtr ParsePrimaryExpression();
        ExpressionPtr ParseStringExpression();
        ExpressionPtr ParseSwitchExpression();
        ExpressionPtr ParseSymbolExpression();

        TypeContext &m_Context;

        std::istream &m_Stream;

        int m_Buf = -1;
        SourceLocation m_Where;
        Token m_Token;

        size_t m_Count = 0;
    };
}
