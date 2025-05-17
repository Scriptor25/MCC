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

const mcc::ResourceLocation &mcc::Builder::GetLocation(const SourceLocation &where) const
{
    return GetInsertParent(where)->Location;
}

mcc::BlockPtr mcc::Builder::CreateFunction(
    const SourceLocation &where,
    ResourceLocation location,
    const ParameterList &parameters)
{
    if (location.Namespace.empty())
        location.Namespace = m_Context.Namespace;

    return m_Blocks.emplace_back(Block::CreateTopLevel(where, location, parameters));
}

mcc::BlockPtr mcc::Builder::CreateBlock(const SourceLocation &where, const BlockPtr &parent)
{
    const ResourceLocation location{
        .Namespace = m_Context.Namespace,
        .Path = std::format("{}/{}", parent->Location.Path, parent->Children.size()),
    };

    return m_Blocks.emplace_back(Block::Create(where, parent, location));
}

mcc::BlockPtr mcc::Builder::FindBlock(const ResourceLocation &location) const
{
    for (auto &block: m_Blocks)
        if (block->Location == location)
            return block;
    return nullptr;
}

void mcc::Builder::RemoveBlock(const SourceLocation &where, const BlockPtr &block)
{
    Assert(!!block, where, "block must not be null");

    for (auto i = m_Blocks.begin(); i != m_Blocks.end(); ++i)
    {
        if (*i != block)
            continue;

        m_Blocks.erase(i);
        return;
    }

    Error(where, "undefined block {}", block->Location);
}

void mcc::Builder::SetInsertBlock(const BlockPtr &block)
{
    m_InsertBlock = block;
}

mcc::BlockPtr mcc::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

mcc::BlockPtr mcc::Builder::GetInsertParent(const SourceLocation &where) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    auto block = m_InsertBlock;
    while (block->Parent)
        block = block->Parent;

    return block;
}

mcc::ValuePtr mcc::Builder::GetVariable(const SourceLocation &where, const std::string &name) const
{
    const auto block = GetInsertParent(where);

    if (!block->Variables.contains(name))
        block->Variables.emplace(name, NamedValue::Create(where, GetLocation(where), name));

    return block->Variables.at(name);
}

mcc::InstructionPtr mcc::Builder::CreateStore(
    const SourceLocation &where,
    const ValuePtr &dst,
    const ValuePtr &src) const
{
    Assert(!!dst, where, "dst must not be null");
    Assert(!!src, where, "src must not be null");

    return Insert(where, StoreInstruction::Create(where, dst, src));
}

mcc::InstructionPtr mcc::Builder::CreateComparison(
    const SourceLocation &where,
    const ComparatorE comparator,
    const ValuePtr &left,
    const ValuePtr &right) const
{
    Assert(comparator, where, "comparator must not be null");
    Assert(!!left, where, "left must not be null");
    Assert(!!right, where, "right must not be null");

    return Insert(
        where,
        ComparisonInstruction::Create(
            where,
            comparator,
            GetLocation(where),
            left,
            right));
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const SourceLocation &where,
    const OperatorE operator_,
    const std::vector<ValuePtr> &operands) const
{
    Assert(operator_, where, "operator must not be null");

    for (auto &operand: operands)
        Assert(!!operand, where, "right must not be null");

    return Insert(
        where,
        OperationInstruction::Create(
            where,
            operator_,
            GetLocation(where),
            operands));
}

mcc::InstructionPtr mcc::Builder::CreateCommand(const SourceLocation &where, const CommandT &command) const
{
    return Insert(where, CommandInstruction::Create(where, GetLocation(where), command));
}

mcc::InstructionPtr mcc::Builder::CreateReturnVoid(const SourceLocation &where) const
{
    return Insert(where, ReturnInstruction::Create(where, GetLocation(where), nullptr));
}

mcc::InstructionPtr mcc::Builder::CreateReturn(const SourceLocation &where, const ValuePtr &value) const
{
    Assert(!!value, where, "value must not be null");

    return Insert(where, ReturnInstruction::Create(where, GetLocation(where), value));
}

