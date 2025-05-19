#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseRangeExpression()
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(token.Where, m_Context, token.Integer),
            token.Value);
    }

    if (At(TokenType_Float))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(token.Where, m_Context, token.Float),
            token.Value);
    }

    auto token = Expect(TokenType_Range);
    auto [beg_, end_] = token.Range;
    return std::make_unique<ConstantExpression>(
        token.Where,
        beg_.has_value() && end_.has_value()
            ? ConstantFloatRange::Create(token.Where, m_Context, *beg_, *end_)
            : beg_.has_value()
                  ? ConstantFloatRange::CreateMin(token.Where, m_Context, *beg_)
                  : ConstantFloatRange::CreateMax(token.Where, m_Context, *end_),
        token.Value);
}
