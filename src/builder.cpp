#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <utility>

mcc::Builder::Builder(Context &context)
    : m_Context(context),
      m_InsertBlock(nullptr)
{
}

mcc::Context &mcc::Builder::GetContext() const
{
    return m_Context;
}

const mcc::ResourceLocation &mcc::Builder::GetLocation() const
{
    Assert(!!m_InsertBlock, "no insert block");
    auto block = m_InsertBlock;
    while (block->Parent)
        block = block->Parent;
    return block->Location;
}

mcc::BlockPtr mcc::Builder::CreateFunction(ResourceLocation location, std::vector<std::string> parameters)
{
    if (location.Namespace.empty())
        location.Namespace = m_Context.Namespace;

    return m_Blocks.emplace_back(Block::CreateTopLevel(std::move(location), std::move(parameters)));
}

mcc::BlockPtr mcc::Builder::CreateBlock(const BlockPtr &parent)
{
    ResourceLocation location{
        .Namespace = m_Context.Namespace,
        .Path = std::format("{}/{}", parent->Location.Path, parent->Children.size()),
    };

    return m_Blocks.emplace_back(Block::Create(parent, std::move(location)));
}

void mcc::Builder::RemoveBlock(const BlockPtr &block)
{
    Assert(!!block, "pointer to block must not be null");

    for (auto i = m_Blocks.begin(); i != m_Blocks.end(); ++i)
    {
        if (*i != block)
            continue;

        m_Blocks.erase(i);
        return;
    }

    Error("undefined block {}", block->Location);
}

void mcc::Builder::SetInsertBlock(BlockPtr block)
{
    m_InsertBlock = std::move(block);
}

mcc::BlockPtr mcc::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

mcc::BlockPtr mcc::Builder::GetInsertParent() const
{
    Assert(!!m_InsertBlock, "no insert block");
    auto block = m_InsertBlock;
    while (block->Parent)
        block = block->Parent;
    return block;
}

mcc::InstructionPtr mcc::Builder::CreateStore(ValuePtr dst, ValuePtr src, const bool inline_) const
{
    return Insert(StoreInstruction::Create(std::move(dst), std::move(src)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateComparison(
    const ComparatorE comparator,
    ValuePtr left,
    ValuePtr right,
    const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        ComparisonInstruction::Create(comparator, GetLocation(), std::move(left), std::move(right)),
        inline_);
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const OperatorE operator_,
    ValuePtr left,
    ValuePtr right,
    const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        OperationInstruction::Create(operator_, GetLocation(), std::move(left), std::move(right)),
        inline_);
}

mcc::InstructionPtr mcc::Builder::CreateCommand(CommandT command, const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(CommandInstruction::Create(GetLocation(), std::move(command)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateReturn(
    ValuePtr value,
    const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(ReturnInstruction::Create(GetLocation(), std::move(value)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateBranch(
    ValuePtr condition,
    ValuePtr then_target,
    ValuePtr else_target,
    const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    Assert(!!condition, "condition must not be null");
    Assert(!!then_target, "then target must not be null");
    Assert(!!else_target, "else target must not be null");
    return Insert(
        BranchInstruction::Create(
            GetLocation(),
            std::move(condition),
            std::move(then_target),
            std::move(else_target)),
        inline_);
}

mcc::InstructionPtr mcc::Builder::CreateDirect(ValuePtr target, const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    Assert(!!target, "target must not be null");
    return Insert(DirectInstruction::Create(std::move(target)), inline_);
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    std::string callee,
    const bool builtin,
    std::vector<ValuePtr> arguments,
    const bool inline_) const
{
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        CallInstruction::Create(GetLocation(), std::move(callee), builtin, std::move(arguments)),
        inline_);
}

mcc::InstructionPtr mcc::Builder::AllocateValue(const bool inline_) const
{
    Assert(!inline_, "cannot inline allocation instruction");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(AllocationInstruction::CreateValue(GetLocation(), m_InsertBlock->StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateArray(const bool inline_) const
{
    Assert(!inline_, "cannot inline allocation instruction");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(AllocationInstruction::CreateArray(GetLocation(), m_InsertBlock->StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::AllocateObject(const bool inline_) const
{
    Assert(!inline_, "cannot inline allocation instruction");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(AllocationInstruction::CreateObject(GetLocation(), m_InsertBlock->StackIndex++), false);
}

mcc::InstructionPtr mcc::Builder::CreateAppend(
    ValuePtr array,
    ValuePtr value,
    const bool stringify,
    const bool inline_) const
{
    Assert(!inline_, "cannot inline array operation instruction");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        ArrayInstruction::CreateAppend(
            GetLocation(),
            std::move(array),
            std::move(value),
            stringify),
        false);
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(
    ValuePtr array,
    ValuePtr value,
    const bool stringify,
    const bool inline_) const
{
    Assert(!inline_, "cannot inline array operation");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        ArrayInstruction::CreatePrepend(
            GetLocation(),
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
    const bool inline_) const
{
    Assert(!inline_, "cannot inline array operation");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        ArrayInstruction::CreateInsert(
            GetLocation(),
            std::move(array),
            std::move(value),
            index,
            stringify),
        false);
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    ValuePtr object,
    ValuePtr value,
    std::string key,
    const bool inline_) const
{
    Assert(!inline_, "cannot inline object operation");
    Assert(!!m_InsertBlock, "no insert block");
    return Insert(
        ObjectInstruction::CreateInsert(
            GetLocation(),
            std::move(object),
            std::move(value),
            std::move(key)),
        false);
}

mcc::InstructionPtr mcc::Builder::Insert(InstructionPtr instruction, const bool inline_) const
{
    if (inline_)
        return instruction;

    Assert(!!m_InsertBlock, "no insert block");
    m_InsertBlock->Instructions.emplace_back(instruction);
    return m_InsertBlock->Instructions.back();
}

void mcc::Builder::Generate() const
{
    for (auto &block: m_Blocks)
    {
        auto &[location_, commands_] = m_Context.Pkg.Functions[block->Location];
        location_ = block->Location;
        commands_.clear();

        CommandVector commands(commands_);
        block->Generate(commands);
    }
}
