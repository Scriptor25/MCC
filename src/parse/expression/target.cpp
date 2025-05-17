#include <functional>
#include <mcc/attribute.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>
#include <mcc/parse.hpp>

mcc::TargetAttributePtr mcc::Parser::ParseIntegerAttribute(bool invert)
{
    auto value = Expect(TokenType_Integer).Integer;
    return std::make_unique<IntegerAttribute>(invert, value);
}

mcc::TargetAttributePtr mcc::Parser::ParseFloatAttribute(bool invert)
{
    if (At(TokenType_Integer))
    {
        auto value = Skip().Integer;
        return std::make_unique<IntegerAttribute>(invert, value);
    }
    auto value = Expect(TokenType_Float).Float;
    return std::make_unique<FloatAttribute>(invert, value);
}

mcc::TargetAttributePtr mcc::Parser::ParseIntegerRangeAttribute(bool invert)
{
    if (At(TokenType_Integer))
    {
        auto value = Skip().Integer;
        return std::make_unique<IntegerAttribute>(invert, value);
    }
    auto [beg_, end_] = Expect(TokenType_Range).Range;
    return std::make_unique<RangeAttribute>(invert, beg_, end_);
}

mcc::TargetAttributePtr mcc::Parser::ParseFloatRangeAttribute(bool invert)
{
    if (At(TokenType_Integer))
    {
        auto value = Skip().Integer;
        return std::make_unique<IntegerAttribute>(invert, value);
    }
    if (At(TokenType_Float))
    {
        auto value = Skip().Float;
        return std::make_unique<FloatAttribute>(invert, value);
    }
    auto [beg_, end_] = Expect(TokenType_Range).Range;
    return std::make_unique<RangeAttribute>(invert, beg_, end_);
}

mcc::TargetAttributePtr mcc::Parser::ParseStringAttribute(bool invert)
{
    auto value = Expect(TokenType_String).Value;
    return std::make_unique<StringAttribute>(invert, std::move(value));
}

mcc::TargetAttributePtr mcc::Parser::ParseEnumAttribute(bool invert, const std::vector<const char *> &values)
{
    auto value = ExpectEnum(values).Value;
    return std::make_unique<EnumAttribute>(invert, std::move(value));
}

mcc::TargetAttributePtr mcc::Parser::ParseNameAttribute(bool invert)
{
    auto value = Expect(TokenType_Symbol).Value;
    return std::make_unique<NameAttribute>(invert, std::move(value));
}

mcc::TargetAttributePtr mcc::Parser::ParseMapAttribute(bool invert, const Parse &parse)
{
    Expect(TokenType_Other, "{");

    std::map<std::string, TargetAttributePtr> values;
    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = Expect(TokenType_Symbol).Value;
        Expect(TokenType_Operator, "=");
        auto value = parse(*this, SkipIf(TokenType_Operator, "!"));
        values.emplace(key, std::move(value));
        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<MapAttribute>(invert, std::move(values));
}

mcc::TargetAttributePtr mcc::Parser::ParseResourceMapAttribute(bool invert, const Parse &parse)
{
    Expect(TokenType_Other, "{");

    std::vector<std::pair<ResourceLocation, TargetAttributePtr>> values;
    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = ParseResourceLocation();
        Expect(TokenType_Operator, "=");
        auto value = parse(*this, SkipIf(TokenType_Operator, "!"));
        values.emplace_back(std::move(key), std::move(value));
        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ResourceMapAttribute>(invert, std::move(values));
}

mcc::TargetAttributePtr mcc::Parser::ParseNBTAttribute(bool invert)
{
    auto value = ParseObjectExpression();
    return std::make_unique<NBTAttribute>(invert, std::move(value));
}

mcc::TargetAttributePtr mcc::Parser::ParseResourceAttribute(bool invert)
{
    auto value = ParseResourceLocation();
    return std::make_unique<ResourceAttribute>(invert, std::move(value));
}

