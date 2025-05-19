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

bool mcc::Builder::HasFunction(const ResourceLocation &location) const
{
    return m_Functions.contains(location.Namespace) && m_Functions.at(location.Namespace).contains(location.Path);
}

mcc::FunctionPtr mcc::Builder::CreateFunction(
    const SourceLocation &where,
    ResourceLocation location,
    const ParameterList &parameters,
    const TypePtr &result,
    const bool throws)
{
    if (location.Namespace.empty())
        location.Namespace = m_Context.Namespace;

    auto &function = m_Functions[location.Namespace][location.Path];
    Assert(!function, where, "already defined function {}", location);
    return function = Function::Create(where, location, parameters, result, throws);
}

mcc::FunctionPtr mcc::Builder::GetFunction(const SourceLocation &where, ResourceLocation location)
{
    if (location.Namespace.empty())
        location.Namespace = m_Context.Namespace;

    auto &function = m_Functions[location.Namespace][location.Path];
    Assert(!!function, where, "undefined function {}", location);
    return function;
}

void mcc::Builder::SetInsertBlock(const BlockPtr &block)
{
    m_InsertBlock = block;
}

mcc::BlockPtr mcc::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

void mcc::Builder::PushVariables()
{
    m_Variables.emplace_back();
}

void mcc::Builder::PopVariables()
{
    m_Variables.pop_back();
}

mcc::ValuePtr mcc::Builder::CreateVariable(const SourceLocation &where, const TypePtr &type, const std::string &name)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!m_Variables.empty(), where, "variables must not be empty");

    auto &variables = m_Variables.back();
    Assert(!variables.contains(name), where, "already defined variable {}", name);
    return variables[name] = NamedValue::Create(where, type, m_InsertBlock->Parent->Location, name);
}

mcc::ValuePtr mcc::Builder::GetVariable(const SourceLocation &where, const std::string &name) const
{
    Assert(!m_Variables.empty(), where, "variables must not be empty");

    for (auto variables = m_Variables.rbegin(); variables != m_Variables.rend(); ++variables)
        if (variables->contains(name))
            return variables->at(name);

    Error(where, "undefined variable {}", name);
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
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!comparator, where, "comparator must not be null");
    Assert(!!left, where, "left must not be null");
    Assert(!!right, where, "right must not be null");

    return Insert(
        where,
        ComparisonInstruction::Create(
            where,
            comparator,
            m_InsertBlock->Parent->Location,
            left,
            right));
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const SourceLocation &where,
    const OperatorE operator_,
    const std::vector<ValuePtr> &operands) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(operator_, where, "operator must not be null");

    for (auto &operand: operands)
        Assert(!!operand, where, "right must not be null");

    return Insert(
        where,
        OperationInstruction::Create(
            where,
            operator_,
            m_InsertBlock->Parent->Location,
            operands));
}

mcc::InstructionPtr mcc::Builder::CreateCommand(
    const SourceLocation &where,
    const TypePtr &type,
    const CommandT &command) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, CommandInstruction::Create(where, type, m_InsertBlock->Parent->Location, command));
}

mcc::InstructionPtr mcc::Builder::CreateReturnVoid(const SourceLocation &where) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, ReturnInstruction::Create(where, m_InsertBlock->Parent->Location, nullptr));
}

mcc::InstructionPtr mcc::Builder::CreateReturn(const SourceLocation &where, const ValuePtr &value) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(where, ReturnInstruction::Create(where, m_InsertBlock->Parent->Location, value));
}

