#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::TypeID mcc::Parser::ParseType()
{
    auto name = Expect(TokenType_Symbol);

    if (name.Value == "boolean")
        return TypeID_Boolean;
    if (name.Value == "number")
        return TypeID_Number;
    if (name.Value == "string")
        return TypeID_String;
    if (name.Value == "array")
        return TypeID_Array;
    if (name.Value == "object")
        return TypeID_Object;

    Error(name.Where, "undefined type {}", name.Value);
}
