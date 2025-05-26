#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::Type::Type(TypeContext &context)
    : Context(context)
{
}

bool mcc::Type::IsVoid() const
{
    return false;
}

bool mcc::Type::IsNull() const
{
    return false;
}

bool mcc::Type::IsBoolean() const
{
    return false;
}

bool mcc::Type::IsNumber() const
{
    return false;
}

bool mcc::Type::IsString() const
{
    return false;
}

bool mcc::Type::IsArray() const
{
    return false;
}

bool mcc::Type::IsObject() const
{
    return false;
}

bool mcc::Type::IsTuple() const
{
    return false;
}

bool mcc::Type::IsUnion() const
{
    return false;
}

bool mcc::Type::IsFunction() const
{
    return false;
}

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}
