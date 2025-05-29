#include <mcc/value.hpp>
#include <utility>

mcc::ValuePtr mcc::GenericStorageReference::Create(const SourceLocation &where, const TypePtr &type, const ResourceLocation &location, const std::string &path)
{
    return std::make_shared<GenericStorageReference>(where, type, location, path);
}

mcc::GenericStorageReference::GenericStorageReference(const SourceLocation &where, const TypePtr &type, ResourceLocation location, std::string path)
    : Value(where, type, true), Location(std::move(location)), Path(std::move(path))
{
}

bool mcc::GenericStorageReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericStorageReference::GenerateResult() const
{
    return {
        .Type          = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target        = Location.String(),
        .Path          = Path,
    };
}
