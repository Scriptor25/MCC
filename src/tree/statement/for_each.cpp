#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>
#include <utility>

mcc::ForEachStatement::ForEachStatement(const SourceLocation &where, const bool is_constant, std::string name, ExpressionPtr iterable, StatementPtr do_)
    : Statement(where), IsConstant(is_constant), Name(std::move(name)), Iterable(std::move(iterable)), Do(std::move(do_))
{
}

std::ostream &mcc::ForEachStatement::Print(std::ostream &stream) const
{
    return Do->Print(Iterable->Print(stream << "foreach (" << (IsConstant ? "const" : "let") << ' ' << Name << " : ") << ") ");
}

void mcc::ForEachStatement::Generate(Builder &builder, Frame &frame) const
{
    const auto parent      = builder.GetInsertBlock()->Parent;
    const auto head_target = Block::Create(Where, builder.GetContext(), parent);
    const auto loop_target = Block::Create(Where, builder.GetContext(), parent);
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);

    auto target_frame = frame;
    target_frame.Head = head_target;
    target_frame.Tail = tail_target;

    const auto iterable          = Iterable->GenerateValue(builder, frame);
    const auto iterable_constant = std::dynamic_pointer_cast<Constant>(iterable);
    const auto iterable_copy     = builder.CreateAllocation(Where, iterable->Type, false, iterable_constant);
    if (!iterable_constant)
        (void) builder.CreateStore(Where, iterable_copy, iterable);
    (void) builder.CreateDirect(Where, head_target);

    builder.SetInsertBlock(head_target);
    const auto first_element = ElementReference::Create(Where, iterable_copy, 0);
    const auto condition     = builder.CreateNotNull(Where, first_element);
    (void) builder.CreateBranch(Where, condition, loop_target, tail_target);

    builder.PushVariables();

    builder.SetInsertBlock(loop_target);
    const auto variable = builder.CreateVariable(Where, first_element->Type, Name, IsConstant, first_element);
    (void) builder.CreateDelete(Where, first_element);
    Do->Generate(builder, frame);
    if (!builder.GetInsertBlock()->GetTerminator())
        (void) builder.CreateDirect(Where, head_target);

    builder.PopVariables();

    builder.SetInsertBlock(tail_target);
}
