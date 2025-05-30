#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::ElementReference::Create(const SourceLocation &where, const ValuePtr &base, const ValuePtr &index)
{
    Assert(!!base, where, "base must not be null");
    Assert(!!index, where, "index must not be null");

    const auto base_type = base->Type;
    const auto is_array  = base_type->IsArray();
    const auto is_tuple  = base_type->IsTuple();

    TypePtr type;
    if (is_array)
        type = std::dynamic_pointer_cast<ArrayType>(base_type)->Elements;
    else if (is_tuple)
    {
        const auto constant_index = std::dynamic_pointer_cast<ConstantNumber>(index);
        Assert(!!constant_index, where, "index must be a constant number");
        type = std::dynamic_pointer_cast<TupleType>(base_type)->Elements[constant_index->Value];
    }
    else
        Error(where, "base must be of type array or tuple");

    return std::make_shared<ElementReference>(where, type, base, index);
}

mcc::ElementReference::ElementReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &base,
    const ValuePtr &index)
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
    auto base  = Base->GenerateResult();
    auto index = Index->GenerateResult();

    Assert(base.Type == ResultType_Reference, Where, "base must be {}, but is {}", ResultType_Reference, base.Type);

    switch (index.Type)
    {
    case ResultType_Value:
        return {
            .Type = ResultType_Reference,
            .WithArgument = base.WithArgument || index.WithArgument,
            .ReferenceType = base.ReferenceType,
            .Target = base.Target,
            .Path = std::format("{}[{}]", base.Path, index.Value),
        };

    case ResultType_Argument:
        return {
            .Type = ResultType_Reference,
            .WithArgument = true,
            .ReferenceType = base.ReferenceType,
            .Target = base.Target,
            .Path = std::format("{}[{}]", base.Path, index.Name),
        };

    default:
        Error(Where, "index must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, index.Type);
    }
}
