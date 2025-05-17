#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseTryCatchStatement()
{
    auto where = Expect(TokenType_Symbol, "try").Where;

    auto try_ = ParseStatement();

    StatementPtr catch_;
    std::string variable;
    if (SkipIf(TokenType_Symbol, "catch"))
    {
        if (SkipIf(TokenType_Other, "("))
        {
            variable = Expect(TokenType_Symbol).Value;
            Expect(TokenType_Other, ")");
        }

        catch_ = ParseStatement();
    }

    return std::make_unique<TryCatchStatement>(
        where,
        std::move(try_),
        std::move(catch_),
        variable);
}
