#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::Type::Type(TypeContext &context)
    : Context(context)
{
}

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}

mcc::VoidType::VoidType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::VoidType::String() const
{
    return "void";
}

std::ostream &mcc::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

mcc::ConstantPtr mcc::VoidType::GetNull(const SourceLocation &where) const
{
    return nullptr;
}

mcc::NullType::NullType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::NullType::String() const
{
    return "null";
}

std::ostream &mcc::NullType::Print(std::ostream &stream) const
{
    return stream << "null";
}

mcc::ConstantPtr mcc::NullType::GetNull(const SourceLocation &where) const
{
    return ConstantNull::Create(where, Context);
}

mcc::BooleanType::BooleanType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::BooleanType::String() const
{
    return "boolean";
}

std::ostream &mcc::BooleanType::Print(std::ostream &stream) const
{
    return stream << "boolean";
}

mcc::ConstantPtr mcc::BooleanType::GetNull(const SourceLocation &where) const
{
    return ConstantBoolean::Create(where, Context, false);
}

mcc::NumberType::NumberType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::NumberType::String() const
{
    return "number";
}

std::ostream &mcc::NumberType::Print(std::ostream &stream) const
{
    return stream << "number";
}

mcc::ConstantPtr mcc::NumberType::GetNull(const SourceLocation &where) const
{
    return ConstantNumber::Create(where, Context, 0);
}

mcc::StringType::StringType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::StringType::String() const
{
    return "string";
}

std::ostream &mcc::StringType::Print(std::ostream &stream) const
{
    return stream << "string";
}

mcc::ConstantPtr mcc::StringType::GetNull(const SourceLocation &where) const
{
    return ConstantString::Create(where, Context, "");
}

mcc::ArrayType::ArrayType(TypeContext &context, TypePtr elements)
    : Type(context),
      Elements(std::move(elements))
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

mcc::ConstantPtr mcc::ArrayType::GetNull(const SourceLocation &where) const
{
    return ConstantArray::Create(where, Self.lock(), {}, false);
}

mcc::StructType::StructType(TypeContext &context, const std::map<std::string, TypePtr> &elements)
    : Type(context),
      Elements(elements)
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

mcc::ConstantPtr mcc::StructType::GetNull(const SourceLocation &where) const
{
    std::map<std::string, ConstantPtr> values;
    for (auto &[key_, element_]: Elements)
    {
        const auto value = element_->GetNull(where);
        if (!value)
        {
            return nullptr;
        }
        values[key_] = value;
    }
    return ConstantObject::Create(where, Context, values);
}

mcc::TupleType::TupleType(TypeContext &context, const std::vector<TypePtr> &elements)
    : Type(context),
      Elements(elements)
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

mcc::ConstantPtr mcc::TupleType::GetNull(const SourceLocation &where) const
{
    std::vector<ConstantPtr> values;
    for (auto &element: Elements)
    {
        auto value = element->GetNull(where);
        if (!value)
        {
            return nullptr;
        }
        values.emplace_back(value);
    }
    return ConstantArray::Create(where, Self.lock(), values, false);
}

mcc::UnionType::UnionType(TypeContext &context, const std::set<TypePtr> &elements)
    : Type(context),
      Elements(elements)
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

mcc::ConstantPtr mcc::UnionType::GetNull(const SourceLocation &where) const
{
    return nullptr;
}

mcc::FunctionType::FunctionType(
    TypeContext &context,
    const std::vector<TypePtr> &parameters,
    const TypePtr &result,
    const bool throws)
    : Type(context),
      Parameters(parameters),
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

mcc::ConstantPtr mcc::FunctionType::GetNull(const SourceLocation &where) const
{
    return nullptr;
}
