#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const FunctionPtr &parent)
{
    auto self  = std::make_shared<BranchResult>(where, name, type, parent);
    self->Self = self;
    return self;
}

mcc::BranchResult::BranchResult(
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

bool mcc::BranchResult::RequireStack() const
{
    return true;
}

mcc::Result mcc::BranchResult::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Parent->Mangle().String(),
        .Path          = std::format("stack[0].x{}", StackId),
    };
}
