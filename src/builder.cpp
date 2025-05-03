#include <mcc/builder.hpp>
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
    return Insert(ComparisonInstruction::Create(comparator, std::move(left), std::move(right)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const OperatorE operator_,
    ValuePtr left,
    ValuePtr right,
    const bool inline_)
{
    return Insert(OperationInstruction::Create(operator_, std::move(left), std::move(right)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    const CalleeE callee,
    std::vector<ValuePtr> arguments,
    const bool inline_)
{
    return Insert(CallInstruction::Create(callee, std::move(arguments)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateReturn(
    ValuePtr value,
    const bool inline_)
{
    return Insert(ReturnInstruction::Create(std::move(value)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateIf(
    const bool unless,
    ValuePtr condition,
    ValuePtr then,
    const bool inline_)
{
    return Insert(IfUnlessInstruction::Create(unless, std::move(condition), std::move(then)), inline_);
}

mcc::InstructionPtr mcc::Builder::AllocateValue(const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline allocation");

    return Insert(AllocationInstruction::CreateValue(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateArray(const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline allocation");

    return Insert(AllocationInstruction::CreateArray(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateObject(const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline allocation");

    return Insert(AllocationInstruction::CreateObject(m_Location, m_StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::CreateAppend(ValuePtr array, ValuePtr value, const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline array operation");

    return Insert(
        ArrayInstruction::CreateAppend(
            m_Location,
            std::move(array),
            std::move(value)),
        false);
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(ValuePtr array, ValuePtr value, const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline array operation");

    return Insert(
        ArrayInstruction::CreatePrepend(
            m_Location,
            std::move(array),
            std::move(value)),
        false);
}

mcc::InstructionPtr mcc::Builder::CreateInsert(ValuePtr array, ValuePtr value, const IndexT index, const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline array operation");

    return Insert(
        ArrayInstruction::CreateInsert(
            m_Location,
            std::move(array),
            std::move(value),
            index),
        false);
}

mcc::InstructionPtr mcc::Builder::CreateInsert(ValuePtr object, ValuePtr value, std::string key, const bool inline_)
{
    if (inline_)
        throw std::runtime_error("cannot inline object operation");

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

void mcc::Builder::Gen(std::vector<Command> &commands) const
{
    for (auto &instruction: m_Instructions)
        instruction->Gen(commands);
}
