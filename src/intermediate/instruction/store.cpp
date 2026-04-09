#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        const ValuePtr &dst,
        const ValuePtr &src)
{
    auto self = std::make_shared<StoreInstruction>(where, name, dst, src);

    self->Self = self;
    self->Dst->Use(self);
    self->Src->Use(self);

    return self;
}

mcc::StoreInstruction::StoreInstruction(
        const SourceLocation &where,
        const std::string &name,
        const ValuePtr &dst,
        ValuePtr src)
    : Instruction(
              where,
              name,
              dst->Type,
              dst->FieldType),
      Dst(dst),
      Src(std::move(src))
{
}

mcc::StoreInstruction::~StoreInstruction()
{
    Dst->Drop(Self);
    Src->Drop(Self);
}

void mcc::StoreInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    if (Dst == Src)
        return;

    auto dst = Dst->GenerateResult();
    auto src = Src->GenerateResult();

    Assert(dst.Type == ResultType_Reference,
           Where,
           "destination must be {}, but is {}",
           ResultType_Reference,
           dst.Type);

    std::string prefix;
    if (dst.WithArgument || src.WithArgument)
        prefix = "$";

    switch (src.Type)
    {
    case ResultType_Value:
        commands
                .Append("{}data modify {} {} {} set value {}",
                        prefix,
                        dst.ReferenceType,
                        dst.Target,
                        dst.Path,
                        src.Value);
        break;

    case ResultType_Reference:
        if (dst.Target == src.Target && dst.Path == src.Path)
        {
            break;
        }

        commands
                .Append("{}data modify {} {} {} set from {} {} {}",
                        prefix,
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
        Error(Where,
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
