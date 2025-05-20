#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (At(TokenType_Symbol, "break"))
        return ParseBreakStatement();
    if (At(TokenType_Symbol, "continue"))
        return ParseContinueStatement();
    if (At(TokenType_Symbol, "for"))
        return ParseForStatement();
    if (AtEnum("if", "unless"))
        return ParseIfUnlessStatement();
    if (At(TokenType_Other, "{"))
        return ParseMultiStatement();
    if (At(TokenType_Symbol, "return"))
        return ParseReturnStatement();
    if (At(TokenType_Symbol, "switch"))
        return ParseSwitchStatement();
    if (At(TokenType_Symbol, "throw"))
        return ParseThrowStatement();
    if (At(TokenType_Symbol, "try"))
        return ParseTryCatchStatement();
    if (AtEnum("const", "let"))
        return ParseVariableStatement();

    return ParseExpression();
}
