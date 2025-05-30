#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::ElementReference::Create(const SourceLocation &where, const ValuePtr &base, const IndexT index)
{
    Assert(!!base, where, "base must not be null");

    const auto base_type = base->Type;
    const auto is_array  = base_type->IsArray();
    const auto is_tuple  = base_type->IsTuple();

    Assert(is_array || is_tuple, where, "base must be of type array or tuple");

    auto type = is_array
                    ? (std::dynamic_pointer_cast<ArrayType>(base_type)->Elements)
                    : (std::dynamic_pointer_cast<TupleType>(base_type)->Elements[index]);

    return std::make_shared<ElementReference>(where, type, base, index);
}

mcc::ElementReference::ElementReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &base,
    const IndexT index)
    : Value(where, type, base->IsMutable),
      Base(base),
      Index(index)
{
    Base->Use();
}

mcc::ElementReference::~ElementReference()
{
    Base->Drop();
}

bool mcc::ElementReference::RequireStack() const
{
    return Base->RequireStack();
}

mcc::Result mcc::ElementReference::GenerateResult() const
{
    auto base = Base->GenerateResult();
    Assert(base.Type == ResultType_Reference, Where, "array must be {}, but is {}", ResultType_Reference, base.Type);

    return {
        .Type = ResultType_Reference,
        .ReferenceType = base.ReferenceType,
        .Target = base.Target,
        .Path = std::format("{}[{}]", base.Path, Index),
    };
}
