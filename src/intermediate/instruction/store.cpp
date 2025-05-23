#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ValuePtr &dst,
    const ValuePtr &src)
{
    return std::make_shared<StoreInstruction>(where, context, dst, src);
}

mcc::StoreInstruction::StoreInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const ValuePtr &dst,
    const ValuePtr &src)
    : Instruction(where, context, dst->Type),
      Dst(dst),
      Src(src)
{
    Dst->Use();
    Src->Use();
}

mcc::StoreInstruction::~StoreInstruction()
{
    Dst->Drop();
    Src->Drop();
}

void mcc::StoreInstruction::Generate(CommandVector &commands, bool stack) const
{
    if (Dst == Src)
        return;

    auto dst = Dst->GenerateResult(false);
    auto src = Src->GenerateResult(false);

    Assert(dst.Type == ResultType_Storage, Where, "destination must be {}, but is {}", ResultType_Storage, dst.Type);

    switch (src.Type)
    {
        case ResultType_Value:
            commands.Append("data modify storage {} {} set value {}", dst.Location, dst.Path, src.Value);
            break;

        case ResultType_Storage:
            if (dst.Location == src.Location && dst.Path == src.Path)
                break;

            commands.Append(
                "data modify storage {} {} set from storage {} {}",
                dst.Location,
                dst.Path,
                src.Location,
                src.Path);
            break;

        default:
            Error(
                Where,
                "src must be {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                src.Type);
    }
}

bool mcc::StoreInstruction::RequireStack() const
{
    return Dst->RequireStack() || Src->RequireStack();
}

mcc::Result mcc::StoreInstruction::GenerateResult(const bool stringify) const
{
    return Dst->GenerateResult(stringify);
}
