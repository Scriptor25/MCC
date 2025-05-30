#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseRefExpression()
{
    auto where = Expect(TokenType_Symbol, "ref").Where;

    Expect(TokenType_Operator, "<");
    auto type = ParseType();
    Expect(TokenType_Operator, ">");

    Expect(TokenType_Other, "(");
    auto target_type = ToTargetType(ExpectEnum("block", "entity", "storage").Value);
    Expect(TokenType_Other, ",");

    ExpressionPtr target_position_x, target_position_y, target_position_z, target_name;
    ResourceLocation target_location;

    switch (target_type)
    {
    case ReferenceType_Block:
        target_position_x = ParseExpression();
        Expect(TokenType_Other, ",");
        target_position_y = ParseExpression();
        Expect(TokenType_Other, ",");
        target_position_z = ParseExpression();
        break;
    case ReferenceType_Entity:
        target_name = ParseExpression();
        break;
    case ReferenceType_Storage:
        target_location = ParseResourceLocation();
        break;
    }

    Expect(TokenType_Other, ",");
    auto path = Expect(TokenType_String).Value;
    Expect(TokenType_Other, ")");

    return std::make_unique<RefExpression>(
        where,
        type,
        target_type,
        std::move(target_position_x),
        std::move(target_position_y),
        std::move(target_position_z),
        std::move(target_name),
        target_location,
        path);
}
