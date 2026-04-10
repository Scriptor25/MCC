#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::Builder::StoreResult(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type)
{
    Assert(!!type, where, "type must not be null");
    Assert(!name.empty(), where, "name must not be empty");

    const auto result = FunctionResult::Create(where, name, type, m_InsertBlock->Parent);
    return CreateVariable(where, type, name, false, result);
}
