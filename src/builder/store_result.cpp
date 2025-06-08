#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::Builder::StoreResult(const SourceLocation &where, const TypePtr &type, const std::string &name)
{
    Assert(!!type, where, "type must not be null");
    Assert(!name.empty(), where, "name must not be empty");

    const auto result = FunctionResult::Create(where, type, m_InsertBlock->Parent->Location);
    return CreateVariable(where, type, name, false, result);
}
