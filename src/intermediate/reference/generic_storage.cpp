#include <utility>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericStorageReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const std::string &path,
    const bool is_mutable)
{
    return std::make_shared<GenericStorageReference>(where, type, location, path, is_mutable);
}

mcc::GenericStorageReference::GenericStorageReference(
    const SourceLocation &where,
    const TypePtr &type,
    ResourceLocation location,
    std::string path,
    const bool is_mutable)
    : Value(where, type, is_mutable),
      Location(std::move(location)),
      Path(std::move(path))
{
}

bool mcc::GenericStorageReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericStorageReference::GenerateResult() const
{
    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = Location.String(),
        .Path = Path,
    };
}
