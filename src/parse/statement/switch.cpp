#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseSwitchStatement()
{
    auto where = Expect(TokenType::Symbol, "switch").Where;
    Expect(TokenType::Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType::Other, ")");
    Expect(TokenType::Other, "{");
    StatementPtr default_;
    std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases;
    while (!At(TokenType::Other, "}") && !At(TokenType::EoF))
    {
        if (SkipIf(TokenType::Symbol, "default"))
        {
            Assert(!default_, where, "only one default case is permitted");
            if (SkipIf(TokenType::Operator, "->"))
                default_ = ParseStatement();
            else
                default_ = ParseMultiStatement();
            continue;
        }

        Expect(TokenType::Symbol, "case");
        std::vector<ExpressionPtr> conditions;
        do
            conditions.emplace_back(ParseExpression());
        while (SkipIf(TokenType::Other, ","));
        StatementPtr value;
        if (SkipIf(TokenType::Operator, "->"))
            value = ParseStatement();
        else
            value = ParseMultiStatement();
        cases.emplace_back(std::move(conditions), std::move(value));
    }
    Expect(TokenType::Other, "}");

    return std::make_unique<SwitchStatement>(where, std::move(condition), std::move(default_), std::move(cases));
}
