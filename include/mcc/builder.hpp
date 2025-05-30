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

        [[nodiscard]] bool HasFunction(ResourceLocation location) const;
        FunctionPtr CreateFunction(
            const SourceLocation &where,
            ResourceLocation location,
            const ParameterList &parameters,
            const TypePtr &result,
            bool throws);
        [[nodiscard]] FunctionPtr GetFunction(const SourceLocation &where, ResourceLocation location) const;

        void SetInsertBlock(const BlockPtr &block);
        [[nodiscard]] BlockPtr GetInsertBlock() const;

        void PushVariables();
        void PopVariables();

        ValuePtr CreateVariable(
            const SourceLocation &where,
            const TypePtr &type,
            const std::string &name,
            bool is_mutable,
            const ValuePtr &initializer);
        ValuePtr InsertVariable(const SourceLocation &where, const std::string &name, const ValuePtr &value);
        [[nodiscard]] ValuePtr GetVariable(const SourceLocation &where, const std::string &name) const;

        [[nodiscard]] InstructionPtr CreateStore(
            const SourceLocation &where,
            const ValuePtr &dst,
            const ValuePtr &src,
            bool force = false) const;

        [[nodiscard]] InstructionPtr CreateComparison(
            const SourceLocation &where,
            ComparatorE comparator,
            const ValuePtr &left,
            const ValuePtr &right) const;

        [[nodiscard]] InstructionPtr CreateOperation(
            const SourceLocation &where,
            OperatorE operator_,
            const std::vector<ValuePtr> &operands) const;

        [[nodiscard]] InstructionPtr CreateCommand(
            const SourceLocation &where,
            const TypePtr &type,
            const CommandT &command) const;

        [[nodiscard]] InstructionPtr CreateReturnVoid(const SourceLocation &where) const;
        [[nodiscard]] InstructionPtr CreateReturn(const SourceLocation &where, const ValuePtr &value) const;

        [[nodiscard]] InstructionPtr CreateBranch(
            const SourceLocation &where,
            const ValuePtr &condition,
            const BlockPtr &then_target,
            const BlockPtr &else_target) const;

        [[nodiscard]] InstructionPtr CreateDirect(const SourceLocation &where, const BlockPtr &target) const;
        [[nodiscard]] InstructionPtr CreateDirect(
            const SourceLocation &where,
            const BlockPtr &target,
            const ValuePtr &result,
            const ValuePtr &branch_result) const;

        [[nodiscard]] InstructionPtr CreateSwitch(
            const SourceLocation &where,
            const ValuePtr &condition,
            const BlockPtr &default_target,
            const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets) const;

        [[nodiscard]] InstructionPtr CreateThrow(
            const SourceLocation &where,
            const ValuePtr &value,
            const BlockPtr &landing_pad) const;

        [[nodiscard]] ValuePtr CreateBranchResult(const SourceLocation &where, const TypePtr &type) const;

        [[nodiscard]] InstructionPtr CreateCall(
            const SourceLocation &where,
            const FunctionPtr &callee,
            const std::vector<ValuePtr> &arguments,
            const BlockPtr &landing_pad) const;
        [[nodiscard]] InstructionPtr CreateMacro(
            const SourceLocation &where,
            const std::string &name,
            const std::vector<ValuePtr> &arguments) const;

        [[nodiscard]] ValuePtr Allocate(
            const SourceLocation &where,
            const TypePtr &type,
            bool is_mutable) const;

        [[nodiscard]] InstructionPtr CreateAppend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value) const;
        [[nodiscard]] InstructionPtr CreatePrepend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value) const;
        [[nodiscard]] InstructionPtr CreateInsert(
            const ValuePtr &array,
            const SourceLocation &where,
            const ValuePtr &value,
            IndexT index) const;

        [[nodiscard]] InstructionPtr CreateInsert(
            const SourceLocation &where,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key) const;

        ValuePtr CreateStoreResult(const SourceLocation &where, const TypePtr &type, const std::string &name);

        [[nodiscard]] InstructionPtr CreateNotNull(const SourceLocation &where, const ValuePtr &value) const;
        [[nodiscard]] InstructionPtr CreateDelete(const SourceLocation &where, const ValuePtr &value) const;

        [[nodiscard]] InstructionPtr Insert(const SourceLocation &where, const InstructionPtr &instruction) const;

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
