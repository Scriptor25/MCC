#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseOperandExpression()
{
    auto operand = ParsePrimaryExpression();

    while (true)
    {
        auto where = m_Token.Where;

        if (SkipIf(TokenType_Other, "("))
        {
            std::vector<ExpressionPtr> arguments;
            while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
            {
                arguments.emplace_back(ParseExpression());

                if (!At(TokenType_Other, ")"))
                    Expect(TokenType_Other, ",");
            }
            Expect(TokenType_Other, ")");

            operand = std::make_unique<CallExpression>(where, std::move(operand), std::move(arguments));
            continue;
        }

        if (SkipIf(TokenType_Other, "["))
        {
            auto index = ParseExpression();
            Expect(TokenType_Other, "]");

            operand = std::make_unique<SubscriptExpression>(where, std::move(operand), std::move(index));
            continue;
        }

        if (SkipIf(TokenType_Other, "."))
        {
            auto member = Expect(TokenType_Symbol).Value;

            operand = std::make_unique<MemberExpression>(where, std::move(operand), member);
            continue;
        }

        return operand;
    }
}
