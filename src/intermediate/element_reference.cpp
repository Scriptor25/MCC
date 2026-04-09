#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ElementReference::SPtr mcc::ElementReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const ValuePtr &base,
        const ValuePtr &index)
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

    auto self = std::make_shared<ElementReference>(where, name, type, base, index);

    self->Self = self;
    self->Base->Use(self);
    self->Index->Use(self);

    return self;
}

mcc::ElementReference::ElementReference(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &base,
        const ValuePtr &index)
    : Value(where,
            name,
            type,
            base->FieldType),
      Base(base),
      Index(index)
{
}

mcc::ElementReference::~ElementReference()
{
    Base->Drop(Self);
    Index->Drop(Self);
}

bool mcc::ElementReference::RequireStack() const
{
    return Base->RequireStack();
}

mcc::Result mcc::ElementReference::GenerateResult() const
{
    auto base  = Base->GenerateResult();
    auto index = Index->GenerateResult();

    Assert(base.Type == ResultType_::Reference, Where, "base must be {}, but is {}", ResultType_::Reference, base.Type);

    switch (index.Type)
    {
    case ResultType_::Value:
        return {
            .Type          = ResultType_::Reference,
            .WithArgument  = base.WithArgument || index.WithArgument,
            .ReferenceType = base.ReferenceType,
            .Target        = base.Target,
            .Path          = std::format("{}[{}]", base.Path, index.Value),
        };

    case ResultType_::Argument:
        return {
            .Type          = ResultType_::Reference,
            .WithArgument  = true,
            .ReferenceType = base.ReferenceType,
            .Target        = base.Target,
            .Path          = std::format("{}[{}]", base.Path, index.Name),
        };

    default:
        Error(Where, "index must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, index.Type);
    }
}
