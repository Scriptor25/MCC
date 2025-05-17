#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Instruction : Value
    {
        explicit Instruction(const SourceLocation &where);

        [[nodiscard]] virtual bool IsTerminator() const;

        [[nodiscard]] std::string GetStackPath() const;
        [[nodiscard]] std::string GetTmpName() const;

        [[nodiscard]] std::string CreateTmpScore() const;
        [[nodiscard]] std::string RemoveTmpScore() const;
    };

    struct AllocationInstruction final : Instruction
    {
        static InstructionPtr CreateValue(const SourceLocation &where, const ResourceLocation &location, IndexT index);
        static InstructionPtr CreateArray(const SourceLocation &where, const ResourceLocation &location, IndexT index);
        static InstructionPtr CreateObject(const SourceLocation &where, const ResourceLocation &location, IndexT index);

        AllocationInstruction(
            const SourceLocation &where,
            AllocationTypeE allocation_type,
            const ResourceLocation &location,
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
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        static InstructionPtr CreatePrepend(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        static InstructionPtr CreateInsert(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            IndexT index,
            bool stringify);
        static InstructionPtr CreateExtract(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &array,
            IndexT index);

        ArrayInstruction(
            const SourceLocation &where,
            ArrayOperationE array_operation,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
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
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &then_target,
            const BlockPtr &else_target);

        BranchInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &then_target,
            const BlockPtr &else_target);
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
            const SourceLocation &where,
            const ResourceLocation &location,
            const ResourceLocation &callee,
            const std::vector<ValuePtr> &arguments,
            bool may_throw,
            const BlockPtr &landing_pad);

        CallInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ResourceLocation &callee,
            const std::vector<ValuePtr> &arguments,
            bool may_throw,
            const BlockPtr &landing_pad);
        ~CallInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        ResourceLocation Callee;
        std::vector<ValuePtr> Arguments;
        bool MayThrow;
        BlockPtr LandingPad;
    };

    struct CommandInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const CommandT &command);

        CommandInstruction(const SourceLocation &where, const ResourceLocation &location, const CommandT &command);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        CommandT Command;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            const ComparatorE &comparator,
            const ResourceLocation &location,
            const ValuePtr &left,
            const ValuePtr &right);

        ComparisonInstruction(
            const SourceLocation &where,
            ComparatorE comparator,
            const ResourceLocation &location,
            const ValuePtr &left,
            const ValuePtr &right);
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
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const BlockPtr &target);
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const BlockPtr &target,
            const ValuePtr &result,
            const ValuePtr &branch_result);

        DirectInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const BlockPtr &target,
            const ValuePtr &result,
            const ValuePtr &branch_result);
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
        static InstructionPtr CreateInsert(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key);

        ObjectInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key);
        ~ObjectInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        ValuePtr Object, Value;
        std::string Key;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            OperatorE operator_,
            const ResourceLocation &location,
            const std::vector<ValuePtr> &operands);

        OperationInstruction(
            const SourceLocation &where,
            OperatorE operator_,
            const ResourceLocation &location,
            const std::vector<ValuePtr> &operands);
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
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &value);

        ReturnInstruction(const SourceLocation &where, const ResourceLocation &location, const ValuePtr &value);
        ~ReturnInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Value;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(const SourceLocation &where, const ValuePtr &dst, const ValuePtr &src);

        StoreInstruction(const SourceLocation &where, const ValuePtr &dst, const ValuePtr &src);
        ~StoreInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ValuePtr Dst, Src;
    };

    struct SwitchInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &default_target,
            const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets);

        SwitchInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &default_target,
            const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets);
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
        static InstructionPtr Create(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &value,
            const BlockPtr &landing_pad);

        ThrowInstruction(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ValuePtr &value,
            const BlockPtr &landing_pad);
        ~ThrowInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Value;
        BlockPtr LandingPad;
    };
}
