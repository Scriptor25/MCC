#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

#include <ranges>

mcc::FunctionPtr mcc::Builder::CreateFunction(
        const SourceLocation &where,
        ResourceLocation location,
        const ParameterList &parameters,
        const TypePtr &result,
        const bool throws)
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    return m_Module.CreateFunction(where, location, parameters, result, throws);
}

mcc::FunctionPtr mcc::Builder::GetFunction(
        ResourceLocation location,
        const ParameterList &parameters) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    return m_Module.GetFunction(location, parameters);
}

std::vector<mcc::FunctionPtr> mcc::Builder::FindFunctions(
        const ResourceLocation &location,
        const ParameterRefList &parameters) const
{
    std::vector<FunctionPtr> collection;
    for (auto &function : m_Module)
    {
        if (function->Location != location)
            continue;

        if (parameters.size() != function->Parameters.size())
            continue;

        unsigned i;
        for (i = 0; i < parameters.size(); ++i)
        {
            if (!SameOrSpecialization(parameters[i].Type, function->Parameters[i].Type))
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

std::vector<mcc::FunctionPtr> mcc::Builder::FindFunctions(
        const std::vector<std::string> &path,
        const ParameterRefList &parameters,
        const bool use_namespace) const
{
    if (use_namespace)
        return FindFunctions({ m_Namespace, { path } }, parameters);

    std::vector<FunctionPtr> collection;
    for (auto &function : m_Module)
    {
        if (function->Location.Path != path)
            continue;

        if (parameters.size() != function->Parameters.size())
            continue;

        unsigned i;
        for (i = 0; i < parameters.size(); ++i)
        {
            if (!SameOrSpecialization(parameters[i].Type, function->Parameters[i].Type))
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

std::vector<mcc::FunctionPtr> mcc::Builder::FindFunctions(const ResourceLocation &location) const
{
    std::vector<FunctionPtr> collection;
    for (auto &function : m_Module)
    {
        if (function->Location != location)
            continue;

        collection.push_back(function);
    }
    return collection;
}

std::vector<mcc::FunctionPtr> mcc::Builder::FindFunctions(
        const std::vector<std::string> &path,
        const bool use_namespace) const
{
    if (use_namespace)
        return FindFunctions({ m_Namespace, { path } });

    std::vector<FunctionPtr> collection;
    for (auto &function : m_Module)
    {
        if (function->Location.Path != path)
            continue;

        collection.push_back(function);
    }
    return collection;
}

mcc::FunctionPtr mcc::Builder::FindUnambiguousCandidate(
        const SourceLocation &where,
        const std::vector<FunctionPtr> &candidates,
        const ParameterRefList &parameters)
{
    Assert(!candidates.empty(), where, "candidates must not be empty");

    std::vector<FunctionPtr> functions;
    auto error = ~0u;

    for (auto &candidate : candidates)
    {
        auto candidate_error = 0u;

        for (unsigned i = 0; i < parameters.size(); ++i)
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
