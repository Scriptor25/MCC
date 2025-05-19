#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    class Builder
    {
    public:
        Builder(TypeContext &context, Package &package);

        [[nodiscard]] TypeContext &GetContext() const;
        [[nodiscard]] Package &GetPackage() const;
        [[nodiscard]] std::string GetNamespace() const;

        void SetNamespace(const std::string &namespace_);

        [[nodiscard]] bool HasFunction(const ResourceLocation &location) const;
        FunctionPtr CreateFunction(
            const SourceLocation &where,
            ResourceLocation location,
            const ParameterList &parameters,
            const TypePtr &result,
            bool throws);
        FunctionPtr GetFunction(const SourceLocation &where, ResourceLocation location);

        void SetInsertBlock(const BlockPtr &block);
        [[nodiscard]] BlockPtr GetInsertBlock() const;

        void PushVariables();
        void PopVariables();

        ValuePtr CreateVariable(
            const SourceLocation &where,
            const TypePtr &type,
            const std::string &name);
        [[nodiscard]] ValuePtr GetVariable(const SourceLocation &where, const std::string &name) const;

        [[nodiscard]] InstructionPtr CreateStore(
            const SourceLocation &where,
            const ValuePtr &dst,
            const ValuePtr &src);

        [[nodiscard]] InstructionPtr CreateComparison(
            const SourceLocation &where,
            ComparatorE comparator,
            const ValuePtr &left,
            const ValuePtr &right);

        [[nodiscard]] InstructionPtr CreateOperation(
            const SourceLocation &where,
            OperatorE operator_,
            const std::vector<ValuePtr> &operands);

        [[nodiscard]] InstructionPtr CreateCommand(
            const SourceLocation &where,
            const TypePtr &type,
            const CommandT &command);

        [[nodiscard]] InstructionPtr CreateReturnVoid(const SourceLocation &where);
        [[nodiscard]] InstructionPtr CreateReturn(const SourceLocation &where, const ValuePtr &value);

        [[nodiscard]] InstructionPtr CreateBranch(
            const SourceLocation &where,
            const ValuePtr &condition,
            const BlockPtr &then_target,
            const BlockPtr &else_target);

        [[nodiscard]] InstructionPtr CreateDirect(const SourceLocation &where, const BlockPtr &target);
        [[nodiscard]] InstructionPtr CreateDirect(
            const SourceLocation &where,
            const BlockPtr &target,
            const ValuePtr &result,
            const ValuePtr &branch_result);

        [[nodiscard]] InstructionPtr CreateSwitch(
            const SourceLocation &where,
            const ValuePtr &condition,
            const BlockPtr &default_target,
            const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets);

        [[nodiscard]] InstructionPtr CreateThrow(
            const SourceLocation &where,
            const ValuePtr &value,
            const BlockPtr &landing_pad);

        [[nodiscard]] ValuePtr CreateBranchResult(const SourceLocation &where, const TypePtr &type);

        [[nodiscard]] InstructionPtr CreateCall(
            const SourceLocation &where,
            const ResourceLocation &callee,
            const std::vector<ValuePtr> &arguments,
            const BlockPtr &landing_pad);
        [[nodiscard]] InstructionPtr CreateMacro(
            const SourceLocation &where,
            const std::string &name,
            const std::vector<ValuePtr> &arguments);

        [[nodiscard]] InstructionPtr AllocateValue(const SourceLocation &where, const TypePtr &type);
        [[nodiscard]] InstructionPtr AllocateArray(const SourceLocation &where, const TypePtr &type);
        [[nodiscard]] InstructionPtr AllocateObject(const SourceLocation &where, const TypePtr &type);

        [[nodiscard]] InstructionPtr CreateAppend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        [[nodiscard]] InstructionPtr CreatePrepend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        [[nodiscard]] InstructionPtr CreateInsert(
            const ValuePtr &array,
            const SourceLocation &where,
            const ValuePtr &value,
            IndexT index,
            bool stringify);
        [[nodiscard]] InstructionPtr CreateExtract(
            const SourceLocation &where,
            const ValuePtr &array,
            IndexT index);

        [[nodiscard]] InstructionPtr CreateInsert(
            const SourceLocation &where,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key);

        ValuePtr CreateStoreResult(
            const SourceLocation &where,
            const TypePtr &type,
            const std::string &variable);

        [[nodiscard]] InstructionPtr Insert(const SourceLocation &where, InstructionPtr instruction) const;

        void Generate() const;

    private:
        TypeContext &m_Context;
        Package &m_Package;

        std::string m_Namespace;
        std::map<std::string, std::map<std::string, FunctionPtr>> m_Functions;

        BlockPtr m_InsertBlock;
        std::vector<std::map<std::string, ValuePtr>> m_Variables;
    };
}
