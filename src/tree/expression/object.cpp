#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
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

    std::map<std::string, TypePtr> elements;

    for (auto &[key_, value_]: Elements)
    {
        const auto value = value_->GenerateValue(builder, frame);
        if (const auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants[key_] = constant;
        values[key_] = value;
        elements[key_] = value->Type;
    }

    const auto type = TypeContext::GetStruct(elements);

    if (values.size() == constants.size())
        return ConstantObject::Create(Where, type, constants);

    auto object = builder.AllocateObject(Where, type);

    for (auto &[key_, value_]: values)
        (void) builder.CreateInsert(Where, object, value_, key_);

    return object;
}
