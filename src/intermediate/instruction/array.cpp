#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const ValuePtr &array,
        const ValuePtr &value)
{
    return Create(where, name, context, ArrayOperation_::Append, location, array, value, ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const ValuePtr &array,
        const ValuePtr &value)
{
    return Create(where, name, context, ArrayOperation_::Prepend, location, array, value, ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const ValuePtr &array,
        const ValuePtr &value,
        const IndexT index)
{
    return Create(where, name, context, ArrayOperation_::Insert, location, array, value, index);
}


mcc::ArrayInstruction::SPtr mcc::ArrayInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        ArrayOperation_ array_operation,
        const ResourceLocation &location,
        const ValuePtr &array,
        const ValuePtr &value,
        IndexT index)
{
    auto self =
            std::make_shared<ArrayInstruction>(where, name, context, array_operation, location, array, value, index);

    self->Self = self;
    self->Array->Use(self);
    if (self->Value)
        self->Value->Use(self);

    return self;
}

mcc::ArrayInstruction::ArrayInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ArrayOperation_ array_operation,
        ResourceLocation location,
        ValuePtr array,
        ValuePtr value,
        const IndexT index)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index)
{
}

mcc::ArrayInstruction::~ArrayInstruction()
{
    Array->Drop(Self);
    if (Value)
        Value->Drop(Self);
}

void mcc::ArrayInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto array = Array->GenerateResult();
    auto value = Value->GenerateResult();

    Assert(array.Type == ResultType_::Reference,
           Where,
           "array must be {}, but is {}",
           ResultType_::Reference,
           array.Type);

    std::string operation;
    switch (ArrayOperation)
    {
    case ArrayOperation_::Append:
        operation = "append";
        break;

    case ArrayOperation_::Prepend:
        operation = "prepend";
        break;

    case ArrayOperation_::Insert:
        operation = std::format("insert {}", Index);
        break;
    }

    std::string prefix;
    if (value.WithArgument)
        prefix = "$";

    switch (value.Type)
    {
    case ResultType_::Value:
        commands.Append(
                "{}data modify {} {} {} {} value {}",
                prefix,
                array.ReferenceType,
                array.Target,
                array.Path,
                operation,
                value.Value);
        break;

    case ResultType_::Reference:
        commands.Append(
                "{}data modify {} {} {} {} from {} {} {}",
                prefix,
                array.ReferenceType,
                array.Target,
                array.Path,
                operation,
                value.ReferenceType,
                value.Target,
                value.Path);
        break;

    case ResultType_::Argument:
        commands.Append(
                "$data modify {} {} {} {} value {}",
                array.ReferenceType,
                array.Target,
                array.Path,
                operation,
                value.Name);
        break;

    default:
        Error(Where,
              "value must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
              value.Type);
    }
}

bool mcc::ArrayInstruction::RequireStack() const
{
    return Array->RequireStack() || Value->RequireStack();
}
