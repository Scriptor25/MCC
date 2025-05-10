#pragma once

#include <functional>
#include <memory>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>

namespace mcc
{
    using Parse = std::function<TargetAttributePtr(Parser &self, bool invert)>;

    class Parser
    {
        friend FixedNode;
        friend SwitchNode;
        friend ValueNode;
        friend GreedyNode;

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

        TargetAttributePtr ParseIntegerAttribute(bool invert);
        TargetAttributePtr ParseFloatAttribute(bool invert);
        TargetAttributePtr ParseIntegerRangeAttribute(bool invert);
        TargetAttributePtr ParseFloatRangeAttribute(bool invert);
        TargetAttributePtr ParseStringAttribute(bool invert);
        TargetAttributePtr ParseEnumAttribute(bool invert, const std::vector<const char *> &values);
        TargetAttributePtr ParseNameAttribute(bool invert);
        TargetAttributePtr ParseMapAttribute(bool invert, const Parse &parse);
        TargetAttributePtr ParseResourceMapAttribute(bool invert, const Parse &parse);
        TargetAttributePtr ParseNBTAttribute(bool invert);
        TargetAttributePtr ParseResourceAttribute(bool invert);
        TargetAttributePtr ParseTagAttribute(bool invert);

        StatementPtr ParseTopLevel();
        StatementPtr ParseNamespaceStatement();
        StatementPtr ParseDefineStatement();

        StatementPtr ParseStatement();
        StatementPtr ParseMultiStatement();
        StatementPtr ParseIfUnlessStatement();
        StatementPtr ParseSwitchStatement();
        StatementPtr ParseForStatement();
        StatementPtr ParseReturnStatement();

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
