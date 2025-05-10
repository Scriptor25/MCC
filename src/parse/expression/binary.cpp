#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseBinaryExpression(ExpressionPtr left, const unsigned min_pre)
{
    static const std::map<std::string, unsigned> pres
    {
        {"=", 0},
        {"+=", 0},
        {"-=", 0},
        {"*=", 0},
        {"/=", 0},
        {"%=", 0},
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
        auto operator_ = Skip();
        auto right = ParseOperandExpression();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > pre || (!get_pre() && !pre)))
            right = ParseBinaryExpression(std::move(right), pre + (get_pre() > pre ? 1 : 0));
        left = std::make_unique<BinaryExpression>(operator_.Where, operator_.Value, std::move(left), std::move(right));
    }

    if (const auto binary = dynamic_cast<BinaryExpression *>(left.get()))
        if (auto merged = binary->Merge())
            left = std::move(merged);

    return left;
}
