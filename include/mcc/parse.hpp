#pragma once

#include <functional>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>

namespace mcc
{
    using Parse = std::function<TargetAttributePtr(Parser &self, bool invert)>;

    class Parser
    {
    public:
        Parser(Context &context, std::istream &stream, std::string filename);
        Parser(Context &context, std::istream &stream, SourceLocation location);

        [[nodiscard]] size_t Count() const;

        explicit operator bool() const;
        StatementPtr operator()();

    private:
        void Get();
        Token &Next();

        [[nodiscard]] bool At(TokenType type, const std::string &value = {}) const;
        [[nodiscard]] bool AtAny(const std::vector<TokenType> &types) const;
        [[nodiscard]] bool AtEnum(const std::vector<const char *> &values) const;

        template<typename... Args>
        [[nodiscard]] bool AtEnum(Args... args) const
        {
            return AtEnum({args...});
        }

        template<typename... Args>
        [[nodiscard]] bool AtAny(Args... args) const
        {
            return AtAny({args...});
        }

        bool SkipIf(TokenType type, const std::string &value = {});

        Token Skip();
        Token Expect(TokenType type, std::string value = {});
        Token ExpectAny(const std::vector<TokenType> &types);
        Token ExpectEnum(const std::vector<const char *> &values);

        template<typename... Args>
        Token ExpectEnum(Args... args)
        {
            return ExpectEnum({args...});
        }

        template<typename... Args>
        Token ExpectAny(Args... args)
        {
            return ExpectAny({args...});
        }

        ResourceLocation ParseResourceLocation();

        TargetAttributePtr ParseEnumAttribute(bool invert, const std::vector<const char *> &values);
        TargetAttributePtr ParseFloatAttribute(bool invert);
        TargetAttributePtr ParseFloatRangeAttribute(bool invert);
        TargetAttributePtr ParseIntegerAttribute(bool invert);
        TargetAttributePtr ParseIntegerRangeAttribute(bool invert);
        TargetAttributePtr ParseMapAttribute(bool invert, const Parse &parse);
        TargetAttributePtr ParseNameAttribute(bool invert);
        TargetAttributePtr ParseNBTAttribute(bool invert);
        TargetAttributePtr ParseResourceAttribute(bool invert);
        TargetAttributePtr ParseResourceMapAttribute(bool invert, const Parse &parse);
        TargetAttributePtr ParseStringAttribute(bool invert);
        TargetAttributePtr ParseTagAttribute(bool invert);

        TypeID ParseType();

        StatementPtr ParseTopLevel();
        StatementPtr ParseDefineStatement();
        StatementPtr ParseNamespaceStatement();

        StatementPtr ParseStatement();
        StatementPtr ParseForStatement();
        StatementPtr ParseIfUnlessStatement();
        StatementPtr ParseMultiStatement();
        StatementPtr ParseReturnStatement();
        StatementPtr ParseSwitchStatement();
        StatementPtr ParseThrowStatement();
        StatementPtr ParseTryCatchStatement();

        ExpressionPtr ParseExpression();
        ExpressionPtr ParseResourceExpression();
        ExpressionPtr ParseTargetExpression(bool with_attributes);
        ExpressionPtr ParseArrayExpression();
        ExpressionPtr ParseObjectExpression();
        ExpressionPtr ParseFormatExpression();
        ExpressionPtr ParseIfUnlessExpression();
        ExpressionPtr ParseSwitchExpression();
        ExpressionPtr ParseCommandExpression();
        ExpressionPtr ParseIntegerExpression();
        ExpressionPtr ParseFloatExpression();
        ExpressionPtr ParseRangeExpression();
        ExpressionPtr ParseStringExpression();
        ExpressionPtr ParseSymbolExpression();
        ExpressionPtr ParsePrimaryExpression();
        ExpressionPtr ParseOperandExpression();
        ExpressionPtr ParseBinaryExpression(ExpressionPtr left, unsigned min_pre);

        Context &m_Context;
        std::istream &m_Stream;

        int m_Buf = -1;
        SourceLocation m_Location;
        Token m_Token;

        size_t m_Count = 0;
    };
}
