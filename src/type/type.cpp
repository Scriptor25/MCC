#include <mcc/type.hpp>
#include <utility>

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}

std::string mcc::VoidType::String() const
{
    return "void";
}

std::ostream &mcc::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

std::string mcc::NullType::String() const
{
    return "null";
}

std::ostream &mcc::NullType::Print(std::ostream &stream) const
{
    return stream << "null";
}

std::string mcc::BooleanType::String() const
{
    return "boolean";
}

std::ostream &mcc::BooleanType::Print(std::ostream &stream) const
{
    return stream << "boolean";
}

std::string mcc::NumberType::String() const
{
    return "number";
}

std::ostream &mcc::NumberType::Print(std::ostream &stream) const
{
    return stream << "number";
}

std::string mcc::StringType::String() const
{
    return "string";
}

std::ostream &mcc::StringType::Print(std::ostream &stream) const
{
    return stream << "string";
}

mcc::ArrayType::ArrayType(TypePtr elements)
    : Elements(std::move(elements))
{
}

std::string mcc::ArrayType::String() const
{
    return Elements->String() + "[]";
}

std::ostream &mcc::ArrayType::Print(std::ostream &stream) const
{
    return Elements->Print(stream) << "[]";
}

mcc::StructType::StructType(const std::map<std::string, TypePtr> &elements)
    : Elements(elements)
{
}

std::string mcc::StructType::String() const
{
    std::string result;
    result += "{ ";

    auto first = true;
    for (auto &[key, element]: Elements)
    {
        if (first)
            first = false;
        else
            result += ", ";
        result += key + ": " + element->String();
    }

    result += " }";
    return result;
}

std::ostream &mcc::StructType::Print(std::ostream &stream) const
{
    stream << "{ ";

    auto first = true;
    for (auto &[key, element]: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        element->Print(stream << key << ": ");
    }

    return stream << " }";
}

mcc::TupleType::TupleType(const std::vector<TypePtr> &elements)
    : Elements(elements)
{
}

std::string mcc::TupleType::String() const
{
    std::string result;
    result += "[ ";

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            result += ", ";
        result += element->String();
    }

    result += " ]";
    return result;
}

std::ostream &mcc::TupleType::Print(std::ostream &stream) const
{
    stream << "[ ";

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        element->Print(stream);
    }

    return stream << " ]";
}

mcc::UnionType::UnionType(const std::set<TypePtr> &elements)
    : Elements(elements)
{
}

std::string mcc::UnionType::String() const
{
    std::string result;
    result += '(';

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            result += " | ";
        result += element->String();
    }

    result += ')';
    return result;
}

std::ostream &mcc::UnionType::Print(std::ostream &stream) const
{
    stream << '(';

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            stream << " | ";
        element->Print(stream);
    }

    return stream << ')';
}

mcc::FunctionType::FunctionType(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws)
    : Parameters(parameters),
      Result(result),
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
    for (auto &parameter: Parameters)
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
    for (auto &parameter: Parameters)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        parameter->Print(stream);
    }

    return Result->Print(stream << ") => ");
}
