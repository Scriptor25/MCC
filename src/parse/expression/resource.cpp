#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseResourceExpression()
{
    auto where = m_Token.Where;
    auto location = ParseResourceLocation();

    return std::make_unique<ResourceExpression>(where, location);
}
