#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::TargetExpression::TargetExpression(
    const TargetSelectorE selector,
    std::map<std::string, std::vector<TargetAttributePtr>> attributes)
    : Selector(selector),
      Attributes(std::move(attributes))
{
}

std::ostream &mcc::TargetExpression::Print(std::ostream &stream) const
{
    if (Attributes.empty())
        return stream << '@' << Selector;

    stream << '@' << Selector << '[';

    auto first = true;
    for (auto &[key_, values_]: Attributes)
    {
        for (auto &value_: values_)
        {
            if (first)
                first = false;
            else
                stream << ',';
            value_->Print(stream << key_ << '=');
        }
    }

    return stream << ']';
}

mcc::ValuePtr mcc::TargetExpression::Gen(Builder &builder, const bool inline_) const
{
    return ConstantTarget::Create(Selector, Attributes);
}
