#ifndef COMPILERS_H_
#define COMPILERS_H_

#include <vector>
#include <string>
#include <sys/types.h>

namespace Leo
{
    class ToolchainBase
    {
    public:
        ToolchainBase() = default;
        ~ToolchainBase() = default;

        void SetProjectInfo(
            std::string projectRootDir,
            std::string projectCacheDir);

        void SetSources(
            std::vector<std::string>& sources,
            std::vector<std::string>& headers);

        void SetCompilerOptions(
            std::vector<std::string>& compilerFlags,
            std::vector<std::string>& compilerDefines,
            std::vector<std::string>& compilerIncludeDirectories);

        void SetLinkerOptions(
            std::vector<std::string>& linkerFlags,
            std::vector<std::string>& linkerLibraries,
            std::vector<std::string>& linkerIncludeDirectories);

        // Set to true to recompile entire project
        void SetCleanFlag(bool option);

        virtual bool SetupState();
        virtual std::vector<std::string> Compile();
        virtual void Link(std::string outFileName, std::vector<std::string>& objectFiles);

        virtual std::vector<std::string> ExamineSources();
        virtual void MakeDependencyTree(std::string depsData, std::vector<std::string>& depsOut);


    protected:
        std::string mName = "Dummy Compiler";
        std::string mProjectRootDir;
        std::string mProjectCacheDir;

        std::vector<std::string> mSourceFiles;
        std::vector<std::string> mHeaderFiles;
        std::vector<std::string> mCompilerFlags;
        std::vector<std::string> mCompilerDefines;
        std::vector<std::string> mCompilerIncludeDirectories;

        std::vector<std::string> mLinkerFlags;
        std::vector<std::string> mLinkerLibraries;
        std::vector<std::string> mLinkerIncludeDirectories;

        bool mCleanBuild;
    };

    class ToolchainMinGW : public ToolchainBase
    {
    public:
        ToolchainMinGW() = default;
        ~ToolchainMinGW() = default;

        bool SetupState() override;
        std::vector<std::string> Compile() override;
        void Link(std::string outFileName, std::vector<std::string>& objectFiles) override;

        std::vector<std::string> ExamineSources() override;
        void MakeDependencyTree(std::string depsData, std::vector<std::string>& depsOut) override;

    protected:
        std::string mName = "MinGW";
    };

    class Compiler
    {
    public:
        Compiler() = default;
        ~Compiler() = default;

        enum class Toolchain
        {
            Dummy,
            MinGW
        };

        void SetProjectInfo(
            std::string projectRootDir,
            std::string projectCacheDir);

        void SetSources(
            std::vector<std::string>& sources,
            std::vector<std::string>& headers);

        void SetCompilerOptions(
            std::vector<std::string>& compilerFlags,
            std::vector<std::string>& compilerDefines,
            std::vector<std::string>& compilerIncludeDirectories);

        void SetLinkerOptions(
            std::vector<std::string>& linkerFlags,
            std::vector<std::string>& linkerLibraries,
            std::vector<std::string>& linkerIncludeDirectories);

        void SetActiveToolchain(Toolchain option);
        void SetCleanFlag(bool option);

        std::vector<std::string> Compile();
        void Link(std::string outFileName, std::vector<std::string>& objectFiles);

    private:
        Toolchain mActiveToolchain = Toolchain::Dummy;

        ToolchainBase mToolchainDummy;
        ToolchainMinGW mToolchainMinGW;
    };
}

#endif // COMPILERS_H_