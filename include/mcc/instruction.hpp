#pragma once

#include <mcc/common.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Instruction : Value
    {
        [[nodiscard]] virtual bool IsTerminator() const;

        [[nodiscard]] std::string GetStackPath() const;
        [[nodiscard]] std::string GetTmpName() const;

        [[nodiscard]] std::string CreateTmpScore() const;
        [[nodiscard]] std::string RemoveTmpScore() const;
    };

    struct AllocationInstruction final : Instruction
    {
        static InstructionPtr CreateValue(ResourceLocation location, IndexT index);
        static InstructionPtr CreateArray(ResourceLocation location, IndexT index);
        static InstructionPtr CreateObject(ResourceLocation location, IndexT index);

        AllocationInstruction(
            AllocationTypeE allocation_type,
            ResourceLocation location,
            IndexT index);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        AllocationTypeE AllocationType;
        ResourceLocation Location;
        IndexT Index;
    };

    struct ArrayInstruction final : Instruction
    {
        static InstructionPtr CreateAppend(
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            bool stringify);
        static InstructionPtr CreatePrepend(
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            bool stringify);
        static InstructionPtr CreateInsert(
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify);
        static InstructionPtr CreateExtract(
            ResourceLocation location,
            ValuePtr array,
            IndexT index);

        ArrayInstruction(
            ArrayOperationE array_operation,
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify);
        ~ArrayInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ArrayOperationE ArrayOperation;
        ResourceLocation Location;
        ValuePtr Array, Value;
        IndexT Index;
        bool Stringify;
    };

    struct BranchInstruction final : Instruction
    {
        static InstructionPtr Create(
            ResourceLocation location,
            ValuePtr condition,
            BlockPtr then_target,
            BlockPtr else_target);

        BranchInstruction(
            ResourceLocation location,
            ValuePtr condition,
            BlockPtr then_target,
            BlockPtr else_target);
        ~BranchInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Condition;
        BlockPtr ThenTarget, ElseTarget;
    };

    struct CallInstruction final : Instruction
    {
        static InstructionPtr Create(
            ResourceLocation location,
            std::string callee,
            bool builtin,
            std::vector<ValuePtr> arguments);

        CallInstruction(ResourceLocation location, std::string callee, bool builtin, std::vector<ValuePtr> arguments);
        ~CallInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        std::string Callee;
        bool Builtin;
        std::vector<ValuePtr> Arguments;
    };

    struct CommandInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, CommandT command);

        CommandInstruction(ResourceLocation location, CommandT command);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        CommandT Command;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);

        ComparisonInstruction(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);
        ~ComparisonInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ComparatorE Comparator;
        ResourceLocation Location;
        ValuePtr Left, Right;
    };

    struct DirectInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, BlockPtr target);
        static InstructionPtr Create(
            ResourceLocation location,
            BlockPtr target,
            ValuePtr result,
            ValuePtr branch_result);

        DirectInstruction(ResourceLocation location, BlockPtr target, ValuePtr result, ValuePtr branch_result);
        ~DirectInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        BlockPtr Target;
        ValuePtr Result, BranchResult;
    };

    struct ObjectInstruction final : Instruction
    {
        static InstructionPtr CreateInsert(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);

        ObjectInstruction(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);
        ~ObjectInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        ValuePtr Object, Value;
        std::string Key;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(OperatorE operator_, ResourceLocation location, std::vector<ValuePtr> operands);

        OperationInstruction(OperatorE operator_, ResourceLocation location, std::vector<ValuePtr> operands);
        ~OperationInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        OperatorE Operator;
        ResourceLocation Location;
        std::vector<ValuePtr> Operands;
    };

    struct ReturnInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, ValuePtr value);

        ReturnInstruction(ResourceLocation location, ValuePtr value);
        ~ReturnInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Value;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(ValuePtr dst, ValuePtr src);

        StoreInstruction(ValuePtr dst, ValuePtr src);
        ~StoreInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ValuePtr Dst, Src;
    };

    struct SwitchInstruction final : Instruction
    {
        static InstructionPtr Create(
            ResourceLocation location,
            ValuePtr condition,
            BlockPtr default_target,
            std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets);

        SwitchInstruction(
            ResourceLocation location,
            ValuePtr condition,
            BlockPtr default_target,
            std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets);
        ~SwitchInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Condition;
        BlockPtr DefaultTarget;
        std::vector<std::pair<ConstantPtr, BlockPtr>> CaseTargets;
    };

    struct ThrowInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, ValuePtr value);

        ThrowInstruction(ResourceLocation location, ValuePtr value);
        ~ThrowInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Value;
    };
}
