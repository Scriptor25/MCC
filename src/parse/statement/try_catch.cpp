#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseTryCatchStatement()
{
    auto where = Expect(TokenType::Symbol, "try").Where;

    auto try_ = ParseStatement();

    StatementPtr catch_;
    std::string variable;
    TypePtr error_type;
    if (SkipIf(TokenType::Symbol, "catch"))
    {
        if (SkipIf(TokenType::Other, "("))
        {
            variable = Expect(TokenType::Symbol).Value;
            Expect(TokenType::Other, ":");
            error_type = ParseType();
            Expect(TokenType::Other, ")");
        }

        catch_ = ParseStatement();
    }

    return std::make_unique<TryCatchStatement>(where, std::move(try_), std::move(catch_), variable, error_type);
}
