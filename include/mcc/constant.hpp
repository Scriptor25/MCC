#pragma once

#include <map>
#include <optional>
#include <mcc/common.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Constant : Value
    {
        [[nodiscard]] bool RequireStack() const override;
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(bool value);

        explicit ConstantBoolean(bool value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        bool Value;
    };

    struct ConstantInteger final : Constant
    {
        static ConstantPtr Create(IntegerT value);

        explicit ConstantInteger(IntegerT value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        IntegerT Value;
    };

    struct ConstantFloat final : Constant
    {
        static ConstantPtr Create(FloatT value);

        explicit ConstantFloat(FloatT value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        FloatT Value;
    };

    struct ConstantFloatRange final : Constant
    {
        static ConstantPtr Create(FloatT min, FloatT max);
        static ConstantPtr CreateMin(FloatT min);
        static ConstantPtr CreateMax(FloatT max);

        ConstantFloatRange(std::optional<FloatT> min, std::optional<FloatT> max);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::optional<FloatT> Min;
        std::optional<FloatT> Max;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(std::string value);

        explicit ConstantString(std::string value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::string Value;
    };

    struct ConstantArray final : Constant
    {
        static ConstantPtr Create(std::vector<ConstantPtr> values, bool stringify);

        ConstantArray(std::vector<ConstantPtr> values, bool stringify);
        ~ConstantArray() override;

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::vector<ConstantPtr> Values;
        bool Stringify;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(std::map<std::string, ConstantPtr> values);

        explicit ConstantObject(std::map<std::string, ConstantPtr> values);
        ~ConstantObject() override;

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

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

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        TargetSelectorE Selector;
        std::map<std::string, std::vector<TargetAttributePtr>> Attributes;
    };

    struct ConstantResource final : Constant
    {
        static ConstantPtr Create(ResourceLocation location, ConstantPtr state, ConstantPtr data);

        ConstantResource(ResourceLocation location, ConstantPtr state, ConstantPtr data);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        ConstantPtr State;
        ConstantPtr Data;
    };
}
