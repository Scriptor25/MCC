#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/statement.hpp>

mcc::ContinueStatement::ContinueStatement(const SourceLocation &where)
    : Statement(where)
{
}

std::ostream &mcc::ContinueStatement::Print(std::ostream &stream) const
{
    return stream << "continue";
}

void mcc::ContinueStatement::Generate(Builder &builder, Frame &frame) const
{
    Assert(!!frame.Head, Where, "no head target in current frame");
    frame.Flags |= FrameFlag_RequireHead;
    (void) builder.CreateDirect(Where, frame.Head);
}
