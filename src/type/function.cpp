#include <utility>
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
    return nullptr;
}

bool mcc::FunctionType::IsFunction() const
{
    return true;
}
