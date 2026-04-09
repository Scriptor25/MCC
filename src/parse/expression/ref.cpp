#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseRefExpression()
{
    auto where = Expect(TokenType::Symbol, "ref").Where;

    Expect(TokenType::Operator, "<");
    auto type = ParseType();
    Expect(TokenType::Operator, ">");

    Expect(TokenType::Other, "(");
    auto target_type = *ToReferenceType(ExpectEnum("block", "entity", "storage").Value);
    Expect(TokenType::Other, ",");

    ExpressionPtr target_position_x, target_position_y, target_position_z, target_name, target_location;

    switch (target_type)
    {
    case ReferenceType_::Block:
        target_position_x = ParseExpression();
        Expect(TokenType::Other, ",");
        target_position_y = ParseExpression();
        Expect(TokenType::Other, ",");
        target_position_z = ParseExpression();
        break;
    case ReferenceType_::Entity:
        target_name = ParseExpression();
        break;
    case ReferenceType_::Storage:
        target_location = ParseExpression();
        break;
    }

    Expect(TokenType::Other, ",");
    auto path = ParseExpression();
    Expect(TokenType::Other, ")");

    return std::make_unique<RefExpression>(
            where,
            type,
            target_type,
            std::move(target_position_x),
            std::move(target_position_y),
            std::move(target_position_z),
            std::move(target_name),
            std::move(target_location),
            std::move(path));
}
