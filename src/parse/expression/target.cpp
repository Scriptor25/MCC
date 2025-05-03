#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseTargetExpression()
{
    auto selector = ToTargetSelector(Expect(TokenType_Target).Value);

    std::map<std::string, std::vector<ExpressionPtr>> arguments;
    if (SkipIf(TokenType_Other, "["))
    {
        while (!SkipIf(TokenType_Other, "]"))
        {
            auto key = Expect(TokenType_Symbol).Value;
            Expect(TokenType_Operator, "=");
            auto value = ParseExpression();
            arguments[key].emplace_back(std::move(value));

            if (!At(TokenType_Other, "]"))
                Expect(TokenType_Other, ",");
        }
    }

    return std::make_unique<TargetExpression>(selector, std::move(arguments));
}