mcc::InstructionPtr mcc::Builder::CreateBranch(
    const SourceLocation &where,
    const ValuePtr &condition,
    const BlockPtr &then_target,
    const BlockPtr &else_target) const
{
    Assert(!!condition, where, "condition must not be null");
    Assert(!!then_target, where, "then target must not be null");
    Assert(!!else_target, where, "else target must not be null");

    return Insert(
        where,
        BranchInstruction::Create(
            where,
            GetLocation(where),
            condition,
            then_target,
            else_target));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(const SourceLocation &where, const BlockPtr &target) const
{
    Assert(!!target, where, "target must not be null");

    return Insert(where, DirectInstruction::Create(where, GetLocation(where), target));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(
    const SourceLocation &where,
    const BlockPtr &target,
    const ValuePtr &result,
    const ValuePtr &branch_result) const
{
    Assert(!!target, where, "target must not be null");
    Assert(!!result, where, "result must not be null");
    Assert(!!branch_result, where, "branch result must not be null");

    return Insert(
        where,
        DirectInstruction::Create(
            where,
            GetLocation(where),
            target,
            result,
            branch_result));
}

mcc::InstructionPtr mcc::Builder::CreateSwitch(
    const SourceLocation &where,
    const ValuePtr &condition,
    const BlockPtr &default_target,
    const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets) const
{
    Assert(!!condition, where, "condition must not be null");
    Assert(!!default_target, where, "default target must not be null");

    for (auto &[condition_, target_]: case_targets)
    {
        Assert(!!condition_, where, "case condition must not be null");
        Assert(!!target_, where, "case target must not be null");
    }

    return Insert(
        where,
        SwitchInstruction::Create(
            where,
            GetLocation(where),
            condition,
            default_target,
            case_targets));
}

mcc::InstructionPtr mcc::Builder::CreateThrow(
    const SourceLocation &where,
    const ValuePtr &value,
    const BlockPtr &landing_pad) const
{
    Assert(!!value, where, "value must not be null");

    return Insert(where, ThrowInstruction::Create(where, GetLocation(where), value, landing_pad));
}

mcc::ValuePtr mcc::Builder::CreateBranchResult(const SourceLocation &where, const TypeID type) const
{
    return BranchResult::Create(where, type, GetLocation(where));
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    const SourceLocation &where,
    const ResourceLocation &callee,
    const std::vector<ValuePtr> &arguments,
    const BlockPtr &landing_pad) const
{
    auto may_throw = !callee.Namespace.empty();
    if (may_throw)
    {
        const auto block = FindBlock(callee);
        Assert(!!block, where, "undefined function {}", callee);
        may_throw = block->MayThrow();
    }

    return Insert(
        where,
        CallInstruction::Create(
            where,
            GetLocation(where),
            callee,
            arguments,
            may_throw,
            landing_pad));
}

mcc::InstructionPtr mcc::Builder::AllocateValue(const SourceLocation &where) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(
        where,
        AllocationInstruction::CreateValue(
            where,
            GetLocation(where),
            m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateArray(const SourceLocation &where) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, AllocationInstruction::CreateArray(where, GetLocation(where), m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateObject(const SourceLocation &where) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, AllocationInstruction::CreateObject(where, GetLocation(where), m_InsertBlock->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::CreateAppend(
    const SourceLocation &where,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(
        where,
        ArrayInstruction::CreateAppend(
            where,
            GetLocation(where),
            array,
            value,
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(
    const SourceLocation &where,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(
        where,
        ArrayInstruction::CreatePrepend(
            where,
            GetLocation(where),
            array,
            value,
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    const ValuePtr &array,
    const SourceLocation &where,
    const ValuePtr &value,
    const IndexT index,
    const bool stringify) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(
        where,
        ArrayInstruction::CreateInsert(
            where,
            GetLocation(where),
            array,
            value,
            index,
            stringify));
}

mcc::InstructionPtr mcc::Builder::CreateExtract(
    const SourceLocation &where,
    const ValuePtr &array,
    const IndexT index) const
{
    Assert(!!array, where, "array must not be null");

    return Insert(where, ArrayInstruction::CreateExtract(where, GetLocation(where), array, index));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    const SourceLocation &where,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key) const
{
    Assert(!!object, where, "object must not be null");
    Assert(!!value, where, "value must not be null");
    Assert(!key.empty(), where, "key must not be empty");

    return Insert(
        where,
        ObjectInstruction::CreateInsert(
            where,
            GetLocation(where),
            object,
            value,
            key));
}

mcc::ValuePtr mcc::Builder::CreateStoreResult(
    const SourceLocation &where,
    const TypeID type,
    const std::string &variable) const
{
    Assert(!variable.empty(), where, "variable name must not be empty");

    const auto dst = GetVariable(where, variable);
    const auto src = FunctionResult::Create(where, type, GetLocation(where));
    return CreateStore(where, dst, src);
}

mcc::InstructionPtr mcc::Builder::Insert(const SourceLocation &where, InstructionPtr instruction) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!instruction, where, "instruction must not be null");

    return m_InsertBlock->Instructions.emplace_back(instruction);
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
