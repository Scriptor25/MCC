#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

#include <ranges>

static bool check_function(
        mcc::FunctionPtr function,
        const mcc::ParameterList &parameters)
{
    if (parameters.size() != function->Parameters.size())
        return false;

    unsigned i;
    for (i = 0; i < parameters.size(); ++i)
    {
        if (parameters[i].FieldType != function->Parameters[i].FieldType)
            break;
        if (parameters[i].Type != function->Parameters[i].Type)
            break;
    }
    return i >= parameters.size();
}

mcc::FunctionPtr mcc::Builder::CreateFunction(
        const SourceLocation &where,
        ResourceLocation location,
        const ParameterList &parameters,
        const TypePtr &result_type,
        const bool throws)
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    auto &functions = m_Functions[location.Namespace][location.Path];
    for (auto &function : functions)
        if (check_function(function, parameters))
            Error(where, "already defined function {} with parameters {}", location, parameters);

    auto function = Function::Create(where, m_Context, location, parameters, result_type, throws);
    functions.push_back(function);
    return function;
}

mcc::FunctionPtr mcc::Builder::FindFunction(
        ResourceLocation location,
        const ParameterList &parameters) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    if (!m_Functions.contains(location.Namespace))
        return {};

    if (!m_Functions.at(location.Namespace).contains(location.Path))
        return {};

    auto &functions = m_Functions.at(location.Namespace).at(location.Path);
    for (auto &function : functions)
        if (check_function(function, parameters))
            return function;

    return {};
}

std::vector<mcc::FunctionPtr> mcc::Builder::FindCandidates(
        ResourceLocation location,
        const ParameterRefList &parameters) const
{
    if (!m_Functions.contains(location.Namespace))
        return {};

    if (!m_Functions.at(location.Namespace).contains(location.Path))
        return {};

    std::vector<FunctionPtr> collection;

    auto &functions = m_Functions.at(location.Namespace).at(location.Path);
    for (auto &function : functions)
    {
        if (parameters.size() != function->Parameters.size())
            continue;

        unsigned i;
        for (i = 0; i < parameters.size(); ++i)
        {
            if (!SameOrSpecial(parameters[i].Type, function->Parameters[i].Type))
                break;
            if (parameters[i].FieldType < function->Parameters[i].FieldType)
                break;
        }
        if (i < parameters.size())
            continue;

        collection.push_back(function);
    }

    return collection;
}

std::vector<mcc::FunctionPtr> mcc::Builder::FindCandidates(
        const std::string &name,
        const ParameterRefList &parameters) const
{
    std::vector<FunctionPtr> collection;
    for (auto &namespace_ : m_Functions | std::views::values)
        for (auto &[path_, functions_] : namespace_)
        {
            if (name != path_.back())
                continue;

            for (auto &function : functions_)
            {
                if (parameters.size() != function->Parameters.size())
                    continue;

                unsigned i;
                for (i = 0; i < parameters.size(); ++i)
                {
                    if (!SameOrSpecial(parameters[i].Type, function->Parameters[i].Type))
                        break;
                    if (parameters[i].FieldType < function->Parameters[i].FieldType)
                        break;
                }
                if (i < parameters.size())
                    continue;

                collection.push_back(function);
            }
        }
    return collection;
}

mcc::FunctionPtr mcc::Builder::FindUnambiguousCandidate(
        const SourceLocation &where,
        const std::vector<FunctionPtr> &candidates,
        const ParameterRefList &parameters) const
{
    Assert(!candidates.empty(), "candidates must not be empty");

    std::vector<FunctionPtr> functions;
    auto error = ~0u;

    for (auto &candidate : candidates)
    {
        auto candidate_error = 0u;

        unsigned i;
        for (i = 0; i < parameters.size(); ++i)
        {
            if (parameters[i].Type != candidate->Parameters[i].Type)
                candidate_error += 5u;
            candidate_error += parameters[i].FieldType - candidate->Parameters[i].FieldType;
        }

        if (candidate_error > error)
            continue;

        if (candidate_error == error)
        {
            functions.push_back(candidate);
            continue;
        }

        functions = { candidate };
        error     = candidate_error;
    }

    if (functions.size() == 1)
        return functions.front();

    std::vector<SourceLocation> sources(functions.size());
    for (unsigned i = 0; i < functions.size(); ++i)
        sources[i] = functions[i]->Where;

    Error(where, "ambiguous candidates {}", sources);
}
