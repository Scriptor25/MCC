#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::Builder::Builder(Context &context, const ResourceLocation &location)
    : m_Context(context),
      m_Location(location)
{
}

mcc::Context &mcc::Builder::GetContext() const
{
    return m_Context;
}

const mcc::ResourceLocation &mcc::Builder::GetLocation() const
{
    return m_Location;
}

bool mcc::Builder::RequireStack() const
{
    return std::ranges::any_of(
        m_Instructions,
        [](auto &instruction)
        {
            return instruction->RequireStack();
        });
}

bool mcc::Builder::RequireCleanup() const
{
    return m_Instructions.empty() || !std::dynamic_pointer_cast<ReturnInstruction>(m_Instructions.back());
}

mcc::InstructionPtr mcc::Builder::CreateStore(ValuePtr dst, ValuePtr src, const bool inline_)
{
    return Insert(StoreInstruction::Create(std::move(dst), std::move(src)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateComparison(
    const ComparatorE comparator,
    ValuePtr left,
    ValuePtr right,
    const bool inline_)
{
    return Insert(ComparisonInstruction::Create(comparator, m_Location, std::move(left), std::move(right)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const OperatorE operator_,
    ValuePtr left,
    ValuePtr right,
    const bool inline_)
{
    return Insert(OperationInstruction::Create(operator_, m_Location, std::move(left), std::move(right)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    const CalleeE callee,
    std::vector<ValuePtr> arguments,
    const bool inline_)
{
    return Insert(CallInstruction::Create(m_Location, callee, std::move(arguments)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateReturn(
    ValuePtr value,
    const bool inline_)
{
    return Insert(ReturnInstruction::Create(m_Location, std::move(value)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateIf(
    const bool unless,
    ValuePtr condition,
    ValuePtr then,
    ValuePtr else_,
    const bool inline_)
{
    return Insert(
        IfUnlessInstruction::Create(m_Location, unless, std::move(condition), std::move(then), std::move(else_)),
        inline_);
}

mcc::InstructionPtr mcc::Builder::AllocateValue(const bool inline_)
{
    Assert(!inline_, "cannot inline allocation instruction");

    return Insert(AllocationInstruction::CreateValue(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateArray(const bool inline_)
{
    Assert(!inline_, "cannot inline allocation instruction");

    return Insert(AllocationInstruction::CreateArray(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateObject(const bool inline_)
{
    Assert(!inline_, "cannot inline allocation instruction");

    return Insert(AllocationInstruction::CreateObject(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::CreateAppend(
    ValuePtr array,
    ValuePtr value,
    const bool stringify,
    const bool inline_)
{
    Assert(!inline_, "cannot inline array operation instruction");

    return Insert(
        ArrayInstruction::CreateAppend(
            m_Location,
            std::move(array),
            std::move(value),
            stringify),
        false);
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(
    ValuePtr array,
    ValuePtr value,
    const bool stringify,
    const bool inline_)
{
    Assert(!inline_, "cannot inline array operation");

    return Insert(
        ArrayInstruction::CreatePrepend(
            m_Location,
            std::move(array),
            std::move(value),
            stringify),
        false);
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    ValuePtr array,
    ValuePtr value,
    const IndexT index,
    const bool stringify,
    const bool inline_)
{
    Assert(!inline_, "cannot inline array operation");

    return Insert(
        ArrayInstruction::CreateInsert(
            m_Location,
            std::move(array),
            std::move(value),
            index,
            stringify),
        false);
}

mcc::InstructionPtr mcc::Builder::CreateInsert(ValuePtr object, ValuePtr value, std::string key, const bool inline_)
{
    Assert(!inline_, "cannot inline object operation");

    return Insert(
        ObjectInstruction::CreateInsert(
            m_Location,
            std::move(object),
            std::move(value),
            std::move(key)),
        false);
}

mcc::InstructionPtr mcc::Builder::Insert(InstructionPtr instruction, const bool inline_)
{
    if (inline_)
        return instruction;

    m_Instructions.emplace_back(instruction);
    return m_Instructions.back();
}

void mcc::Builder::Generate(std::vector<CommandT> &commands, const bool use_stack) const
{
    CommandVector command_vector(commands);
    for (auto &instruction: m_Instructions)
        instruction->Generate(command_vector, use_stack);
}
