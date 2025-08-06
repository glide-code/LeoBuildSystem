#include "BuildSystem.hpp"
#include "Utils.hpp"

static std::string helpText =
"Usage: buildsystem [options] <projectfile.xml>\n"
"Options:\n"
"--help       - Display this help text\n"
"--verbose    - Enable extended verbosity\n"
"--version    - Display version information\n"
;

static std::string versionText =
"Leo build system (version 1.0.0)\n"
;

int main(int argc, char** argv)
{
    Leo::BuildSystem buildSystem;
    std::string fileToRead;

    if(argc < 2)
    {
        std::cout << helpText;
        return 0;
    }

    for(int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if(arg == "--help")
        {
            std::cout << helpText;
            return 0;
        }

        if(arg == "--version") 
        {
            std::cout << versionText;
            return 0;
        }

        if(arg == "--verbose")
        {
            buildSystem.SetVerbosity(Leo::BuildSystem::VerbosityLevel::Extended);
            continue;
        }

        if(Utils::PathExists(arg))
        {
            fileToRead = arg;
            continue;
        }
        else
        {
            std::cout << "Skipping unknown command: " << arg << "\n";
            continue;
        }
    }

    if(fileToRead.empty())
    {
        std::cout << "No project files to read\n";
        return 0;
    }

    std::cout << "------------[ Leo Build System ]------------\n";
    bool success = buildSystem.ReadProjectFile(fileToRead);
    if(success) buildSystem.StartBuild();

    return 0;
}