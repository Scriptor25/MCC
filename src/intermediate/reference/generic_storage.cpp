#include <utility>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericStorageReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &location,
    const ValuePtr &path,
    bool is_mutable)
{
    return std::make_shared<GenericStorageReference>(where, type, location, path, is_mutable);
}

mcc::ValuePtr mcc::GenericStorageReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const std::string &path,
    const bool is_mutable)
{
    return std::make_shared<GenericStorageReference>(
        where,
        type,
        ConstantResource::Create(where, type->Context.GetVoid(), location),
        ConstantString::Create(where, type->Context, path),
        is_mutable);
}

mcc::GenericStorageReference::GenericStorageReference(
    const SourceLocation &where,
    const TypePtr &type,
    ValuePtr location,
    ValuePtr path,
    const bool is_mutable)
    : Value(where, type, is_mutable ? FieldType_MutableReference : FieldType_ImmutableReference),
      Location(std::move(location)),
      Path(std::move(path))
{
    Location->Use();
    Path->Use();
}

mcc::GenericStorageReference::~GenericStorageReference()
{
    Location->Drop();
    Path->Drop();
}

bool mcc::GenericStorageReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericStorageReference::GenerateResult() const
{
    auto location = Location->GenerateResultUnwrap();
    auto path = Path->GenerateResultUnwrap();

    auto with_argument = location.WithArgument || path.WithArgument;
    std::string location_value, path_value;

    switch (location.Type)
    {
    case ResultType_Value:
        location_value = location.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        location_value = location.Name;
        break;

    default:
        Error(Where, "location must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, location.Type);
    }

    switch (path.Type)
    {
    case ResultType_Value:
        path_value = path.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        path_value = path.Name;
        break;

    default:
        Error(Where, "path must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, path.Type);
    }

    return {
        .Type = ResultType_Reference,
        .WithArgument = with_argument,
        .ReferenceType = ReferenceType_Storage,
        .Target = location_value,
        .Path = path_value,
    };
}
