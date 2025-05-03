#pragma once

#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Value
    {
        virtual ~Value() = default;

        virtual void Gen(std::vector<Command> &commands) const;
        virtual Command GenInline() const = 0;

        bool Invert = false;
    };

    struct Constant : Value
    {
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(bool value);

        explicit ConstantBoolean(bool value);

        Command GenInline() const override;

        bool Value;
    };

    struct ConstantInteger final : Constant
    {
        static ConstantPtr Create(IntegerT value);

        explicit ConstantInteger(IntegerT value);

        Command GenInline() const override;

        IntegerT Value;
    };

    struct ConstantFloat final : Constant
    {
        static ConstantPtr Create(FloatT value);

        explicit ConstantFloat(FloatT value);

        Command GenInline() const override;

        FloatT Value;
    };

    struct ConstantFloatRange final : Constant
    {
        static ConstantPtr Create(FloatT min, FloatT max);
        static ConstantPtr CreateMin(FloatT min);
        static ConstantPtr CreateMax(FloatT max);

        ConstantFloatRange(std::optional<FloatT> min, std::optional<FloatT> max);

        Command GenInline() const override;

        std::optional<FloatT> Min;
        std::optional<FloatT> Max;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(std::string value);

        explicit ConstantString(std::string value);

        Command GenInline() const override;

        std::string Value;
    };

    struct ConstantArray final : Constant
    {
        static ConstantPtr Create(std::vector<ConstantPtr> values);

        explicit ConstantArray(std::vector<ConstantPtr> values);

        Command GenInline() const override;

        std::vector<ConstantPtr> Values;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(std::map<std::string, ConstantPtr> values);

        explicit ConstantObject(std::map<std::string, ConstantPtr> values);

        Command GenInline() const override;

        std::map<std::string, ConstantPtr> Values;
    };

    struct ConstantTarget final : Constant
    {
        static ConstantPtr Create(TargetSelectorE selector, std::map<std::string, std::vector<ConstantPtr>> arguments);

        ConstantTarget(
            TargetSelectorE selector,
            const std::map<std::string, std::vector<ConstantPtr>> &arguments);

        Command GenInline() const override;

        TargetSelectorE Selector;

        std::optional<FloatT> X, Y, Z;
        std::optional<FloatT> DX, DY, DZ;
        OptionalRange<FloatT> Distance;
        OptionalRange<FloatT> X_Rotation;
        OptionalRange<FloatT> Y_Rotation;

        std::map<std::string, Range<FloatT>> Scores;
        std::vector<Invertible<std::string>> Tags;
        std::vector<Invertible<std::string>> Teams;

        std::vector<Invertible<std::string>> Names;
        std::vector<Invertible<ResourceTag>> Types;
        std::vector<Invertible<ResourceLocation>> Predicates;

        std::optional<Invertible<ConstantPtr>> NBT;

        OptionalRange<FloatT> Level;
        std::vector<Invertible<GameModeE>> GameModes;
        std::map<ResourceLocation, std::pair<bool, Invertible<std::string>>> Advancements;

        std::optional<IntegerT> Limit;
        std::optional<SortOrderE> Sort;
    };

    struct ConstantResource final : Constant
    {
        static ConstantPtr Create(ResourceLocation location);

        explicit ConstantResource(ResourceLocation location);

        Command GenInline() const override;

        ResourceLocation Location;
    };

    struct ConstantLocalOffset final : Constant
    {
        static ConstantPtr Create(FloatT offset);

        explicit ConstantLocalOffset(FloatT offset);

        Command GenInline() const override;

        FloatT Offset;
    };

    struct ConstantRelativeOffset final : Constant
    {
        static ConstantPtr Create(FloatT offset);

        explicit ConstantRelativeOffset(FloatT offset);

        Command GenInline() const override;

        FloatT Offset;
    };

    struct Instruction : Value
    {
    };

    struct CallInstruction final : Instruction
    {
        static InstructionPtr Create(CalleeE callee, std::vector<ValuePtr> arguments);

        CallInstruction(CalleeE callee, std::vector<ValuePtr> arguments);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        CalleeE Callee;
        std::vector<ValuePtr> Arguments;
    };

    struct ReturnInstruction final : Instruction
    {
        static InstructionPtr Create(ValuePtr value);

        explicit ReturnInstruction(ValuePtr value);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        ValuePtr Value;
    };

    struct IfUnlessInstruction final : Instruction
    {
        static InstructionPtr Create(bool unless, ValuePtr condition, ValuePtr then);

        IfUnlessInstruction(bool unless, ValuePtr condition, ValuePtr then);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        bool Unless;
        ValuePtr Condition;
        ValuePtr Then;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(ValuePtr dst, ValuePtr src);

        StoreInstruction(ValuePtr dst, ValuePtr src);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        ValuePtr Dst, Src;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(ComparatorE comparator, ValuePtr left, ValuePtr right);

        ComparisonInstruction(ComparatorE comparator, ValuePtr left, ValuePtr right);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        ComparatorE Comparator;
        ValuePtr Left, Right;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(OperatorE operator_, ValuePtr left, ValuePtr right);

        OperationInstruction(OperatorE operator_, ValuePtr left, ValuePtr right);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        OperatorE Operator;
        ValuePtr Left, Right;
    };

    struct AllocationInstruction final : Instruction
    {
        static InstructionPtr CreateValue(ResourceLocation location, IndexT index);
        static InstructionPtr CreateArray(ResourceLocation location, IndexT index);
        static InstructionPtr CreateObject(ResourceLocation location, IndexT index);

        explicit AllocationInstruction(
            AllocationTypeE allocation_type,
            ResourceLocation location,
            IndexT index);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        AllocationTypeE AllocationType;
        ResourceLocation Location;
        IndexT Index;
    };

    struct ArrayInstruction final : Instruction
    {
        static InstructionPtr CreateAppend(ResourceLocation location, ValuePtr array, ValuePtr value);
        static InstructionPtr CreatePrepend(ResourceLocation location, ValuePtr array, ValuePtr value);
        static InstructionPtr CreateInsert(ResourceLocation location, ValuePtr array, ValuePtr value, IndexT index);

        ArrayInstruction(
            ArrayOperationE array_operation,
            ResourceLocation location,
            ValuePtr array,
            ValuePtr value,
            IndexT index);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        ArrayOperationE ArrayOperation;
        ResourceLocation Location;
        ValuePtr Array, Value;
        IndexT Index;
    };

    struct ObjectInstruction final : Instruction
    {
        static InstructionPtr CreateInsert(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);

        ObjectInstruction(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key);

        void Gen(std::vector<Command> &commands) const override;
        Command GenInline() const override;

        ResourceLocation Location;
        ValuePtr Object, Value;
        std::string Key;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(ResourceLocation location, std::string id);

        NamedValue(ResourceLocation location, std::string id);

        Command GenInline() const override;

        ResourceLocation Location;
        std::string ID;
    };
}
