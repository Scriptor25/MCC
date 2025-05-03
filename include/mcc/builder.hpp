#pragma once

#include <vector>
#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    class Builder
    {
    public:
        explicit Builder(Context &context, const ResourceLocation &location);

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] const ResourceLocation &GetLocation() const;

        InstructionPtr CreateStore(ValuePtr dst, ValuePtr src, bool inline_);
        InstructionPtr CreateComparison(ComparatorE comparator, ValuePtr left, ValuePtr right, bool inline_);
        InstructionPtr CreateOperation(OperatorE operator_, ValuePtr left, ValuePtr right, bool inline_);
        InstructionPtr CreateCall(CalleeE callee, std::vector<ValuePtr> arguments, bool inline_);
        InstructionPtr CreateReturn(ValuePtr value, bool inline_);
        InstructionPtr CreateIf(bool unless, ValuePtr condition, ValuePtr then, ValuePtr else_, bool inline_);

        InstructionPtr AllocateValue(bool inline_);
        InstructionPtr AllocateArray(bool inline_);
        InstructionPtr AllocateObject(bool inline_);
        InstructionPtr CreateAppend(
            ValuePtr array,
            ValuePtr value,
            bool stringify,
            bool inline_);
        InstructionPtr CreatePrepend(
            ValuePtr array,
            ValuePtr value,
            bool stringify,
            bool inline_);
        InstructionPtr CreateInsert(
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify,
            bool inline_);
        InstructionPtr CreateInsert(
            ValuePtr object,
            ValuePtr value,
            std::string key,
            bool inline_);

        InstructionPtr Insert(InstructionPtr instruction, bool inline_);

        void Gen(std::vector<Command> &commands) const;

    private:
        Context &m_Context;
        const ResourceLocation &m_Location;

        std::vector<InstructionPtr> m_Instructions;

        IndexT m_StackIndex = 0;
    };
}
