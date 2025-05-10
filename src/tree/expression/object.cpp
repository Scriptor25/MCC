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
    std::map<std::string, ConstantPtr> constants;

    for (auto &[key_, value_]: Elements)
    {
        auto value = value_->GenerateValue(builder);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace(key_, constant);
        values.emplace(key_, value);
    }

    if (values.size() == constants.size())
        return ConstantObject::Create(std::move(constants));

    auto object = builder.AllocateObject();

    for (auto &[key_, value_]: values)
        (void) builder.CreateInsert(object, value_, key_);

    return object;
}
