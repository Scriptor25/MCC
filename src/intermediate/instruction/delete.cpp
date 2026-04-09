#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <utility>

mcc::InstructionPtr mcc::DeleteInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ValuePtr &value)
{
    auto self = std::make_shared<DeleteInstruction>(where, name, context, value);

    self->Self = self;
    self->Value->Use(self);

    return self;
}

mcc::DeleteInstruction::DeleteInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        ValuePtr value)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Value(std::move(value))
{
}

mcc::DeleteInstruction::~DeleteInstruction()
{
    Value->Drop(Self);
}

void mcc::DeleteInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto value = Value->GenerateResult();
    Assert(value.Type == ResultType_::Reference,
           Where,
           "value must be {}, but is {}",
           ResultType_::Reference,
           value.Type);

    std::string prefix;
    if (value.WithArgument)
        prefix = "$";

    commands.Append("{}data remove {} {} {}", prefix, value.ReferenceType, value.Target, value.Path);
}

bool mcc::DeleteInstruction::RequireStack() const
{
    return Value->RequireStack();
}
