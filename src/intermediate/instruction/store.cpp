#include <utility>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(
    const SourceLocation &where,
    const ValuePtr &dst,
    const ValuePtr &src)
{
    return std::make_shared<StoreInstruction>(where, dst, src);
}

mcc::StoreInstruction::StoreInstruction(
    const SourceLocation &where,
    const ValuePtr &dst,
    ValuePtr src)
    : Instruction(where, dst->Type, dst->IsMutable),
      Dst(dst),
      Src(std::move(src))
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
    {
        return;
    }

    auto dst = Dst->GenerateResult();
    auto src = Src->GenerateResult();

    Assert(
        dst.Type == ResultType_Reference,
        Where,
        "destination must be {}, but is {}",
        ResultType_Reference,
        dst.Type);

    switch (src.Type)
    {
        case ResultType_Value:
            commands.Append("data modify {} {} {} set value {}", dst.ReferenceType, dst.Target, dst.Path, src.Value);
            break;

        case ResultType_Reference:
            if (dst.Target == src.Target && dst.Path == src.Path)
            {
                break;
            }

            commands.Append(
                "data modify {} {} {} set from {} {} {}",
                dst.ReferenceType,
                dst.Target,
                dst.Path,
                src.ReferenceType,
                src.Target,
                src.Path);
            break;

        case ResultType_Argument:
            commands.Append("$data modify {} {} {} set value {}", dst.ReferenceType, dst.Target, dst.Path, src.Name);
            break;

        default:
            Error(
                Where,
                "src must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Reference,
                ResultType_Argument,
                src.Type);
    }
}

bool mcc::StoreInstruction::RequireStack() const
{
    return Dst->RequireStack() || Src->RequireStack();
}

mcc::Result mcc::StoreInstruction::GenerateResult() const
{
    return Dst->GenerateResult();
}
