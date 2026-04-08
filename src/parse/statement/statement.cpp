#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (At(TokenType::Symbol, "break"))
        return ParseBreakStatement();
    if (At(TokenType::Symbol, "continue"))
        return ParseContinueStatement();
    if (At(TokenType::Symbol, "delete"))
        return ParseDeleteStatement();
    if (At(TokenType::Symbol, "for"))
        return ParseForStatement();
    if (At(TokenType::Symbol, "foreach"))
        return ParseForEachStatement();
    if (AtEnum("if", "unless"))
        return ParseIfUnlessStatement();
    if (At(TokenType::Other, "{"))
        return ParseMultiStatement();
    if (At(TokenType::Symbol, "return"))
        return ParseReturnStatement();
    if (At(TokenType::Symbol, "switch"))
        return ParseSwitchStatement();
    if (At(TokenType::Symbol, "throw"))
        return ParseThrowStatement();
    if (At(TokenType::Symbol, "try"))
        return ParseTryCatchStatement();
    if (AtEnum("let", "const", "constexpr"))
        return ParseVariableStatement();
    return ParseExpression();
}
