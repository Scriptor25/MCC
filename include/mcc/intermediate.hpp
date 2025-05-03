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

        virtual void Gen(CommandVector &commands) const;
        virtual Command GenInline() const;
        virtual CommandResult GenResult(bool stringify = false) const;

        bool Invert = false;
    };

    struct Constant : Value
    {
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(bool value);

        explicit ConstantBoolean(bool value);

        CommandResult GenResult(bool stringify = false) const override;

        bool Value;
    };

    struct ConstantInteger final : Constant
    {
        static ConstantPtr Create(IntegerT value);

        explicit ConstantInteger(IntegerT value);

        CommandResult GenResult(bool stringify = false) const override;

        IntegerT Value;
    };

    struct ConstantFloat final : Constant
    {
        static ConstantPtr Create(FloatT value);

        explicit ConstantFloat(FloatT value);

        CommandResult GenResult(bool stringify = false) const override;

        FloatT Value;
    };

    struct ConstantFloatRange final : Constant
    {
        static ConstantPtr Create(FloatT min, FloatT max);
        static ConstantPtr CreateMin(FloatT min);
        static ConstantPtr CreateMax(FloatT max);

        ConstantFloatRange(std::optional<FloatT> min, std::optional<FloatT> max);

        CommandResult GenResult(bool stringify = false) const override;

        std::optional<FloatT> Min;
        std::optional<FloatT> Max;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(std::string value);

        explicit ConstantString(std::string value);

        CommandResult GenResult(bool stringify = false) const override;

        std::string Value;
    };

    struct ConstantArray final : Constant
    {
        static ConstantPtr Create(std::vector<ConstantPtr> values, bool stringify);

        ConstantArray(std::vector<ConstantPtr> values, bool stringify);

        CommandResult GenResult(bool stringify = false) const override;

        std::vector<ConstantPtr> Values;
        bool Stringify;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(std::map<std::string, ConstantPtr> values);

        explicit ConstantObject(std::map<std::string, ConstantPtr> values);

        CommandResult GenResult(bool stringify = false) const override;

        std::map<std::string, ConstantPtr> Values;
    };

    struct ConstantTarget final : Constant
    {
        static ConstantPtr Create(TargetSelectorE selector, std::map<std::string, std::vector<ConstantPtr>> arguments);

        ConstantTarget(
            TargetSelectorE selector,
            const std::map<std::string, std::vector<ConstantPtr>> &arguments);

        CommandResult GenResult(bool stringify = false) const override;

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

        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
    };

    struct ConstantLocalOffset final : Constant
    {
        static ConstantPtr Create(FloatT offset);

        explicit ConstantLocalOffset(FloatT offset);

        CommandResult GenResult(bool stringify = false) const override;

        FloatT Offset;
    };

    struct ConstantRelativeOffset final : Constant
    {
        static ConstantPtr Create(FloatT offset);

        explicit ConstantRelativeOffset(FloatT offset);

        CommandResult GenResult(bool stringify = false) const override;

        FloatT Offset;
    };

    struct Instruction : Value
    {
        std::string GetResultID() const;
    };

    struct CallInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, CalleeE callee, std::vector<ValuePtr> arguments);

        CallInstruction(ResourceLocation location, CalleeE callee, std::vector<ValuePtr> arguments);

        void Gen(CommandVector &commands) const override;
        Command GenInline() const override;
        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
        CalleeE Callee;
        std::vector<ValuePtr> Arguments;
    };

    struct ReturnInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, ValuePtr value);

        ReturnInstruction(ResourceLocation location, ValuePtr value);

        void Gen(CommandVector &commands) const override;
        Command GenInline() const override;

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

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
        bool Unless;
        ValuePtr Condition, Then, Else;
    };

    struct StoreInstruction final : Instruction
    {
        static InstructionPtr Create(ResourceLocation location, ValuePtr dst, ValuePtr src);

        StoreInstruction(ResourceLocation location, ValuePtr dst, ValuePtr src);

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
        ValuePtr Dst, Src;
    };

    struct ComparisonInstruction final : Instruction
    {
        static InstructionPtr Create(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);

        ComparisonInstruction(ComparatorE comparator, ResourceLocation location, ValuePtr left, ValuePtr right);

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

        ComparatorE Comparator;
        ResourceLocation Location;
        ValuePtr Left, Right;
    };

    struct OperationInstruction final : Instruction
    {
        static InstructionPtr Create(OperatorE operator_, ResourceLocation location, ValuePtr left, ValuePtr right);

        OperationInstruction(OperatorE operator_, ResourceLocation location, ValuePtr left, ValuePtr right);

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

        OperatorE Operator;
        ResourceLocation Location;
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

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

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

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

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

        void Gen(CommandVector &commands) const override;
        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
        ValuePtr Object, Value;
        std::string Key;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(ResourceLocation location, std::string id);

        NamedValue(ResourceLocation location, std::string id);

        Command GenInline() const override;
        CommandResult GenResult(bool stringify = false) const override;

        ResourceLocation Location;
        std::string ID;
    };
}
