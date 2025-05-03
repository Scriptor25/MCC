#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::TargetExpression::TargetExpression(
    const TargetSelectorE selector,
    std::map<std::string, std::vector<ExpressionPtr>> arguments)
    : Selector(selector),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::TargetExpression::Print(std::ostream &stream) const
{
    if (Arguments.empty())
        return stream << '@' << Selector;

    stream << '@' << Selector << '[';

    auto first = true;
    for (auto &[key_, values_]: Arguments)
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
    std::map<std::string, std::vector<ConstantPtr>> arguments;

    for (auto &[key_, values_]: Arguments)
    {
        for (auto &value_: values_)
        {
            auto value = value_->Gen(builder, inline_);
            auto constant = std::dynamic_pointer_cast<Constant>(value);

            if (!constant)
                throw std::runtime_error("non - constant value in target arguments");

            arguments[key_].emplace_back(constant);
        }
    }

    return ConstantTarget::Create(Selector, arguments);
}
