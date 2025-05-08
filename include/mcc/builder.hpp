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
        BlockPtr GetInsertBlock() const;
        BlockPtr GetInsertParent() const;

        [[nodiscard]] InstructionPtr CreateStore(ValuePtr dst, ValuePtr src, bool inline_) const;

        [[nodiscard]] InstructionPtr CreateComparison(
            ComparatorE comparator,
            ValuePtr left,
            ValuePtr right,
            bool inline_) const;

        [[nodiscard]] InstructionPtr CreateOperation(
            OperatorE operator_,
            ValuePtr left,
            ValuePtr right,
            bool inline_) const;

        [[nodiscard]] InstructionPtr CreateCommand(CommandT command, bool inline_) const;

        [[nodiscard]] InstructionPtr CreateReturn(ValuePtr value, bool inline_) const;
        [[nodiscard]] InstructionPtr CreateBranch(
            ValuePtr condition,
            ValuePtr then_target,
            ValuePtr else_target,
            bool inline_) const;
        [[nodiscard]] InstructionPtr CreateDirect(ValuePtr target, bool inline_) const;

        [[nodiscard]] InstructionPtr CreateCall(
            std::string callee,
            bool builtin,
            std::vector<ValuePtr> arguments,
            bool inline_) const;

        [[nodiscard]] InstructionPtr AllocateValue(bool inline_) const;
        [[nodiscard]] InstructionPtr AllocateArray(bool inline_) const;
        [[nodiscard]] InstructionPtr AllocateObject(bool inline_) const;

        [[nodiscard]] InstructionPtr CreateAppend(
            ValuePtr array,
            ValuePtr value,
            bool stringify,
            bool inline_) const;
        [[nodiscard]] InstructionPtr CreatePrepend(
            ValuePtr array,
            ValuePtr value,
            bool stringify,
            bool inline_) const;
        [[nodiscard]] InstructionPtr CreateInsert(
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify,
            bool inline_) const;

        [[nodiscard]] InstructionPtr CreateInsert(
            ValuePtr object,
            ValuePtr value,
            std::string key,
            bool inline_) const;

        [[nodiscard]] InstructionPtr Insert(InstructionPtr instruction, bool inline_) const;

        void Generate() const;

    private:
        Context &m_Context;

        std::vector<BlockPtr> m_Blocks;
        BlockPtr m_InsertBlock;
    };
}
