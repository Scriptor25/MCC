#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::ObjectExpression::ObjectExpression(const SourceLocation &where, std::map<std::string, ExpressionPtr> elements)
    : Expression(where),
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

mcc::ValuePtr mcc::ObjectExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::map<std::string, ValuePtr> values;
    std::map<std::string, ConstantPtr> constants;

    for (auto &[key_, value_]: Elements)
    {
        auto value = value_->GenerateValue(builder, frame);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace(key_, constant);
        values.emplace(key_, value);
    }

    if (values.size() == constants.size())
        return ConstantObject::Create(Where, constants);

    auto object = builder.AllocateObject(Where);

    for (auto &[key_, value_]: values)
        (void) builder.CreateInsert(Where, object, value_, key_);

    return object;
}
