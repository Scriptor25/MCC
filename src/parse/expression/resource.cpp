#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseResourceExpression()
{
    auto where = m_Token.Where;
    auto location = ParseResourceLocation();

    return std::make_unique<ResourceExpression>(std::move(where), std::move(location), nullptr, nullptr);
}
