#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSwitchExpression()
{
    auto where = Expect(TokenType::Symbol, "switch").Where;
    Expect(TokenType::Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType::Other, ")");
    Expect(TokenType::Other, "{");
    ExpressionPtr default_;
    std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases;
    while (!At(TokenType::Other, "}") && !At(TokenType::EoF))
    {
        if (SkipIf(TokenType::Symbol, "default"))
        {
            Expect(TokenType::Operator, "->");
            default_ = ParseExpression();
            continue;
        }

        Expect(TokenType::Symbol, "case");
        std::vector<ExpressionPtr> conditions;
        do
            conditions.emplace_back(ParseExpression());
        while (SkipIf(TokenType::Other, ","));
        Expect(TokenType::Operator, "->");
        auto value = ParseExpression();
        cases.emplace_back(std::move(conditions), std::move(value));
    }
    Expect(TokenType::Other, "}");

    Assert(!!default_, where, "switch expression must specify exactly one default case");

    return std::make_unique<SwitchExpression>(where, std::move(condition), std::move(default_), std::move(cases));
}
