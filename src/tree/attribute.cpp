#include <sstream>
#include <mcc/attribute.hpp>
#include <mcc/tree.hpp>

mcc::TargetAttribute::TargetAttribute(const bool invert)
    : Invert(invert)
{
}

mcc::IntegerAttribute::IntegerAttribute(const bool invert, const IntegerT value)
    : TargetAttribute(invert),
      Value(value)
{
}

std::ostream &mcc::IntegerAttribute::Print(std::ostream &stream) const
{
    return stream << (Invert ? "!" : "") << Value;
}

std::string mcc::IntegerAttribute::String() const
{
    return (Invert ? "!" : "") + std::to_string(Value);
}

mcc::FloatAttribute::FloatAttribute(const bool invert, const FloatT value)
    : TargetAttribute(invert),
      Value(value)
{
}

std::ostream &mcc::FloatAttribute::Print(std::ostream &stream) const
{
    return stream << (Invert ? "!" : "") << Value;
}

std::string mcc::FloatAttribute::String() const
{
    return (Invert ? "!" : "") + std::to_string(Value);
}

mcc::RangeAttribute::RangeAttribute(
    const bool invert,
    const std::optional<IntegerT> beg,
    const std::optional<IntegerT> end)
    : TargetAttribute(invert),
      Beg(beg),
      End(end)
{
}

std::ostream &mcc::RangeAttribute::Print(std::ostream &stream) const
{
    stream << (Invert ? "!" : "");
    if (Beg.has_value())
        stream << Beg.value();
    stream << "..";
    if (End.has_value())
        stream << End.value();
    return stream;
}

std::string mcc::RangeAttribute::String() const
{
    return (Invert ? "!" : "")
           + (Beg.has_value() ? std::to_string(Beg.value()) : "")
           + ".."
           + (End.has_value() ? std::to_string(End.value()) : "");
}

mcc::StringAttribute::StringAttribute(const bool invert, std::string value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::StringAttribute::Print(std::ostream &stream) const
{
    return stream << (Invert ? "!" : "") << '"' << Value << '"';
}

std::string mcc::StringAttribute::String() const
{
    return (Invert ? "!" : "") + '"' + Value + '"';
}

mcc::EnumAttribute::EnumAttribute(const bool invert, std::string value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::EnumAttribute::Print(std::ostream &stream) const
{
    return stream << (Invert ? "!" : "") << Value;
}

std::string mcc::EnumAttribute::String() const
{
    return (Invert ? "!" : "") + Value;
}

mcc::NameAttribute::NameAttribute(const bool invert, std::string value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::NameAttribute::Print(std::ostream &stream) const
{
    return stream << (Invert ? "!" : "") << Value;
}

std::string mcc::NameAttribute::String() const
{
    return (Invert ? "!" : "") + Value;
}

mcc::MapAttribute::MapAttribute(const bool invert, std::map<std::string, TargetAttributePtr> values)
    : TargetAttribute(invert),
      Values(std::move(values))
{
}

std::ostream &mcc::MapAttribute::Print(std::ostream &stream) const
{
    stream << (Invert ? "!" : "") << '{';
    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            stream << ',';
        value_->Print(stream << key_ << '=');
    }
    return stream << '}';
}

std::string mcc::MapAttribute::String() const
{
    std::string result;
    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += key_;
        result += '=';
        result += value_->String();
    }
    return (Invert ? "!" : "") + ('{' + result + '}');
}

mcc::ResourceMapAttribute::ResourceMapAttribute(
    const bool invert,
    std::vector<std::pair<ResourceLocation, TargetAttributePtr>> values)
    : TargetAttribute(invert),
      Values(std::move(values))
{
}

std::ostream &mcc::ResourceMapAttribute::Print(std::ostream &stream) const
{
    stream << (Invert ? "!" : "") << '{';
    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            stream << ',';
        value_->Print(key_.Print(stream) << '=');
    }
    return stream << '}';
}

std::string mcc::ResourceMapAttribute::String() const
{
    std::string result;
    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += key_.String();
        result += '=';
        result += value_->String();
    }
    return (Invert ? "!" : "") + ('{' + result + '}');
}

mcc::NBTAttribute::NBTAttribute(const bool invert, ExpressionPtr value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::NBTAttribute::Print(std::ostream &stream) const
{
    return Value->Print(stream << (Invert ? "!" : ""));
}

std::string mcc::NBTAttribute::String() const
{
    std::stringstream stream;
    Value->Print(stream);
    return (Invert ? "!" : "") + stream.str();
}

mcc::ResourceAttribute::ResourceAttribute(const bool invert, ResourceLocation value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::ResourceAttribute::Print(std::ostream &stream) const
{
    return Value.Print(stream << (Invert ? "!" : ""));
}

std::string mcc::ResourceAttribute::String() const
{
    return (Invert ? "!" : "") + Value.String();
}

mcc::TagAttribute::TagAttribute(const bool invert, ResourceTag value)
    : TargetAttribute(invert),
      Value(std::move(value))
{
}

std::ostream &mcc::TagAttribute::Print(std::ostream &stream) const
{
    return Value.Print(stream << (Invert ? "!" : ""));
}

std::string mcc::TagAttribute::String() const
{
    return (Invert ? "!" : "") + Value.String();
}
