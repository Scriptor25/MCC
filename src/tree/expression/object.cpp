#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ObjectExpression::ObjectExpression(SourceLocation where, std::map<std::string, ExpressionPtr> elements)
    : Expression(std::move(where)),
      Elements(std::move(elements))
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

mcc::ValuePtr mcc::ObjectExpression::GenerateValue(Builder &builder) const
{
    std::map<std::string, ValuePtr> values;

    auto all_constant = true;
    for (auto &[key_, value_]: Elements)
    {
        auto value = value_->GenerateValue(builder);
        all_constant &= !!std::dynamic_pointer_cast<Constant>(value);
        values.emplace(key_, value);
    }

    if (all_constant)
    {
        std::map<std::string, ConstantPtr> constants;

        for (auto &[key_, value_]: values)
            constants.emplace(key_, std::dynamic_pointer_cast<Constant>(value_));

        return ConstantObject::Create(constants);
    }

    auto object = builder.AllocateObject();

    for (auto &[key_, value_]: values)
        (void) builder.CreateInsert(object, value_, key_);

    return object;
}
