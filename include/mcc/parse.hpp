#pragma once

#include <memory>
#include <mcc/lex.hpp>
#include <mcc/tree.hpp>

namespace mcc
{
    class Parser
    {
    public:
        Parser(std::istream &stream, std::string filename);

        explicit operator bool() const;
        StatementPtr operator()();

    private:
        void Get();
        Token &Next();

        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;

        bool SkipIf(TokenType type, const std::string &value = {});

        Token Skip();
        Token Expect(TokenType type, std::string value = {});
        Token ExpectEnum(std::vector<std::string> values);

        template<typename... Args>
        Token ExpectEnum(Args... args)
        {
            return ExpectEnum({args...});
        }

        ResourceLocation ParseResourceLocation();

        StatementPtr ParseStatement();
        StatementPtr ParseNamespaceStatement();
        StatementPtr ParseDefineStatement();

        ExpressionPtr ParseExpression();

        ExpressionPtr ParseResourceExpression();
        ExpressionPtr ParseTargetExpression();

        ExpressionPtr ParseArrayExpression();
        ExpressionPtr ParseObjectExpression();

        ExpressionPtr ParseFormatExpression();

        ExpressionPtr ParseIfUnlessExpression(bool unless);
        ExpressionPtr ParseReturnExpression();

        ExpressionPtr ParsePrimaryExpression();
        ExpressionPtr ParseCallExpression();
        ExpressionPtr ParseBinaryExpression(ExpressionPtr left, ExpressionPtr (Parser::*next)(), unsigned min_pre);

        std::istream &m_Stream;
        int m_Buf = -1;
        Location m_Location;
        Token m_Token;
    };
}
