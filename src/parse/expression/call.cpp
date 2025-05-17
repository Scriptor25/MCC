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

            operand = std::make_unique<CallExpression>(std::move(where), std::move(operand), std::move(arguments));
            continue;
        }

        if (SkipIf(TokenType_Other, "["))
        {
            auto index = ParseExpression();
            Expect(TokenType_Other, "]");

            operand = std::make_unique<SubscriptExpression>(std::move(where), std::move(operand), std::move(index));
            continue;
        }

        return operand;
    }
}
