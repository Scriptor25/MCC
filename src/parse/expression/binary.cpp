#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseBinaryExpression(
    ExpressionPtr left,
    ExpressionPtr (Parser::*next)(),
    const unsigned min_pre)
{
    static const std::map<std::string, unsigned> pres
    {
        {"=", 0},
        {"==", 1},
        {"!=", 1},
        {"<=", 1},
        {">=", 1},
        {"<", 1},
        {">", 1},
        {"+", 2},
        {"-", 2},
        {"*", 3},
        {"/", 3},
        {"%", 3},
    };

    auto has_pre = [this]
    {
        return pres.contains(m_Token.Value);
    };

    auto get_pre = [this]
    {
        return pres.at(m_Token.Value);
    };

    while (At(TokenType_Operator) && has_pre() && get_pre() >= min_pre)
    {
        const auto pre = get_pre();
        auto operator_ = Skip().Value;
        auto right = (this->*next)();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > pre || (!get_pre() && !pre)))
            right = ParseBinaryExpression(std::move(right), next, pre + (get_pre() > pre ? 1 : 0));
        left = std::make_unique<BinaryExpression>(operator_, std::move(left), std::move(right));
    }

    return left;
}
