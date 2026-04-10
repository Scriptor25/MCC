#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericStorageReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &location,
        const ValuePtr &path,
        bool is_mutable)
{
    auto self = std::make_shared<GenericStorageReference>(where, name, type, location, path, is_mutable);

    self->Self = self;
    self->Location->Use(self);
    self->Path->Use(self);

    return self;
}

mcc::ValuePtr mcc::GenericStorageReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ResourceLocation &location,
        const std::string &path,
        const bool is_mutable)
{
    const auto location_ = ConstantResource::Create(where, type->Types.GetVoid(), location);
    const auto path_     = ConstantString::Create(where, type->Types, path);

    return Create(where, name, type, location_, path_, is_mutable);
}

mcc::GenericStorageReference::GenericStorageReference(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        ValuePtr location,
        ValuePtr path,
        const bool is_mutable)
    : Value(where,
            name,
            type,
            is_mutable ? FieldType_::MutableReference : FieldType_::ImmutableReference),
      Location(std::move(location)),
      Path(std::move(path))
{
}

mcc::GenericStorageReference::~GenericStorageReference()
{
    Location->Drop(Self);
    Path->Drop(Self);
}

bool mcc::GenericStorageReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericStorageReference::GenerateResult() const
{
    auto location = Location->GenerateResultUnwrap();
    auto path     = Path->GenerateResultUnwrap();

    auto with_argument = location.WithArgument || path.WithArgument;
    std::string location_value, path_value;

    switch (location.Type)
    {
    case ResultType_::Value:
        location_value = location.Value;
        break;

    case ResultType_::Argument:
        with_argument  = true;
        location_value = location.Name;
        break;

    default:
        Error(Where, "location must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, location.Type);
    }

    switch (path.Type)
    {
    case ResultType_::Value:
        path_value = path.Value;
        break;

    case ResultType_::Argument:
        with_argument = true;
        path_value    = path.Name;
        break;

    default:
        Error(Where, "path must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, path.Type);
    }

    return {
        .Type          = ResultType_::Reference,
        .WithArgument  = with_argument,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = location_value,
        .Path          = path_value,
    };
}
