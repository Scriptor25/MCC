#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>
#include <utility>

mcc::ValuePtr mcc::ElementReference::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ValuePtr &array,
    const IndexT index)
{
    const auto array_type = std::dynamic_pointer_cast<ArrayType>(array->Type);
    Assert(!!array_type, where, "array must be of type array");
    auto type = array_type->Elements;
    return std::make_shared<ElementReference>(where, context, type, array, index);
}

mcc::ElementReference::ElementReference(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    ValuePtr array,
    const IndexT index)
    : Value(where, context, type),
      Array(std::move(array)),
      Index(index)
{
    Array->Use();
}

mcc::ElementReference::~ElementReference()
{
    Array->Drop();
}

bool mcc::ElementReference::RequireStack() const
{
    return Array->RequireStack();
}

mcc::Result mcc::ElementReference::GenerateResult(const bool stringify) const
{
    auto array = Array->GenerateResult(stringify);
    Assert(array.Type == ResultType_Storage, Where, "array must be {}, but is {}", ResultType_Storage, array.Type);

    return {
        .Type = ResultType_Storage,
        .Location = array.Location,
        .Path = std::format("{}[{}]", array.Path, Index),
    };
}
