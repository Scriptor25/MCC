#pragma once

#include <mcc/common.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Instruction : Value
    {
        Instruction(const SourceLocation &where, TypeContext &context, const TypePtr &type);

        [[nodiscard]] virtual bool IsTerminator() const;

        [[nodiscard]] std::string GetStackPath() const;
        [[nodiscard]] std::string GetTmpName() const;

        [[nodiscard]] std::string CreateTmpScore() const;
        [[nodiscard]] std::string RemoveTmpScore() const;
    };

    struct AllocationInstruction final : Instruction
    {
        static InstructionPtr CreateValue(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            IndexT index);
        static InstructionPtr CreateArray(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            IndexT index);
        static InstructionPtr CreateObject(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            IndexT index);

        AllocationInstruction(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
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
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        static InstructionPtr CreatePrepend(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            bool stringify);
        static InstructionPtr CreateInsert(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &array,
            const ValuePtr &value,
            IndexT index,
            bool stringify);

        ArrayInstruction(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            ArrayOperationE array_operation,
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify);
        ~ArrayInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

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
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &then_target,
            const BlockPtr &else_target);

        BranchInstruction(
            const SourceLocation &where,
            TypeContext &context,
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
            TypeContext &context,
            const ResourceLocation &location,
            const FunctionPtr &callee,
            const std::vector<std::pair<std::string, ValuePtr>> &arguments,
            const BlockPtr &landing_pad);

        CallInstruction(
            const SourceLocation &where,
            TypeContext &context,
            ResourceLocation location,
            const FunctionPtr &callee,
            const std::vector<std::pair<std::string, ValuePtr>> &arguments,
            BlockPtr landing_pad);
        ~CallInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        FunctionPtr Callee;
        std::vector<std::pair<std::string, ValuePtr>> Arguments;
        BlockPtr LandingPad;
    };

    struct CommandInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            const CommandT &command);

        CommandInstruction(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            const CommandT &command);

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
            TypeContext &context,
            const ComparatorE &comparator,
            const ResourceLocation &location,
            const ValuePtr &left,
            const ValuePtr &right);

        ComparisonInstruction(
            const SourceLocation &where,
            TypeContext &context,
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

    struct DeleteInstruction final : Instruction
    {
        static InstructionPtr Create(const SourceLocation &where, TypeContext &context, const ValuePtr &value);

        DeleteInstruction(const SourceLocation &where, TypeContext &context, const ValuePtr &value);
        ~DeleteInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ValuePtr Value;
    };

    struct DirectBranchInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const BlockPtr &target);
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const BlockPtr &target,
            const ValuePtr &result,
            const ValuePtr &branch_result);

        DirectBranchInstruction(
            const SourceLocation &where,
            TypeContext &context,
            ResourceLocation location,
            BlockPtr target,
            ValuePtr result,
            ValuePtr branch_result);
        ~DirectBranchInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        BlockPtr Target;
        ValuePtr Result, BranchResult;
    };

    struct MacroInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const std::string &name,
            const std::vector<ValuePtr> &arguments);

        MacroInstruction(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const std::string &name,
            const std::vector<ValuePtr> &arguments);
        ~MacroInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        std::string Name;
        std::vector<ValuePtr> Arguments;
    };

    struct NotNullInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &value);

        NotNullInstruction(
            const SourceLocation &where,
            TypeContext &context,
            ResourceLocation location,
            ValuePtr value);
        ~NotNullInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        ValuePtr Value;
    };

    struct ObjectInstruction final : Instruction
    {
        static InstructionPtr CreateInsert(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &object,
            const ValuePtr &value,
            const std::string &key);

        ObjectInstruction(
            const SourceLocation &where,
            TypeContext &context,
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
            TypeContext &context,
            OperatorE operator_,
            const ResourceLocation &location,
            const std::vector<ValuePtr> &operands);

        OperationInstruction(
            const SourceLocation &where,
            TypeContext &context,
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
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &value);

        ReturnInstruction(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &value);
        ~ReturnInstruction() override;

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        ResourceLocation Location;
        ValuePtr Value;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ValuePtr &dst,
            const ValuePtr &src);

        StoreInstruction(const SourceLocation &where, TypeContext &context, const ValuePtr &dst, const ValuePtr &src);
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
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &condition,
            const BlockPtr &default_target,
            const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets);

        SwitchInstruction(
            const SourceLocation &where,
            TypeContext &context,
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
            TypeContext &context,
            const ResourceLocation &location,
            const ValuePtr &value,
            const BlockPtr &landing_pad);

        ThrowInstruction(
            const SourceLocation &where,
            TypeContext &context,
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
