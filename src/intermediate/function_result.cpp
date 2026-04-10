#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::FunctionResult::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const FunctionPtr &parent)
{
    auto self  = std::make_shared<FunctionResult>(where, name, type, parent);
    self->Self = self;
    return self;
}

mcc::FunctionResult::FunctionResult(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        FunctionPtr parent)
    : Value(where,
            name,
            type,
            FieldType_::ImmutableReference),
      Parent(std::move(parent))
{
}

bool mcc::FunctionResult::RequireStack() const
{
    return false;
}

mcc::Result mcc::FunctionResult::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Parent->Mangle().String(),
        .Path          = "result",
    };
}
