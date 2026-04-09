#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::NotNullInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const ValuePtr &value)
{
    auto self = std::make_shared<NotNullInstruction>(where, name, context, location, value);

    self->Self = self;
    self->Value->Use(self);

    return self;
}

mcc::NotNullInstruction::NotNullInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        ResourceLocation location,
        ValuePtr value)
    : Instruction(
              where,
              name,
              context.GetNumber(),
              FieldType_ImmutableReference),
      Location(std::move(location)),
      Value(std::move(value))
{
}

mcc::NotNullInstruction::~NotNullInstruction()
{
    Value->Drop(Self);
}

void mcc::NotNullInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto value = Value->GenerateResult();
    Assert(value.Type == ResultType_Reference, Where, "value must be {}, but is {}", ResultType_Reference, value.Type);

    auto stack_path = GetStackPath();

    std::string prefix;
    if (value.WithArgument)
        prefix = "$";

    commands.Append("data modify storage {} {} set value 0", Location, stack_path);
    commands
            .Append("{}execute if data {} {} {} run data modify storage {} {} set value 1",
                    prefix,
                    value.ReferenceType,
                    value.Target,
                    value.Path,
                    Location,
                    stack_path);
}

bool mcc::NotNullInstruction::RequireStack() const
{
    return !Uses.empty() || Value->RequireStack();
}

mcc::Result mcc::NotNullInstruction::GenerateResult() const
{
    return {
        .Type          = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target        = Location.String(),
        .Path          = GetStackPath(),
    };
}
