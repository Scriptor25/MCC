#include <mcc/intermediate.hpp>

void mcc::Value::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("only inlined");
}
