#pragma once

#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Result
    {
        ResultTypeE Type;

        /* Constant */
        std::string Value;

        /* Storage */
        ResourceLocation Location;
        std::string Path;

        /* Score */
        std::string Player;
        std::string Objective;
    };

    struct Value
    {
        virtual ~Value() = default;

        virtual void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const;
        [[nodiscard]] virtual CommandT GenerateInline(const Builder &builder, bool use_stack) const;
        [[nodiscard]] virtual Result GenerateResult(bool stringify, bool use_stack) const;
        [[nodiscard]] virtual bool RequireStack() const = 0;

        void Use();
        void Drop();

        IndexT UseCount = 0;
    };

    struct Constant : Value
    {
        [[nodiscard]] bool RequireStack() const override;
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(bool value);

        explicit ConstantBoolean(bool value);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        bool Value;
    };

    struct ConstantInteger final : Constant
    {
        static ConstantPtr Create(IntegerT value);

        explicit ConstantInteger(IntegerT value);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        IntegerT Value;
    };

    struct ConstantFloat final : Constant
    {
        static ConstantPtr Create(FloatT value);

        explicit ConstantFloat(FloatT value);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        FloatT Value;
    };

    struct ConstantFloatRange final : Constant
    {
        static ConstantPtr Create(FloatT min, FloatT max);
        static ConstantPtr CreateMin(FloatT min);
        static ConstantPtr CreateMax(FloatT max);

        ConstantFloatRange(std::optional<FloatT> min, std::optional<FloatT> max);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        std::optional<FloatT> Min;
        std::optional<FloatT> Max;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(std::string value);

        explicit ConstantString(std::string value);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        std::string Value;
    };

    struct ConstantArray final : Constant
    {
        static ConstantPtr Create(std::vector<ConstantPtr> values, bool stringify);

        ConstantArray(std::vector<ConstantPtr> values, bool stringify);
        ~ConstantArray() override;

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        std::vector<ConstantPtr> Values;
        bool Stringify;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(std::map<std::string, ConstantPtr> values);

        explicit ConstantObject(std::map<std::string, ConstantPtr> values);
        ~ConstantObject() override;

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        std::map<std::string, ConstantPtr> Values;
    };

    struct ConstantTarget final : Constant
    {
        static ConstantPtr Create(
            TargetSelectorE selector,
            std::map<std::string, std::vector<TargetAttributePtr>> attributes);

        ConstantTarget(
            TargetSelectorE selector,
            std::map<std::string, std::vector<TargetAttributePtr>> attributes);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        TargetSelectorE Selector;
        std::map<std::string, std::vector<TargetAttributePtr>> Attributes;
    };

    struct ConstantResource final : Constant
    {
        static ConstantPtr Create(ResourceLocation location);

        explicit ConstantResource(ResourceLocation location);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        ResourceLocation Location;
    };

    struct ConstantOffset final : Constant
    {
        static ConstantPtr Create(OffsetTypeE type, FloatT offset);

        ConstantOffset(OffsetTypeE type, FloatT offset);

        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;

        OffsetTypeE Type;
        FloatT Offset;
    };

    struct Instruction : Value
    {
        [[nodiscard]] std::string GetStackPath() const;
        [[nodiscard]] std::string GetTmpName() const;

        [[nodiscard]] std::string CreateTmpScore() const;
        [[nodiscard]] std::string RemoveTmpScore() const;
    };

    struct CommandInstruction final : Instruction
    {
        static InstructionPtr Create(
            ResourceLocation location,
            std::vector<std::string> path,
            std::vector<ValuePtr> arguments);

        CommandInstruction(ResourceLocation location, std::vector<std::string> path, std::vector<ValuePtr> arguments);
        ~CommandInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] CommandT GenerateInline(const Builder &builder, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        std::vector<std::string> Path;
        std::vector<ValuePtr> Arguments;
    };

    struct ReturnInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, ValuePtr value);

        ReturnInstruction(ResourceLocation location, ValuePtr value);
        ~ReturnInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        ValuePtr Value;
    };

    struct IfUnlessInstruction final : Instruction
    {
        static InstructionPtr Create(
            ResourceLocation location,
            bool unless,
            ValuePtr condition,
            ValuePtr then,
            ValuePtr else_);

        IfUnlessInstruction(ResourceLocation location, bool unless, ValuePtr condition, ValuePtr then, ValuePtr else_);
        ~IfUnlessInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        bool Unless;
        ValuePtr Condition, Then, Else;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(ValuePtr dst, ValuePtr src);

        StoreInstruction(ValuePtr dst, ValuePtr src);
        ~StoreInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ValuePtr Dst, Src;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);

        ComparisonInstruction(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);
        ~ComparisonInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ComparatorE Comparator;
        ResourceLocation Location;
        ValuePtr Left, Right;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(OperatorE operator_, ResourceLocation location, ValuePtr left, ValuePtr right);

        OperationInstruction(OperatorE operator_, ResourceLocation location, ValuePtr left, ValuePtr right);
        ~OperationInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        OperatorE Operator;
        ResourceLocation Location;
        ValuePtr Left, Right;
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

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

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

        ArrayInstruction(
            ArrayOperationE array_operation,
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            IndexT index,
            bool stringify);
        ~ArrayInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ArrayOperationE ArrayOperation;
        ResourceLocation Location;
        ValuePtr Array, Value;
        IndexT Index;
        bool Stringify;
    };

    struct ObjectInstruction final : Instruction
    {
        static InstructionPtr CreateInsert(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);

        ObjectInstruction(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);
        ~ObjectInstruction() override;

        void Generate(const Builder &builder, CommandVector &commands, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        ValuePtr Object, Value;
        std::string Key;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(ResourceLocation location, std::string id);

        NamedValue(ResourceLocation location, std::string id);

        [[nodiscard]] CommandT GenerateInline(const Builder &builder, bool use_stack) const override;
        [[nodiscard]] Result GenerateResult(bool stringify, bool use_stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        ResourceLocation Location;
        std::string ID;
    };
}