mcc::TargetAttributePtr mcc::Parser::ParseTagAttribute(bool invert)
{
    Expect(TokenType_Other, "#");
    auto [namespace_, path_] = ParseResourceLocation();
    return std::make_unique<TagAttribute>(invert, ResourceTag(std::move(namespace_), std::move(path_)));
}

mcc::ExpressionPtr mcc::Parser::ParseTargetExpression(const bool with_attributes)
{
    static const Parse parse_boolean = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseEnumAttribute(invert, {"true", "false"});
    };
    static const Parse parse_float = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseFloatAttribute(invert);
    };
    static const Parse parse_integer = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseIntegerAttribute(invert);
    };
    static const Parse parse_integer_range = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseIntegerRangeAttribute(invert);
    };
    static const Parse parse_float_range = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseFloatRangeAttribute(invert);
    };
    static const Parse parse_string = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        if (!self.At(TokenType_String))
            return nullptr;
        return self.ParseStringAttribute(invert);
    };
    static const Parse parse_name = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        if (!self.At(TokenType_Symbol))
            return nullptr;
        return self.ParseNameAttribute(invert);
    };
    static const Parse parse_nbt = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseNBTAttribute(invert);
    };
    static const Parse parse_resource = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseResourceAttribute(invert);
    };
    static const Parse parse_tag = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        if (!self.At(TokenType_Other, "#"))
            return self.ParseResourceAttribute(invert);
        return self.ParseTagAttribute(invert);
    };

    static const Parse parse_scores = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseMapAttribute(invert, parse_float_range);
    };
    static const Parse parse_sort = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseEnumAttribute(invert, {"nearest", "furthest", "random", "arbitrary"});
    };
    static const Parse parse_game_mode = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseEnumAttribute(invert, {"spectator", "survival", "creative", "adventure"});
    };
    static const Parse parse_advancement = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        if (self.At(TokenType_Other, "{"))
            return self.ParseMapAttribute(invert, parse_boolean);
        return parse_boolean(self, invert);
    };
    static const Parse parse_advancements = [](Parser &self, const bool invert) -> TargetAttributePtr
    {
        return self.ParseResourceMapAttribute(invert, parse_advancement);
    };

    static const std::map<std::string_view, Parse> map
    {
        {"x", parse_float},
        {"y", parse_float},
        {"z", parse_float},
        {"dx", parse_float},
        {"dy", parse_float},
        {"dz", parse_float},
        {"distance", parse_float_range},
        {"scores", parse_scores},
        {"tag", parse_string},
        {"team", parse_name},
        {"limit", parse_integer},
        {"sort", parse_sort},
        {"level", parse_integer_range},
        {"gamemode", parse_game_mode},
        {"name", parse_string},
        {"x_rotation", parse_float_range},
        {"y_rotation", parse_float_range},
        {"type", parse_tag},
        {"nbt", parse_nbt},
        {"advancements", parse_advancements},
        {"predicate", parse_resource},
    };

    auto where = m_Token.Where;
    const auto selector = ToTargetSelector(
        Expect(
            with_attributes
                ? TokenType_TargetAttributes
                : TokenType_Target).Value);

    std::map<std::string, std::vector<TargetAttributePtr>> attributes;
    if (with_attributes)
    {
        while (!At(TokenType_Other, "]") && !At(TokenType_EOF))
        {
            auto key = Expect(TokenType_Symbol).Value;
            Expect(TokenType_Operator, "=");
            const auto invert = SkipIf(TokenType_Operator, "!");
            auto node = map.at(key)(*this, invert);
            attributes[key].emplace_back(std::move(node));

            if (!At(TokenType_Other, "]"))
                Expect(TokenType_Other, ",");
        }
        Expect(TokenType_Other, "]");
    }

    auto view = '@' + std::string(ToString(selector));
    if (!attributes.empty())
    {
        view += '[';

        auto first = true;
        for (auto &[key_, value_]: attributes)
            for (const auto &attribute: value_)
            {
                if (first)
                    first = false;
                else
                    view += ',';
                view += key_ + '=' + attribute->String();
            }

        view += ']';
    }

    return std::make_unique<ConstantExpression>(where, ConstantTarget::Create(selector, std::move(attributes)), view);
}