mcc::InstructionPtr mcc::Builder::CreateBranch(
    const SourceLocation &where,
    const ValuePtr &condition,
    const BlockPtr &then_target,
    const BlockPtr &else_target) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!condition, where, "condition must not be null");
    Assert(!!then_target, where, "then target must not be null");
    Assert(!!else_target, where, "else target must not be null");

    return Insert(
        where,
        BranchInstruction::Create(
            where,
            m_InsertBlock->Parent->Location,
            condition,
            then_target,
            else_target));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(const SourceLocation &where, const BlockPtr &target) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!target, where, "target must not be null");

    return Insert(where, DirectInstruction::Create(where, m_InsertBlock->Parent->Location, target));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(
    const SourceLocation &where,
    const BlockPtr &target,
    const ValuePtr &result,
    const ValuePtr &branch_result) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!target, where, "target must not be null");
    Assert(!!result, where, "result must not be null");
    Assert(!!branch_result, where, "branch result must not be null");

    return Insert(
        where,
        DirectInstruction::Create(
            where,
            m_InsertBlock->Parent->Location,
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
    Assert(!!m_InsertBlock, where, "insert block must not be null");
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
            m_InsertBlock->Parent->Location,
            condition,
            default_target,
            case_targets));
}

mcc::InstructionPtr mcc::Builder::CreateThrow(
    const SourceLocation &where,
    const ValuePtr &value,
    const BlockPtr &landing_pad) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(where, ThrowInstruction::Create(where, m_InsertBlock->Parent->Location, value, landing_pad));
}

mcc::ValuePtr mcc::Builder::CreateBranchResult(const SourceLocation &where, const TypePtr &type) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return BranchResult::Create(where, type, m_InsertBlock->Parent->Location);
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    const SourceLocation &where,
    const ResourceLocation &callee,
    const std::vector<ValuePtr> &arguments,
    const BlockPtr &landing_pad)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    const auto &function = m_Functions[callee.Namespace][callee.Path];
    Assert(!!function, where, "undefined function {}", callee);

    const auto type = function->Type;

    std::vector<std::pair<std::string, ValuePtr>> argument_pairs;
    for (unsigned i = 0; i < arguments.size(); ++i)
        argument_pairs.emplace_back(function->Parameters.at(i).first, arguments.at(i));

    return Insert(
        where,
        CallInstruction::Create(
            where,
            type,
            m_InsertBlock->Parent->Location,
            callee,
            argument_pairs,
            function->Throws,
            landing_pad));
}

mcc::InstructionPtr mcc::Builder::CreateMacro(
    const SourceLocation &where,
    const std::string &name,
    const std::vector<ValuePtr> &arguments) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, MacroInstruction::Create(where, m_InsertBlock->Parent->Location, name, arguments));
}

mcc::InstructionPtr mcc::Builder::AllocateValue(const SourceLocation &where, const TypePtr &type) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(
        where,
        AllocationInstruction::CreateValue(
            where,
            type,
            m_InsertBlock->Parent->Location,
            m_InsertBlock->Parent->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateArray(const SourceLocation &where, const TypePtr &type) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(
        where,
        AllocationInstruction::CreateArray(
            where,
            type,
            m_InsertBlock->Parent->Location,
            m_InsertBlock->Parent->StackIndex++));
}

mcc::InstructionPtr mcc::Builder::AllocateObject(const SourceLocation &where, const TypePtr &type) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(
        where,
        AllocationInstruction::CreateObject(
            where,
            type,
            m_InsertBlock->Parent->Location,
            m_InsertBlock->Parent->StackIndex++));
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
            m_InsertBlock->Parent->Location,
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
            m_InsertBlock->Parent->Location,
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
            m_InsertBlock->Parent->Location,
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

    return Insert(where, ArrayInstruction::CreateExtract(where, m_InsertBlock->Parent->Location, array, index));
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
            m_InsertBlock->Parent->Location,
            object,
            value,
            key));
}

mcc::ValuePtr mcc::Builder::CreateStoreResult(
    const SourceLocation &where,
    const TypePtr &type,
    const std::string &variable)
{
    Assert(!variable.empty(), where, "variable name must not be empty");

    const auto dst = CreateVariable(where, type, variable);
    const auto src = FunctionResult::Create(where, type, m_InsertBlock->Parent->Location);
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
    for (auto &functions: m_Functions | std::views::values)
        for (auto &function: functions | std::views::values)
            function->GenerateFunction(m_Context);
}
