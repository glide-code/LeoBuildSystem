#ifndef BUILDSYSTEM_H_
#define BUILDSYSTEM_H_

#include <vector>
#include <string>

namespace Leo
{
    class BuildSystem
    {
    public:
        BuildSystem() = default;
        ~BuildSystem() = default;

        bool ReadProjectFile(std::string filepath);
        void StartBuild();
        void DisplayBuildInfo();

    private:
        std::string mProjectName;

        std::vector<std::string> mSourceFiles;
        std::vector<std::string> mHeaderFiles;
        std::vector<std::string> mCompilerFlags;
        std::vector<std::string> mCompilerDefines;
        std::vector<std::string> mCompilerIncludeDirectories;

        std::vector<std::string> mLinkerFlags;
        std::vector<std::string> mLinkerLibraries;
        std::vector<std::string> mLinkerIncludeDirectories;

        bool VerifyProjectStructure(std::string filepath);
    };
}

#endif // BUILDSYSTEM_H_