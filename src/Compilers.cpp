#include "Compilers.hpp"
#include "Utils.hpp"

#include <filesystem>


namespace Leo
{
    void ToolchainBase::SetSources(
        std::vector<std::string>& sources,
        std::vector<std::string>& headers)
    {
        mSourceFiles = sources;
        mHeaderFiles = headers;
    }

    void ToolchainBase::SetCompilerOptions(
        std::vector<std::string>& compilerFlags,
        std::vector<std::string>& compilerDefines,
        std::vector<std::string>& compilerIncludeDirectories)
    {
        mCompilerFlags = compilerFlags;
        mCompilerDefines = compilerDefines;
        mCompilerIncludeDirectories = compilerIncludeDirectories;
    }

    void ToolchainBase::SetLinkerOptions(
        std::vector<std::string>& linkerFlags,
        std::vector<std::string>& linkerLibraries,
        std::vector<std::string>& linkerIncludeDirectories)
    {
        mLinkerFlags = linkerFlags;
        mLinkerLibraries = linkerLibraries;
        mLinkerIncludeDirectories = linkerIncludeDirectories;
    }

    void ToolchainBase::SetupState()
    {
        if(!std::filesystem::exists("obj"))
            std::filesystem::create_directory("obj");
        
        if(!std::filesystem::exists("bin"))
            std::filesystem::create_directory("bin");
    }

    std::vector<std::string> ToolchainBase::Compile()
    {
        std::vector<std::string> command;
        std::vector<std::string> objectFiles;

        if(mSourceFiles.empty())
        {
            std::cout << "ERROR: Toolchain: No source files available\n";
            return objectFiles;
        }

        command.push_back("-c");

        for(std::string item : mCompilerFlags)
            command.push_back(item);

        for(std::string item : mCompilerDefines)
            command.push_back("-D" + item);

        for(std::string item : mCompilerIncludeDirectories)
            command.push_back("-I" + item);

        for(std::string file : mSourceFiles)
        {
            objectFiles.push_back("obj/" + Utils::StripFileName(file) + ".obj");
            std::cout << "Compiling: " << file << " > " << objectFiles.back() << "\n";

            command.push_back(file);
            command.push_back("-o");
            command.push_back(objectFiles.back());

            // run command

            command.erase(command.end() - 3, command.end());
        }

        return objectFiles;
    }

    void ToolchainBase::Link(std::string outFileName, std::vector<std::string>& objectFiles)
    {
        std::vector<std::string> command;

        if(objectFiles.empty())
        {
            std::cout << "ERROR: Toolchain: No object files available\n";
            return;
        }

        for(std::string item : mLinkerFlags)
            command.push_back(item);

        for(std::string item : mLinkerIncludeDirectories)
            command.push_back("-L" + item);

        for(std::string item : objectFiles)
            command.push_back(item);

        for(std::string item : mLinkerLibraries)
            command.push_back("-l" + item);

        std::cout << "Linking final executable\n";
        command.push_back("-o");
        command.push_back("bin/" + outFileName);

        // run command
        std::cout << "Saved final executable: \"" << outFileName << "\"\n";
    }


    // MinGW toolchain

    void ToolchainMinGW::SetupState()
    {
        if(!std::filesystem::exists("obj"))
            std::filesystem::create_directory("obj");
        
        if(!std::filesystem::exists("bin"))
            std::filesystem::create_directory("bin");
    }

    std::vector<std::string> ToolchainMinGW::Compile()
    {
        std::vector<std::string> command;
        std::vector<std::string> objectFiles;

        if(mSourceFiles.empty())
        {
            std::cout << "ERROR: Toolchain: No source files available\n";
            return objectFiles;
        }

        command.push_back("-c");

        for(std::string item : mCompilerFlags)
            command.push_back(item);

        for(std::string item : mCompilerDefines)
            command.push_back("-D" + item);

        for(std::string item : mCompilerIncludeDirectories)
            command.push_back("-I" + item);

        for(std::string file : mSourceFiles)
        {
            objectFiles.push_back("obj/" + Utils::StripFileName(file) + ".obj");
            std::cout << "Compiling: " << file << " > " << objectFiles.back() << "\n";

            command.push_back(file);
            command.push_back("-o");
            command.push_back(objectFiles.back());

            Utils::StartProcessAndWait("g++", command);

            command.erase(command.end() - 3, command.end());
        }

        return objectFiles;
    }

    void ToolchainMinGW::Link(std::string outFileName, std::vector<std::string>& objectFiles)
    {
        std::vector<std::string> command;

        if(objectFiles.empty())
        {
            std::cout << "ERROR: Toolchain: No object files available\n";
            return;
        }

        for(std::string item : mLinkerFlags)
            command.push_back(item);

        for(std::string item : mLinkerIncludeDirectories)
            command.push_back("-L" + item);

        for(std::string item : objectFiles)
            command.push_back(item);

        for(std::string item : mLinkerLibraries)
            command.push_back("-l" + item);

        std::cout << "Linking final executable\n";
        command.push_back("-o");
        command.push_back("bin/" + outFileName);

        Utils::StartProcessAndWait("g++", command);
        std::cout << "Saved final executable: \"" << outFileName << "\"\n";
    }


    // Compiler class

    void Compiler::SetSources(
        std::vector<std::string>& sources,
        std::vector<std::string>& headers)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetSources(sources, headers);
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetSources(sources, headers);
            break;
        }
    }

    void Compiler::SetCompilerOptions(
        std::vector<std::string>& compilerFlags,
        std::vector<std::string>& compilerDefines,
        std::vector<std::string>& compilerIncludeDirectories)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetCompilerOptions(compilerFlags, compilerDefines, compilerIncludeDirectories);
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetCompilerOptions(compilerFlags, compilerDefines, compilerIncludeDirectories);
            break;
        }
    }

    void Compiler::SetLinkerOptions(
        std::vector<std::string>& linkerFlags,
        std::vector<std::string>& linkerLibraries,
        std::vector<std::string>& linkerIncludeDirectories)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetLinkerOptions(linkerFlags, linkerLibraries, linkerIncludeDirectories);
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetLinkerOptions(linkerFlags, linkerLibraries, linkerIncludeDirectories);
            break;
        }
    }

    void Compiler::SetActiveToolchain(Toolchain option)
    {
        mActiveToolchain = option;
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetupState();
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetupState();
            break;
        }
    }

    std::vector<std::string> Compiler::Compile()
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            return mToolchainDummy.Compile();
            break;

        case Toolchain::MinGW:
            return mToolchainMinGW.Compile();
            break;
        }

        std::vector<std::string> empty;
        return empty;
    }

    void Compiler::Link(std::string outFileName, std::vector<std::string>& objectFiles)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            return mToolchainDummy.Link(outFileName, objectFiles);
            break;

        case Toolchain::MinGW:
            return mToolchainMinGW.Link(outFileName, objectFiles);
            break;
        }
    }
}