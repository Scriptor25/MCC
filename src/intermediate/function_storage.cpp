#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::FunctionStorageReference::SPtr mcc::FunctionStorageReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const FunctionPtr &parent,
        const std::string &path,
        bool is_mutable)
{
    auto self  = std::make_shared<FunctionStorageReference>(where, name, type, parent, path, is_mutable);
    self->Self = self;
    return self;
}

mcc::FunctionStorageReference::FunctionStorageReference(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        FunctionPtr parent,
        std::string path,
        const bool is_mutable)
    : Value(where,
            name,
            type,
            is_mutable ? FieldType_::MutableReference : FieldType_::ImmutableReference),
      Parent(std::move(parent)),
      Path(std::move(path))
{
}

mcc::FunctionStorageReference::~FunctionStorageReference()
{
}

bool mcc::FunctionStorageReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::FunctionStorageReference::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .WithArgument  = false,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Parent->Mangle().String(),
        .Path          = Path,
    };
}
