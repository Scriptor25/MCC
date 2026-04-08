#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseOperandExpression()
{
    auto operand = ParsePrimaryExpression();

    while (true)
    {
        auto where = m_Token.Where;

        if (SkipIf(TokenType::Other, "("))
        {
            std::vector<ExpressionPtr> arguments;
            while (!At(TokenType::Other, ")") && !At(TokenType::EoF))
            {
                arguments.push_back(ParseExpression());

                if (!At(TokenType::Other, ")"))
                    Expect(TokenType::Other, ",");
            }
            Expect(TokenType::Other, ")");

            operand = std::make_unique<CallExpression>(where, std::move(operand), std::move(arguments));
            continue;
        }

        if (SkipIf(TokenType::Other, "["))
        {
            auto index = ParseExpression();
            Expect(TokenType::Other, "]");

            operand = std::make_unique<SubscriptExpression>(where, std::move(operand), std::move(index));
            continue;
        }

        if (SkipIf(TokenType::Other, "."))
        {
            auto member = Expect(TokenType::Symbol).Value;

            operand = std::make_unique<MemberExpression>(where, std::move(operand), member);
            continue;
        }

        return operand;
    }
}
