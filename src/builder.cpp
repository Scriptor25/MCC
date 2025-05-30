#include <ranges>
#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::Builder::Builder(TypeContext &context, Package &package)
    : m_Context(context),
      m_Package(package)
{
}

mcc::TypeContext &mcc::Builder::GetContext() const
{
    return m_Context;
}

mcc::Package &mcc::Builder::GetPackage() const
{
    return m_Package;
}

std::string mcc::Builder::GetNamespace() const
{
    return m_Namespace;
}

void mcc::Builder::SetNamespace(const std::string &namespace_)
{
    m_Namespace = namespace_;
}

bool mcc::Builder::HasFunction(ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

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
        location.Namespace = m_Namespace;

    auto &function = m_Functions[location.Namespace][location.Path];
    Assert(!function, where, "already defined function {}", location);
    return function = Function::Create(where, m_Context, location, parameters, result, throws);
}

mcc::FunctionPtr mcc::Builder::GetFunction(const SourceLocation &where, ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    Assert(m_Functions.contains(location.Namespace), where, "undefined namespace {}", location.Namespace);
    Assert(m_Functions.at(location.Namespace).contains(location.Path), where, "undefined function {}", location);

    return m_Functions.at(location.Namespace).at(location.Path);
}

bool mcc::Builder::HasGlobal(ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    return m_Globals.contains(location.Namespace) && m_Globals.at(location.Namespace).contains(location.Path);
}

mcc::ValuePtr mcc::Builder::CreateGlobal(const SourceLocation &where, ResourceLocation location, const TypePtr &type)
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    auto &global = m_Globals[location.Namespace][location.Path];
    Assert(!global, where, "already defined global {}", location);
    return global = GenericStorageReference::Create(
               where,
               type,
               { location.Namespace, { "__storage__" } },
               location.Path.front(),
               true);
}

mcc::ValuePtr mcc::Builder::GetGlobal(const SourceLocation &where, ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    Assert(m_Globals.contains(location.Namespace), where, "undefined namespace {}", location.Namespace);
    Assert(m_Globals.at(location.Namespace).contains(location.Path), where, "undefined global {}", location);

    return m_Globals.at(location.Namespace).at(location.Path);
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

mcc::ValuePtr mcc::Builder::CreateVariable(
    const SourceLocation &where,
    const TypePtr &type,
    const std::string &name,
    const bool is_mutable,
    const ValuePtr &initializer)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!m_Variables.empty(), where, "variables must not be empty");
    Assert(!!type, where, "type must not be null");
    Assert(!name.empty(), where, "name must not be empty");

    auto &variable = m_Variables.back()[name];
    Assert(!variable, where, "already defined variable {}", name);

    variable = Allocate(where, type, is_mutable);
    if (initializer)
        (void) CreateStore(where, variable, initializer, true);
    return variable;
}

mcc::ValuePtr mcc::Builder::InsertVariable(const SourceLocation &where, const std::string &name, const ValuePtr &value)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!m_Variables.empty(), where, "variables must not be empty");
    Assert(!name.empty(), where, "name must not be empty");
    Assert(!!value, where, "value must not be null");

    auto &variables = m_Variables.back();
    auto &variable = variables[name];
    Assert(!variable, where, "already defined variable {}", name);

    return variable = value;
}

mcc::ValuePtr mcc::Builder::GetVariable(const SourceLocation &where, const std::string &name) const
{
    Assert(!m_Variables.empty(), where, "variables must not be empty");

    for (const auto &variables : std::ranges::reverse_view(m_Variables))
        if (variables.contains(name))
            return variables.at(name);

    const ResourceLocation location{ { name } };

    if (HasGlobal(location))
        return GetGlobal(where, location);

    if (HasFunction(location))
        return GetFunction(where, location);

    Error(where, "undefined variable {}", name);
}

mcc::InstructionPtr mcc::Builder::CreateStore(
    const SourceLocation &where,
    const ValuePtr &dst,
    const ValuePtr &src,
    const bool force) const
{
    Assert(!!dst, where, "dst must not be null");
    Assert(!!src, where, "src must not be null");
    Assert(force || dst->IsMutable, where, "dst must be mutable");

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
        ComparisonInstruction::Create(where, m_Context, comparator, m_InsertBlock->Parent->Location, left, right));
}

mcc::InstructionPtr mcc::Builder::CreateOperation(
    const SourceLocation &where,
    const OperatorE operator_,
    const std::vector<ValuePtr> &operands) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(operator_, where, "operator must not be null");

    for (auto &operand : operands)
        Assert(!!operand, where, "right must not be null");

    return Insert(
        where,
        OperationInstruction::Create(where, m_Context, operator_, m_InsertBlock->Parent->Location, operands));
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

    return Insert(where, ReturnInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, nullptr));
}

mcc::InstructionPtr mcc::Builder::CreateReturn(const SourceLocation &where, const ValuePtr &value) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!value, where, "value must not be null");

    return Insert(where, ReturnInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, value));
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

    m_InsertBlock->Successors.insert(then_target);
    m_InsertBlock->Successors.insert(else_target);
    then_target->Predecessors.insert(m_InsertBlock);
    else_target->Predecessors.insert(m_InsertBlock);

    return Insert(
        where,
        BranchInstruction::Create(
            where,
            m_Context,
            m_InsertBlock->Parent->Location,
            condition,
            then_target,
            else_target));
}

