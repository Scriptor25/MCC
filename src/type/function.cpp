#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::FunctionType::FunctionType(
    TypeContext &context,
    const std::vector<TypePtr> &parameters,
    TypePtr result,
    const bool throws)
    : Type(context),
      Parameters(parameters),
      Result(std::move(result)),
      Throws(throws)
{
}

std::string mcc::FunctionType::String() const
{
    std::string result;
    result += '$';
    if (Throws)
        result += '!';
    result += '(';

    auto first = true;
    for (auto &parameter : Parameters)
    {
        if (first)
            first = false;
        else
            result += ", ";
        result += parameter->String();
    }

    result += ") => " + Result->String();
    return result;
}

std::ostream &mcc::FunctionType::Print(std::ostream &stream) const
{
    stream << '$';
    if (Throws)
        stream << '!';
    stream << '(';

    auto first = true;
    for (auto &parameter : Parameters)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        parameter->Print(stream);
    }

    return Result->Print(stream << ") => ");
}

mcc::ConstantPtr mcc::FunctionType::GetNull(const SourceLocation &where) const
{
    return ConstantResource::Create(where, Self.lock(), {});
}

bool mcc::FunctionType::HasSpecial(const TypePtr &other) const
{
    if (other->IsAny() || !other->IsFunction())
        return false;

    const auto other_function = std::dynamic_pointer_cast<FunctionType>(other);

    if (other_function->Throws && !Throws)
        return false;

    if (other_function->Parameters.size() != Parameters.size())
        return false;

    for (unsigned i = 0; i < Parameters.size(); ++i)
        if (other_function->Parameters[i] != Parameters[i])
            return false;

    return SameOrSpecial(other_function->Result, Result);
}

bool mcc::FunctionType::IsFunction() const
{
    return true;
}
