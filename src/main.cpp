#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <mcc/actions.hpp>
#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/package.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

static void parse_file(mcc::Package &package, const std::filesystem::path &path)
{
    std::ifstream stream(path);
    mcc::Assert(stream.is_open(), "failed to open source file {}", path.string());

    mcc::Context context(package, {});
    mcc::Parser parser(context, stream, path.string());
    mcc::Builder builder(context);

    while (parser)
        if (const auto statement = parser())
            statement->Generate(builder, {});

    builder.Generate();
}

static void parse_directory(mcc::Package &package, const std::filesystem::path &path)
{
    for (auto &entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            parse_directory(package, entry.path());
            continue;
        }

        if (entry.path().extension() != ".mcc")
            continue;

        parse_file(package, entry.path());
    }
}

int main(const int argc, const char **argv)
{
    // https://minecraft.fandom.com/wiki/Argument_types
    // https://minecraft.fandom.com/wiki/Commands
    // https://minecraft.fandom.com/wiki/Data_pack

    mcc::Actions actions(
        {
            {
                0,
                "help",
                "display help text and program description"
            },
            // mcc init [-name <package name>] [-version <package version>] [-description <package description>] -> initialize a new package with a given name, version and description
            {
                1,
                "init",
                "initialize a new package",
                {
                    {false, "-name", "package name (default: 'example')"},
                    {false, "-description", "package description (default: 'the example package')"},
                    {false, "-version", "package version (default: '71')"}
                }
            },
            // mcc compile [-pkg <package file>] [-target <target directory>] -> compile a package to a target directory
            {
                2,
                "compile",
                "compile a package into the target directory",
                {
                    {false, "-pkg", "package file (default: 'info.json')"},
                    {false, "-target", "target directory (default: 'target')"}
                }
            },
            // mcc package [-pkg <package file>] [-target <target directory>] [-destination <destination file name>] -> package a package into a zip destination file
            {
                3,
                "package",
                "compress a package into a zip file, into the target directory",
                {
                    {false, "-pkg", "package file (default: 'info.json')"},
                    {false, "-target", "taget directory (default: 'target')"},
                    {false, "-destination", "destination file name (default: '<package name>.zip')"}
                }
            }
        });
    actions(argc, argv);

    switch (actions.ActionID())
    {
        case 1: // init
        {
            std::string name = "package";
            std::string description = "the package description";
            std::string version = "71";

            (void) actions.String(0, name);
            (void) actions.String(1, description);
            (void) actions.String(2, version);

            mcc::PackageInfo{
                .Name = name,
                .Description = description,
                .Version = std::stoul(version),
            }.Serialize(std::filesystem::path(name) / "info.json");

            std::filesystem::create_directories(std::filesystem::path(name) / "src");

            break;
        }

        case 2: // compile
        {
            std::string pkg = "info.json";
            std::string target = "target";

            (void) actions.String(0, pkg);
            (void) actions.String(1, target);

            auto info = mcc::PackageInfo::Deserialize(pkg);
            mcc::Package package(info);

            mcc::Assert(std::filesystem::exists("src"), "source directory does not exist");

            parse_directory(package, "src");

            package.Write(target);

            break;
        }

        case 3: // package
        {
            // TODO: read 'info.json'
            // TODO: compile files in 'src'
            // TODO: write output to 'target'
            // TODO: compress 'target' into 'package.zip'
            break;
        }

        case 0: // help
            actions.Print();
            break;

        default:
            std::cerr << "no valid action. see 'help' for how to use." << std::endl;
            return 1;
    }

    return 0;
}
