#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseRangeExpression(const bool negative)
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(negative ? -token.Integer : token.Integer),
            token.Value);
    }

    if (At(TokenType_Float))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(negative ? -token.Float : token.Float),
            token.Value);
    }

    auto token = Expect(TokenType_Range);
    auto [beg_, end_] = token.Range;
    return std::make_unique<ConstantExpression>(
        token.Where,
        beg_.has_value() && end_.has_value()
            ? ConstantFloatRange::Create(negative ? -*beg_ : *beg_, negative ? -*end_ : *end_)
            : beg_.has_value()
                  ? ConstantFloatRange::CreateMin(negative ? -*beg_ : *beg_)
                  : ConstantFloatRange::CreateMax(negative ? -*end_ : *end_),
        token.Value);
}
