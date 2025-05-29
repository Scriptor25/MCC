#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseSwitchStatement()
{
    auto where = Expect(TokenType_Symbol, "switch").Where;
    Expect(TokenType_Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType_Other, ")");
    Expect(TokenType_Other, "{");
    StatementPtr default_;
    std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases;
    while (!At(TokenType_Other, "}") && !At(TokenType_EOF))
    {
        if (SkipIf(TokenType_Symbol, "default"))
        {
            Assert(!default_, where, "only one default case is permitted");
            if (SkipIf(TokenType_Operator, "->"))
                default_ = ParseStatement();
            else
                default_ = ParseMultiStatement();
            continue;
        }

        Expect(TokenType_Symbol, "case");
        std::vector<ExpressionPtr> conditions;
        do
            conditions.emplace_back(ParseExpression());
        while (SkipIf(TokenType_Other, ","));
        StatementPtr value;
        if (SkipIf(TokenType_Operator, "->"))
            value = ParseStatement();
        else
            value = ParseMultiStatement();
        cases.emplace_back(std::move(conditions), std::move(value));
    }
    Expect(TokenType_Other, "}");

    return std::make_unique<SwitchStatement>(where, std::move(condition), std::move(default_), std::move(cases));
}
