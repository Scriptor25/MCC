#include <mcc/tree.hpp>

static constexpr unsigned spaces = 2;
static unsigned indentation = 0;

static std::string get_indentation()
{
    std::string s;
    for (unsigned i = 0; i < indentation; ++i)
        s += ' ';
    return s;
}

std::ostream &mcc::ResourceLocation::Print(std::ostream &stream) const
{
    return stream << NS << ':' << ID;
}

mcc::NamespaceStatement::NamespaceStatement(std::string id)
    : m_ID(std::move(id))
{
}

std::ostream &mcc::NamespaceStatement::Print(std::ostream &stream) const
{
    return stream << "namespace " << m_ID;
}

std::string mcc::NamespaceStatement::GetID() const
{
    return m_ID;
}

mcc::DefineStatement::DefineStatement(
    ResourceLocation location,
    std::vector<std::string> parameters,
    std::vector<ResourceLocation> tags,
    std::vector<ExpressionPtr> expressions)
    : m_Location(std::move(location)),
      m_Parameters(std::move(parameters)),
      m_Tags(std::move(tags)),
      m_Expressions(std::move(expressions))
{
}

std::ostream &mcc::DefineStatement::Print(std::ostream &stream) const
{
    m_Location.Print(stream << "define ") << '(';
    for (unsigned i = 0; i < m_Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << m_Parameters[i];
    }
    stream << ") ";
    for (unsigned i = 0; i < m_Tags.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        m_Tags[i].Print(stream << '#');
    }
    if (!m_Tags.empty())
        stream << ' ';
    stream << '{' << std::endl;
    indentation += spaces;
    auto is = get_indentation();
    for (auto &statement: m_Expressions)
        statement->Print(stream << is) << ';' << std::endl;
    indentation -= spaces;
    is = get_indentation();
    return stream << is << '}';
}

mcc::BoolExpression::BoolExpression(const bool value)
    : m_Value(value)
{
}

std::ostream &mcc::BoolExpression::Print(std::ostream &stream) const
{
    return stream << (m_Value ? "true" : "false");
}

mcc::IntegerExpression::IntegerExpression(const int64_t value)
    : m_Value(value)
{
}

std::ostream &mcc::IntegerExpression::Print(std::ostream &stream) const
{
    return stream << m_Value;
}

mcc::FloatExpression::FloatExpression(const double value)
    : m_Value(value)
{
}

std::ostream &mcc::FloatExpression::Print(std::ostream &stream) const
{
    return stream << m_Value;
}

mcc::StringExpression::StringExpression(std::string value)
    : m_Value(std::move(value))
{
}

std::ostream &mcc::StringExpression::Print(std::ostream &stream) const
{
    return stream << '"' << m_Value << '"';
}

mcc::SymbolExpression::SymbolExpression(std::string id)
    : m_ID(std::move(id))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << m_ID;
}

mcc::TargetExpression::TargetExpression(std::string id)
    : m_ID(std::move(id))
{
}

std::ostream &mcc::TargetExpression::Print(std::ostream &stream) const
{
    return stream << '@' << m_ID;
}

mcc::ArrayExpression::ArrayExpression(std::vector<ExpressionPtr> elements)
    : m_Elements(std::move(elements))
{
}

std::ostream &mcc::ArrayExpression::Print(std::ostream &stream) const
{
    stream << "[ ";
    for (unsigned i = 0; i < m_Elements.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        m_Elements[i]->Print(stream);
    }
    return stream << " ]";
}

mcc::ObjectExpression::ObjectExpression(std::map<std::string, ExpressionPtr> elements)
    : m_Elements(std::move(elements))
{
}

std::ostream &mcc::ObjectExpression::Print(std::ostream &stream) const
{
    stream << "{ ";
    auto first = true;
    for (auto &[key, value]: m_Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        value->Print(stream << key << ": ");
    }
    return stream << " }";
}

mcc::RelativeOffsetExpression::RelativeOffsetExpression()
{
}

std::ostream &mcc::RelativeOffsetExpression::Print(std::ostream &stream) const
{
    return stream << '~';
}

mcc::LocalOffsetExpression::LocalOffsetExpression()
{
}

std::ostream &mcc::LocalOffsetExpression::Print(std::ostream &stream) const
{
    return stream << '^';
}

mcc::BinaryExpression::BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right)
    : m_Operator(std::move(operator_)),
      m_Left(std::move(left)),
      m_Right(std::move(right))
{
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return m_Right->Print(m_Left->Print(stream) << ' ' << m_Operator << ' ');
}

mcc::CallExpression::CallExpression(ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
    : m_Callee(std::move(callee)),
      m_Arguments(std::move(arguments))
{
}

std::ostream &mcc::CallExpression::Print(std::ostream &stream) const
{
    m_Callee->Print(stream) << '(';
    for (unsigned i = 0; i < m_Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        m_Arguments[i]->Print(stream);
    }
    return stream << ')';
}

mcc::StringNode::StringNode(std::string value)
    : m_Value(std::move(value))
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << m_Value;
}

mcc::ExpressionNode::ExpressionNode(ExpressionPtr expression)
    : m_Expression(std::move(expression))
{
}

std::ostream &mcc::ExpressionNode::Print(std::ostream &stream) const
{
    return m_Expression->Print(stream << "${") << '}';
}

mcc::FormatExpression::FormatExpression(std::vector<FormatNodePtr> nodes)
    : m_Nodes(std::move(nodes))
{
}

std::ostream &mcc::FormatExpression::Print(std::ostream &stream) const
{
    stream << '`';
    for (auto &node: m_Nodes)
        node->Print(stream);
    return stream << '`';
}
