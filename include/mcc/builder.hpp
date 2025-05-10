#pragma once

#include <vector>
#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    class Builder
    {
    public:
        explicit Builder(Context &context);

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] const ResourceLocation &GetLocation() const;

        BlockPtr CreateFunction(ResourceLocation location, std::vector<std::string> parameters);
        BlockPtr CreateBlock(const BlockPtr &parent);
        void RemoveBlock(const BlockPtr &block);

        void SetInsertBlock(BlockPtr block);
        [[nodiscard]] BlockPtr GetInsertBlock() const;
        [[nodiscard]] BlockPtr GetInsertParent() const;

        [[nodiscard]] ValuePtr GetVariable(const std::string &name) const;

        [[nodiscard]] InstructionPtr CreateStore(ValuePtr dst, ValuePtr src) const;

        [[nodiscard]] InstructionPtr CreateComparison(ComparatorE comparator, ValuePtr left, ValuePtr right) const;

        [[nodiscard]] InstructionPtr CreateOperation(OperatorE operator_, std::vector<ValuePtr> operands) const;

        [[nodiscard]] InstructionPtr CreateCommand(CommandT command) const;

        [[nodiscard]] InstructionPtr CreateReturnVoid() const;
        [[nodiscard]] InstructionPtr CreateReturn(ValuePtr value) const;
        [[nodiscard]] InstructionPtr CreateBranch(ValuePtr condition, ValuePtr then_target, ValuePtr else_target) const;
        [[nodiscard]] InstructionPtr CreateDirect(ValuePtr target) const;
        [[nodiscard]] InstructionPtr CreateDirect(ValuePtr target, ValuePtr result, ValuePtr landing_pad) const;

        [[nodiscard]] ValuePtr CreateBranchResult() const;

        [[nodiscard]] InstructionPtr CreateCall(
            std::string callee,
            bool builtin,
            std::vector<ValuePtr> arguments) const;

        [[nodiscard]] InstructionPtr AllocateValue() const;
        [[nodiscard]] InstructionPtr AllocateArray() const;
        [[nodiscard]] InstructionPtr AllocateObject() const;

        [[nodiscard]] InstructionPtr CreateAppend(ValuePtr array, ValuePtr value, bool stringify) const;
        [[nodiscard]] InstructionPtr CreatePrepend(ValuePtr array, ValuePtr value, bool stringify) const;
        [[nodiscard]] InstructionPtr CreateInsert(ValuePtr array, ValuePtr value, IndexT index, bool stringify) const;

        [[nodiscard]] InstructionPtr CreateInsert(ValuePtr object, ValuePtr value, std::string key) const;

        [[nodiscard]] InstructionPtr Insert(InstructionPtr instruction) const;

        void Generate() const;

    private:
        Context &m_Context;

        std::vector<BlockPtr> m_Blocks;
        BlockPtr m_InsertBlock;
    };
}
