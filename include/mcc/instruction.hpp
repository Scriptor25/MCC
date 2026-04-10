#pragma once

#include <mcc/common.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Instruction : Value<Instruction>
    {
        Instruction(
                const SourceLocation &where,
                const std::string &name,
                const TypePtr &type,
                FieldType_ field_type);

        [[nodiscard]] virtual bool IsTerminator() const;

        [[nodiscard]] std::string GetStackPath() const;
        [[nodiscard]] std::string GetTemp() const;

        [[nodiscard]] std::string CreateScore() const;
        [[nodiscard]] std::string RemoveScore() const;
    };

    struct ArrayInstruction final : Instruction
    {
        static InstructionPtr CreateAppend(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const ValuePtr &array,
                const ValuePtr &value);
        static InstructionPtr CreatePrepend(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const ValuePtr &array,
                const ValuePtr &value);
        static InstructionPtr CreateInsert(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const ValuePtr &array,
                const ValuePtr &value,
                IndexT index);

        static SPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                ArrayOperation_ array_operation,
                const ValuePtr &array,
                const ValuePtr &value,
                IndexT index);

        ArrayInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                ArrayOperation_ array_operation,
                ValuePtr array,
                ValuePtr value,
                IndexT index);
        ~ArrayInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        ArrayOperation_ ArrayOperation;
        ValuePtr Array, Value;
        IndexT Index;
    };

    struct BranchInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const ValuePtr &condition,
                const BlockPtr &then_target,
                const BlockPtr &else_target);

        BranchInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                ValuePtr condition,
                BlockPtr then_target,
                BlockPtr else_target);
        ~BranchInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        FunctionPtr Parent;
        ValuePtr Condition;
        BlockPtr ThenTarget, ElseTarget;
    };

    struct CallInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                const FunctionPtr &parent,
                const FunctionPtr &callee,
                const std::vector<std::pair<
                        std::string,
                        ValuePtr
                >> &arguments,
                const BlockPtr &landing_pad);

        CallInstruction(
                const SourceLocation &where,
                const std::string &name,
                FunctionPtr parent,
                const FunctionPtr &callee,
                const std::vector<std::pair<
                        std::string,
                        ValuePtr
                >> &arguments,
                BlockPtr landing_pad);
        ~CallInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        FunctionPtr Parent;
        FunctionPtr Callee;
        std::vector<std::pair<std::string, ValuePtr>> Arguments;
        BlockPtr LandingPad;
    };

    struct CommandInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                const TypePtr &type,
                const FunctionPtr &parent,
                const CommandT &command);

        CommandInstruction(
                const SourceLocation &where,
                const std::string &name,
                const TypePtr &type,
                FunctionPtr parent,
                CommandT command);

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        FunctionPtr Parent;
        CommandT Command;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const Comparator_ &comparator,
                const FunctionPtr &parent,
                const ValuePtr &left,
                const ValuePtr &right);

        ComparisonInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                Comparator_ comparator,
                FunctionPtr parent,
                ValuePtr left,
                ValuePtr right);
        ~ComparisonInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        Comparator_ Comparator;
        FunctionPtr Parent;
        ValuePtr Left, Right;
    };

    struct DeleteInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const ValuePtr &value);

        DeleteInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                ValuePtr value);
        ~DeleteInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        ValuePtr Value;
    };

    struct DirectBranchInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const BlockPtr &target);
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const BlockPtr &target,
                const ValuePtr &result,
                const ValuePtr &branch_result);

        DirectBranchInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                BlockPtr target,
                ValuePtr result,
                ValuePtr branch_result);
        ~DirectBranchInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        FunctionPtr Parent;
        BlockPtr Target;
        ValuePtr Result, BranchResult;
    };

    struct MacroInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const std::string &macro,
                const std::vector<ValuePtr> &arguments);

        MacroInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                std::string macro,
                const std::vector<ValuePtr> &arguments);
        ~MacroInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        FunctionPtr Parent;
        std::string Macro;
        std::vector<ValuePtr> Arguments;
    };

    struct NotNullInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const ValuePtr &value);

        NotNullInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                ValuePtr value);
        ~NotNullInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        FunctionPtr Parent;
        ValuePtr Value;
    };

    struct ObjectInstruction final : Instruction
    {
        static InstructionPtr CreateInsert(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const ValuePtr &object,
                const ValuePtr &value,
                const std::string &key);

        ObjectInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                ValuePtr object,
                ValuePtr value,
                std::string key);
        ~ObjectInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        ValuePtr Object, Value;
        std::string Key;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                Operator_ operator_,
                const FunctionPtr &parent,
                const std::vector<ValuePtr> &operands);

        OperationInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                Operator_ operator_,
                FunctionPtr parent,
                const std::vector<ValuePtr> &operands);
        ~OperationInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        Operator_ Operator;
        FunctionPtr Parent;
        std::vector<ValuePtr> Operands;
    };

    struct ReturnInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const ValuePtr &value);

        ReturnInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                ValuePtr value);
        ~ReturnInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        FunctionPtr Parent;
        ValuePtr Value;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                const ValuePtr &dst,
                const ValuePtr &src);

        StoreInstruction(
                const SourceLocation &where,
                const std::string &name,
                const ValuePtr &dst,
                ValuePtr src);
        ~StoreInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Dst, Src;
    };

    struct SwitchInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const ValuePtr &condition,
                const BlockPtr &default_target,
                const CaseTargetMap &case_targets);

        SwitchInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                ValuePtr condition,
                BlockPtr default_target,
                CaseTargetMap case_targets);
        ~SwitchInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        void Replace(
                ValuePtr value,
                ValuePtr replacement) override;

        FunctionPtr Parent;
        ValuePtr Condition;
        BlockPtr DefaultTarget;
        CaseTargetMap CaseTargets;
    };

    struct ThrowInstruction final : Instruction
    {
        static InstructionPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent,
                const ValuePtr &value,
                const BlockPtr &landing_pad);

        ThrowInstruction(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                FunctionPtr parent,
                ValuePtr value,
                BlockPtr landing_pad);
        ~ThrowInstruction() override;

        void Generate(
                CommandVector &commands,
                bool stack) const override;

        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] bool IsTerminator() const override;

        FunctionPtr Parent;
        ValuePtr Value;
        BlockPtr LandingPad;
    };
}