mcc::InstructionPtr mcc::Builder::CreateDirect(const SourceLocation &where, const BlockPtr &target) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!target, where, "target must not be null");

    m_InsertBlock->Successors.insert(target);
    target->Predecessors.insert(m_InsertBlock);

    return Insert(where, DirectBranchInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, target));
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

    m_InsertBlock->Successors.insert(target);
    target->Predecessors.insert(m_InsertBlock);

    return Insert(
        where,
        DirectBranchInstruction::Create(
            where,
            m_Context,
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

    for (auto &[condition_, target_] : case_targets)
    {
        Assert(!!condition_, where, "case condition must not be null");
        Assert(!!target_, where, "case target must not be null");

        m_InsertBlock->Successors.insert(target_);
        target_->Predecessors.insert(m_InsertBlock);
    }

    m_InsertBlock->Successors.insert(default_target);
    default_target->Predecessors.insert(m_InsertBlock);

    return Insert(
        where,
        SwitchInstruction::Create(
            where,
            m_Context,
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

    if (landing_pad)
    {
        m_InsertBlock->Successors.insert(landing_pad);
        landing_pad->Predecessors.insert(m_InsertBlock);
    }

    return Insert(
        where,
        ThrowInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, value, landing_pad));
}

mcc::ValuePtr mcc::Builder::CreateBranchResult(const SourceLocation &where, const TypePtr &type) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return BranchResult::Create(where, type, m_InsertBlock->Parent->Location);
}

mcc::InstructionPtr mcc::Builder::CreateCall(
    const SourceLocation &where,
    const FunctionPtr &callee,
    const std::vector<ValuePtr> &arguments,
    const BlockPtr &landing_pad) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    std::vector<std::pair<std::string, ValuePtr>> argument_pairs;
    for (unsigned i = 0; i < arguments.size(); ++i)
        argument_pairs.emplace_back(callee->Parameters[i].Name, arguments[i]);

    if (landing_pad)
    {
        m_InsertBlock->Successors.insert(landing_pad);
        landing_pad->Predecessors.insert(m_InsertBlock);
    }

    return Insert(
        where,
        CallInstruction::Create(where, m_InsertBlock->Parent->Location, callee, argument_pairs, landing_pad));
}

mcc::InstructionPtr mcc::Builder::CreateMacro(
    const SourceLocation &where,
    const std::string &name,
    const std::vector<ValuePtr> &arguments) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    return Insert(where, MacroInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, name, arguments));
}

mcc::ValuePtr mcc::Builder::Allocate(
    const SourceLocation &where,
    const TypePtr &type,
    const bool is_mutable) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    const auto location = m_InsertBlock->Parent->Location;
    const auto index = m_InsertBlock->Parent->StackIndex++;

    return GenericStorageReference::Create(where, type, location, std::format("stack[0].val[{}]", index), is_mutable);
}

mcc::InstructionPtr mcc::Builder::CreateAppend(
    const SourceLocation &where,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool force) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");
    Assert(force || array->IsMutable, where, "array must be mutable");

    return Insert(
        where,
        ArrayInstruction::CreateAppend(where, m_Context, m_InsertBlock->Parent->Location, array, value));
}

mcc::InstructionPtr mcc::Builder::CreatePrepend(
    const SourceLocation &where,
    const ValuePtr &array,
    const ValuePtr &value) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");
    Assert(array->IsMutable, where, "array must be mutable");

    return Insert(
        where,
        ArrayInstruction::CreatePrepend(where, m_Context, m_InsertBlock->Parent->Location, array, value));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    const ValuePtr &array,
    const SourceLocation &where,
    const ValuePtr &value,
    const IndexT index) const
{
    Assert(!!array, where, "array must not be null");
    Assert(!!value, where, "value must not be null");
    Assert(array->IsMutable, where, "array must be mutable");

    return Insert(
        where,
        ArrayInstruction::CreateInsert(where, m_Context, m_InsertBlock->Parent->Location, array, value, index));
}

mcc::InstructionPtr mcc::Builder::CreateInsert(
    const SourceLocation &where,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key,
    const bool force) const
{
    Assert(!!object, where, "object must not be null");
    Assert(!!value, where, "value must not be null");
    Assert(!key.empty(), where, "key must not be empty");
    Assert(force || object->IsMutable, where, "object must be mutable");

    return Insert(
        where,
        ObjectInstruction::CreateInsert(where, m_Context, m_InsertBlock->Parent->Location, object, value, key));
}

mcc::ValuePtr mcc::Builder::CreateStoreResult(const SourceLocation &where, const TypePtr &type, const std::string &name)
{
    Assert(!!type, where, "type must not be null");
    Assert(!name.empty(), where, "name must not be empty");

    const auto result = FunctionResult::Create(where, type, m_InsertBlock->Parent->Location);
    return CreateVariable(where, type, name, false, result);
}

mcc::InstructionPtr mcc::Builder::CreateNotNull(const SourceLocation &where, const ValuePtr &value) const
{
    Assert(!!value, where, "value must not be null");

    return Insert(where, NotNullInstruction::Create(where, m_Context, m_InsertBlock->Parent->Location, value));
}

mcc::InstructionPtr mcc::Builder::CreateDelete(const SourceLocation &where, const ValuePtr &value) const
{
    Assert(!!value, where, "value must not be null");
    Assert(value->IsMutable, where, "value must be mutable");

    return Insert(where, DeleteInstruction::Create(where, m_Context, value));
}

mcc::InstructionPtr mcc::Builder::Insert(const SourceLocation &where, const InstructionPtr &instruction) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!instruction, where, "instruction must not be null");

    return m_InsertBlock->Instructions.emplace_back(instruction);
}

void mcc::Builder::Generate() const
{
    for (auto &functions : m_Functions | std::views::values)
        for (auto &function : functions | std::views::values)
        {
            function->OptimizeBlocks();
            function->GenerateFunction(m_Package);
        }
}
