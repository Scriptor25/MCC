#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::MemberExpression::MemberExpression(const SourceLocation &where, ExpressionPtr object, std::string member)
    : Expression(where),
      Object(std::move(object)),
      Member(std::move(member))
{
}

std::ostream &mcc::MemberExpression::Print(std::ostream &stream) const
{
    return Object->Print(stream) << '.' << Member;
}

mcc::ValuePtr mcc::MemberExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    auto object = Object->GenerateValue(builder, frame);
    Assert(object->Type->IsObject(), Object->Where, "object must be of type object, but is {}", object->Type);

    if (const auto constant_object = std::dynamic_pointer_cast<ConstantObject>(object))
        return constant_object->Values.at(Member);

    if (const auto argument_object = std::dynamic_pointer_cast<ArgumentValue>(object))
    {
        object = builder.Allocate(Object->Where, object->Type, false);
        (void) builder.CreateStore(Object->Where, object, argument_object, true);
    }

    return MemberReference::Create(Where, object, Member);
}
