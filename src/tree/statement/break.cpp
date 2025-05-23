#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/statement.hpp>

mcc::BreakStatement::BreakStatement(const SourceLocation &where)
    : Statement(where)
{
}

std::ostream &mcc::BreakStatement::Print(std::ostream &stream) const
{
    return stream << "break";
}

void mcc::BreakStatement::Generate(Builder &builder, Frame &frame) const
{
    Assert(!!frame.Tail, Where, "no tail target in current frame");
    frame.Flags |= FrameFlag_RequireTail;
    (void) builder.CreateDirect(Where, frame.Tail);
}
