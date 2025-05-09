#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseRangeExpression()
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            std::move(token.Where),
            ConstantFloat::Create(token.Integer),
            std::move(token.Value));
    }

    if (At(TokenType_Float))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            std::move(token.Where),
            ConstantFloat::Create(token.Float),
            std::move(token.Value));
    }

    auto token = Expect(TokenType_Range);
    auto [beg_, end_] = token.Range;
    return std::make_unique<ConstantExpression>(
        std::move(token.Where),
        beg_.has_value() && end_.has_value()
            ? ConstantFloatRange::Create(*beg_, *end_)
            : beg_.has_value()
                  ? ConstantFloatRange::CreateMin(*beg_)
                  : ConstantFloatRange::CreateMax(*end_),
        std::move(token.Value));
}
