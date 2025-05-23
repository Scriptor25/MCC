#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSwitchExpression()
{
    auto where = Expect(TokenType_Symbol, "switch").Where;
    Expect(TokenType_Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType_Other, ")");
    Expect(TokenType_Other, "{");
    ExpressionPtr default_;
    std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases;
    while (!At(TokenType_Other, "}") && !At(TokenType_EOF))
    {
        if (SkipIf(TokenType_Symbol, "default"))
        {
            Expect(TokenType_Operator, "->");
            default_ = ParseExpression();
            continue;
        }

        Expect(TokenType_Symbol, "case");
        std::vector<ExpressionPtr> conditions;
        do
            conditions.emplace_back(ParseExpression());
        while (SkipIf(TokenType_Other, ","));
        Expect(TokenType_Operator, "->");
        auto value = ParseExpression();
        cases.emplace_back(std::move(conditions), std::move(value));
    }
    Expect(TokenType_Other, "}");

    Assert(!!default_, where, "switch expression must specify exactly one default case");

    return std::make_unique<SwitchExpression>(
        where,
        std::move(condition),
        std::move(default_),
        std::move(cases));
}
