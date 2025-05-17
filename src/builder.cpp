#include <utility>
#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

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
    return GetInsertParent()->Location;
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
    Assert(!!block, "block must not be null");

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
    Assert(!!m_InsertBlock, "insert block must not be null");

    auto block = m_InsertBlock;
    while (block->Parent)
        block = block->Parent;

    return block;
}

mcc::ValuePtr mcc::Builder::GetVariable(const std::string &name) const
{
    const auto block = GetInsertParent();

    if (!block->Variables.contains(name))
        block->Variables.emplace(name, NamedValue::Create(GetLocation(), name));

    return block->Variables.at(name);
}

mcc::InstructionPtr mcc::Builder::CreateStore(ValuePtr dst, ValuePtr src) const
{
    Assert(!!dst, "dst must not be null");
    Assert(!!src, "src must not be null");

    return Insert(StoreInstruction::Create(std::move(dst), std::move(src)));
}

mcc::InstructionPtr mcc::Builder::CreateComparison(const ComparatorE comparator, ValuePtr left, ValuePtr right) const
{
    Assert(comparator, "comparator must not be null");
    Assert(!!left, "left must not be null");
    Assert(!!right, "right must not be null");

    return Insert(
        ComparisonInstruction::Create(
            comparator,
            GetLocation(),
            std::move(left),
            std::move(right)));
}

mcc::InstructionPtr mcc::Builder::CreateOperation(const OperatorE operator_, std::vector<ValuePtr> operands) const
{
    Assert(operator_, "operator must not be null");

    for (auto &operand: operands)
        Assert(!!operand, "right must not be null");

    return Insert(
        OperationInstruction::Create(
            operator_,
            GetLocation(),
            std::move(operands)));
}

mcc::InstructionPtr mcc::Builder::CreateCommand(CommandT command) const
{
    return Insert(CommandInstruction::Create(GetLocation(), std::move(command)));
}

mcc::InstructionPtr mcc::Builder::CreateReturnVoid() const
{
    return Insert(ReturnInstruction::Create(GetLocation(), nullptr));
}

mcc::InstructionPtr mcc::Builder::CreateReturn(ValuePtr value) const
{
    Assert(!!value, "value must not be null");

    return Insert(ReturnInstruction::Create(GetLocation(), std::move(value)));
}

mcc::InstructionPtr mcc::Builder::CreateBranch(ValuePtr condition, BlockPtr then_target, BlockPtr else_target) const
{
    Assert(!!condition, "condition must not be null");
    Assert(!!then_target, "then target must not be null");
    Assert(!!else_target, "else target must not be null");

    return Insert(
        BranchInstruction::Create(
            GetLocation(),
            std::move(condition),
            std::move(then_target),
            std::move(else_target)));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(BlockPtr target) const
{
    Assert(!!target, "target must not be null");

    return Insert(DirectInstruction::Create(GetLocation(), std::move(target)));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(BlockPtr target, ValuePtr result, ValuePtr branch_result) const
{
    Assert(!!target, "target must not be null");
    Assert(!!result, "result must not be null");
    Assert(!!branch_result, "branch result must not be null");

    return Insert(
        DirectInstruction::Create(
            GetLocation(),
            std::move(target),
            std::move(result),
            std::move(branch_result)));
}

mcc::InstructionPtr mcc::Builder::CreateSwitch(
    ValuePtr condition,
    BlockPtr default_target,
    std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets) const
{
    Assert(!!condition, "condition must not be null");
    Assert(!!default_target, "default target must not be null");

    for (auto &[condition_, target_]: case_targets)
    {
        Assert(!!condition_, "case condition must not be null");
        Assert(!!target_, "case target must not be null");
    }

    return Insert(
        SwitchInstruction::Create(
            GetLocation(),
            std::move(condition),
            std::move(default_target),
            std::move(case_targets)));
}

mcc::InstructionPtr mcc::Builder::CreateThrow(ValuePtr value) const
{
    Assert(!!value, "value must not be null");

    return Insert(ThrowInstruction::Create(GetLocation(), std::move(value)));
}

mcc::ValuePtr mcc::Builder::CreateBranchResult() const
{
    return BranchResult::Create(GetLocation());
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    std::string callee,
    const bool builtin,
    std::vector<ValuePtr> arguments) const
{
    Assert(!callee.empty(), "callee must not be empty");

    return Insert(
        CallInstruction::Create(
            GetLocation(),
            std::move(callee),
            builtin,
            std::move(arguments)));
}

mcc::InstructionPtr mcc::Builder::AllocateValue() const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    return Insert(AllocationInstruction::CreateValue(GetLocation(), m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateArray() const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    return Insert(AllocationInstruction::CreateArray(GetLocation(), m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateObject() const
{
    Assert(!!m_InsertBlock, "insert block must not be null");

    return Insert(AllocationInstruction::CreateObject(GetLocation(), m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::CreateAppend(ValuePtr array, ValuePtr value, const bool stringify) const
{
    Assert(!!array, "array must not be null");
    Assert(!!value, "value must not be null");

    return Insert(
        ArrayInstruction::CreateAppend(
            GetLocation(),
            std::move(array),
            std::move(value),
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(ValuePtr array, ValuePtr value, const bool stringify) const
{
    Assert(!!array, "array must not be null");
    Assert(!!value, "value must not be null");

    return Insert(
        ArrayInstruction::CreatePrepend(
            GetLocation(),
            std::move(array),
            std::move(value),
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    ValuePtr array,
    ValuePtr value,
    const IndexT index,
    const bool stringify) const
{
    Assert(!!array, "array must not be null");
    Assert(!!value, "value must not be null");

    return Insert(
        ArrayInstruction::CreateInsert(
            GetLocation(),
            std::move(array),
            std::move(value),
            index,
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreateExtract(ValuePtr array, const IndexT index) const
{
    Assert(!!array, "array must not be null");

    return Insert(ArrayInstruction::CreateExtract(GetLocation(), std::move(array), index));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(ValuePtr object, ValuePtr value, std::string key) const
{
    Assert(!!object, "object must not be null");
    Assert(!!value, "value must not be null");
    Assert(!key.empty(), "key must not be empty");

    return Insert(
        ObjectInstruction::CreateInsert(
            GetLocation(),
            std::move(object),
            std::move(value),
            std::move(key)));
}

mcc::InstructionPtr mcc::Builder::Insert(InstructionPtr instruction) const
{
    Assert(!!m_InsertBlock, "insert block must not be null");
    Assert(!!instruction, "instruction must not be null");

    return m_InsertBlock->Instructions.emplace_back(std::move(instruction));
}

void mcc::Builder::Generate() const
{
    for (auto &block: m_Blocks)
    {
        auto &[location_, commands_] = m_Context.Pkg.Functions[block->Location];
        location_ = block->Location;
        commands_.clear();

        CommandVector commands(commands_);
        block->Generate(commands, false);
    }
}
