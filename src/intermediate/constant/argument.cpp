#include <utility>
#include <mcc/constant.hpp>

mcc::ValuePtr mcc::ConstantArgument::Create(const SourceLocation &where, const TypePtr &type, const std::string &name)
{
    return std::make_shared<ConstantArgument>(where, type, name);
}

mcc::ConstantArgument::ConstantArgument(const SourceLocation &where, const TypePtr &type, std::string name)
    : Constant(where, type),
      Name(std::move(name))
{
}

mcc::Result mcc::ConstantArgument::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Argument,
        .Name = Name,
    };
}
