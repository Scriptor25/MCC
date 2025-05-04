#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ObjectExpression::ObjectExpression(std::map<std::string, ExpressionPtr> elements)
    : Elements(std::move(elements))
{
}

std::ostream &mcc::ObjectExpression::Print(std::ostream &stream) const
{
    stream << "{ ";
    auto first = true;
    for (auto &[key_, value_]: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        value_->Print(stream << key_ << ": ");
    }
    return stream << " }";
}

mcc::ValuePtr mcc::ObjectExpression::Gen(Builder &builder, const bool inline_) const
{
    std::map<std::string, ValuePtr> values;

    auto all_constant = true;
    for (auto &[key_, value_]: Elements)
    {
        auto value = value_->Gen(builder, inline_);
        all_constant &= !!std::dynamic_pointer_cast<Constant>(value);
        values.emplace(key_, value);
    }

    Assert(!inline_ || all_constant, "inline object must only contain constant values");

    if (all_constant)
    {
        std::map<std::string, ConstantPtr> constants;

        for (auto &[key_, value_]: values)
            constants.emplace(key_, std::dynamic_pointer_cast<Constant>(value_));

        return ConstantObject::Create(constants);
    }

    auto object = builder.AllocateObject(inline_);

    for (auto &[key_, value_]: values)
        builder.CreateInsert(object, value_, key_, inline_);

    return object;
}
