#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::DeleteInstruction::Create(const SourceLocation &where, TypeContext &context, const ValuePtr &value)
{
    return std::make_shared<DeleteInstruction>(where, context, value);
}

mcc::DeleteInstruction::DeleteInstruction(const SourceLocation &where, TypeContext &context, const ValuePtr &value)
    : Instruction(where, context.GetVoid(), false), Value(value)
{
    value->Use();
}

mcc::DeleteInstruction::~DeleteInstruction()
{
    Value->Drop();
}

void mcc::DeleteInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto value = Value->GenerateResult();
    Assert(value.Type == ResultType_Reference, Where, "value must be {}, but is {}", ResultType_Reference, value.Type);

    std::string prefix;
    if (value.WithArgument)
        prefix = "$";

    commands.Append("{}data remove {} {} {}", prefix, value.ReferenceType, value.Target, value.Path);
}

bool mcc::DeleteInstruction::RequireStack() const
{
    return Value->RequireStack();
}
