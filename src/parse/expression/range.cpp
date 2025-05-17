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
            ConstantFloat::Create(token.Where, token.Integer),
            token.Value);
    }

    if (At(TokenType_Float))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(token.Where, token.Float),
            token.Value);
    }

    auto token = Expect(TokenType_Range);
    auto [beg_, end_] = token.Range;
    return std::make_unique<ConstantExpression>(
        token.Where,
        beg_.has_value() && end_.has_value()
            ? ConstantFloatRange::Create(token.Where, *beg_, *end_)
            : beg_.has_value()
                  ? ConstantFloatRange::CreateMin(token.Where, *beg_)
                  : ConstantFloatRange::CreateMax(token.Where, *end_),
        token.Value);
}
