#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericEntityReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &name,
    const std::string &path)
{
    return std::make_shared<GenericEntityReference>(where, type, name, path);
}

mcc::GenericEntityReference::GenericEntityReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &name,
    const std::string &path)
    : Value(where, type, true),
      Name(name),
      Path(path)
{
    Name->Use();
}

mcc::GenericEntityReference::~GenericEntityReference()
{
    Name->Drop();
}

bool mcc::GenericEntityReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericEntityReference::GenerateResult() const
{
    auto name = Name->GenerateResultUnwrap();
    Assert(name.Type == ResultType_Value, Where, "name must be {}, but is {}", ResultType_Value, name.Type);

    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = name.Value,
        .Path = Path,
    };
}
