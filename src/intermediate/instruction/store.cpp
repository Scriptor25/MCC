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
            {
                break;
            }

            commands.Append(
                "data modify storage {} {} set from storage {} {}",
                dst.Location,
                dst.Path,
                src.Location,
                src.Path);
            break;

        case ResultType_Argument:
            commands.Append("$data modify storage {} {} set value {}", dst.Location, dst.Path, src.Name);
            break;

        default:
            Error(
                Where,
                "src must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Argument,
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
