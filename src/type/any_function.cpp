#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::AnyFunctionType::AnyFunctionType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::AnyFunctionType::String() const
{
    return "function";
}

std::ostream &mcc::AnyFunctionType::Print(std::ostream &stream) const
{
    return stream << "function";
}

mcc::ConstantPtr mcc::AnyFunctionType::GetNull(const SourceLocation &where) const
{
    return ConstantResource::Create(where, Self.lock(), {});
}

bool mcc::AnyFunctionType::HasSpecial(const TypePtr &other) const
{
    return other->IsFunction();
}

bool mcc::AnyFunctionType::IsAny() const
{
    return true;
}

bool mcc::AnyFunctionType::IsFunction() const
{
    return true;
}
