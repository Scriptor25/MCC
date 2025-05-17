#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    class Builder
    {
    public:
        explicit Builder(Context &context);

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] const ResourceLocation &GetLocation(const SourceLocation &where) const;

        BlockPtr CreateFunction(
            const SourceLocation &where,
            ResourceLocation location,
            const ParameterList &parameters);
        BlockPtr CreateBlock(const SourceLocation &where, const BlockPtr &parent);
        BlockPtr FindBlock(const ResourceLocation &location) const;
        void RemoveBlock(const SourceLocation &where, const BlockPtr &block);

        void SetInsertBlock(const BlockPtr &block);
        [[nodiscard]] BlockPtr GetInsertBlock() const;
        [[nodiscard]] BlockPtr GetInsertParent(const SourceLocation &where) const;

        [[nodiscard]] ValuePtr GetVariable(const SourceLocation &where, const std::string &name) const;

        [[nodiscard]] InstructionPtr CreateStore(
            const SourceLocation &where,
            const ValuePtr &dst,
            const ValuePtr &src) const;

        [[nodiscard]] InstructionPtr CreateComparison(
            const SourceLocation &where,
            ComparatorE comparator,
            const ValuePtr &left,
            const ValuePtr &right) const;

        [[nodiscard]] InstructionPtr CreateOperation(
            const SourceLocation &where,
            OperatorE operator_,
            const std::vector<ValuePtr> &operands) const;

        [[nodiscard]] InstructionPtr CreateCommand(const SourceLocation &where, const CommandT &command) const;

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

        [[nodiscard]] ValuePtr CreateBranchResult(const SourceLocation &where, TypeID type) const;

        [[nodiscard]] InstructionPtr CreateCall(
            const SourceLocation &where,
            const ResourceLocation &callee,
            const std::vector<ValuePtr> &arguments,
            const BlockPtr &landing_pad) const;

        [[nodiscard]] InstructionPtr AllocateValue(const SourceLocation &where) const;
        [[nodiscard]] InstructionPtr AllocateArray(const SourceLocation &where) const;
        [[nodiscard]] InstructionPtr AllocateObject(const SourceLocation &where) const;

        [[nodiscard]] InstructionPtr CreateAppend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify) const;
        [[nodiscard]] InstructionPtr CreatePrepend(
            const SourceLocation &where,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify) const;
        [[nodiscard]] InstructionPtr CreateInsert(
            const ValuePtr &array,
            const SourceLocation &where,
            const ValuePtr &value,
            IndexT index,
            bool stringify) const;
        [[nodiscard]] InstructionPtr CreateExtract(
            const SourceLocation &where,
            const ValuePtr &array,
            IndexT index) const;

        [[nodiscard]] InstructionPtr CreateInsert(
            const SourceLocation &where,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key) const;

        [[nodiscard]] ValuePtr CreateStoreResult(const SourceLocation &where, TypeID type, const std::string &variable) const;

        [[nodiscard]] InstructionPtr Insert(const SourceLocation &where, InstructionPtr instruction) const;

        void Generate() const;

    private:
        Context &m_Context;

        std::vector<BlockPtr> m_Blocks;
        BlockPtr m_InsertBlock;
    };
}
