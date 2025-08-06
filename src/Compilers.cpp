#include "Compilers.hpp"
#include "Utils.hpp"

#include <sstream>

static std::filesystem::file_time_type referenceTime;

namespace Leo
{
    void ToolchainBase::SetProjectInfo(
        std::string projectRootDir,
        std::string projectCacheDir)
    {
        mProjectRootDir = projectRootDir;
        mProjectCacheDir = projectCacheDir;
        referenceTime = Utils::GetFileModifiedTime(mProjectCacheDir + "/reference");
    }

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

    void ToolchainBase::SetCleanFlag(bool option)
    {
        mCleanBuild = option;
    }

    bool ToolchainBase::SetupState()
    {
        if(!Utils::PathExists("./obj"))
            Utils::CreateDirectory("./obj");
        
        if(!Utils::PathExists("./bin"))
            Utils::CreateDirectory("./bin");

        return true;
    }

    std::vector<std::string> ToolchainBase::ExamineSources()
    {
        // Nothing :)
        return mSourceFiles;
    }

    void ToolchainBase::MakeDependencyTree(std::string depsData, std::vector<std::string>& depsOut)
    {
        // Nothing :)
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
            objectFiles.push_back("./obj/" + Utils::StripFileName(file) + ".obj");
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

    bool ToolchainMinGW::SetupState()
    {
        if(!Utils::PathExists("./obj"))
            Utils::CreateDirectory("./obj");
        
        if(!Utils::PathExists("./bin"))
            Utils::CreateDirectory("./bin");

        return true;
    }

    std::vector<std::string> ToolchainMinGW::ExamineSources()
    {
        std::vector<std::string> changedFiles;

        std::vector<std::string> command;
        command.push_back("-M");
        command.push_back("-MT");
        command.push_back("a");
        command.push_back("-MF");
        command.push_back(mProjectCacheDir + "/out.dep");

        for(std::string item : mCompilerDefines)
            command.push_back("-D" + item);

        for(std::string item : mCompilerIncludeDirectories)
            command.push_back("-I" + item);

        command.push_back("-E");

        for(const std::string& source : mSourceFiles)
        {
            // Make a list of dependencies            
            command.push_back(source);
            Utils::StartProcessAndWait("g++", command);
            command.pop_back();

            std::string buf;
            std::ifstream file(mProjectCacheDir + "/out.dep");
            while(!file.eof())
            {
                std::string tmp;
                std::getline(file, tmp);
                buf += tmp;
            }
            file.close();

            // Trim unnecessary parts from 'buf'
            std::string leftTrim = "a: " + source + " ";
            buf = buf.substr(leftTrim.length());
            
            std::vector<std::string> deps;
            MakeDependencyTree(buf, deps);

            // Check if the source file itself has changed
            if(Utils::FileModified(source, referenceTime))
            {
                changedFiles.push_back(source);
                continue;
            }

            // Chech if its dependencies have changed
            for(const std::string& path : deps)
            {
                if(Utils::FileModified(path, referenceTime))
                {
                    changedFiles.push_back(source);
                    // We only need a single changed file to recompile the source
                    // It's practically worthless to continue checking any further
                    break;
                }
            }
        }

        changedFiles.shrink_to_fit();
        return changedFiles;
    }

    void ToolchainMinGW::MakeDependencyTree(std::string depsData, std::vector<std::string>& depsOut)
    {
        // Copy raw data into string stream
        std::stringstream data(depsData);
        depsOut.clear();

        // Usually enough for most jobs
        depsOut.reserve(10);

        while(!data.eof())
        {
            // Get each file path separated by whitespace
            std::string tmp;
            data >> tmp;

            // Makefile rules can be split into multiple lines by '\' character
            // While breaking whitespaces, "\" is also returned as a string sequence
            // We don't need it so get rid of it
            if(tmp == "\\") continue;

            // Some paths may contain necessary whitespaces such as this:
            // /home/someone/some long folder name/some other folder/file
            // This will get split int "/home/someone/some", "long", "folder",
            // "name/some", "other" and "folder/file"
            // For such cases, g++ appends a '\' character before whitespace:
            // /home/someone/some\ long\ folder\ name/some\ other\ folder/file
            // If such case is encountered we keep appending further string
            // sequences into current one
            while(tmp.back() == '\\')
            {
                tmp.back() = ' ';
                std::string tmp2;
                data >> tmp2;
                tmp += tmp2;
            }
            depsOut.push_back(tmp);
        }

        // Trim off extra memory at the end
        depsOut.shrink_to_fit();
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

        std::cout << "Checking dependencies...\n";
        std::vector<std::string> changedFiles;
        if(!mCleanBuild)
        {
            changedFiles = ExamineSources();
            if(changedFiles.empty())
            {
                std::cout << "All files are up to date\n";
                return objectFiles;
            }
        }

        command.push_back("-c");

        for(std::string item : mCompilerFlags)
            command.push_back(item);

        for(std::string item : mCompilerDefines)
            command.push_back("-D" + item);

        for(std::string item : mCompilerIncludeDirectories)
            command.push_back("-I" + item);

        if(mCleanBuild)
        {
            for(std::string& file : mSourceFiles)
            {
                objectFiles.push_back("./obj/" + Utils::StripFileName(file) + ".obj");
                std::cout << "Compiling: " << file << " > " << objectFiles.back() << "\n";

                command.push_back(file);
                command.push_back("-o");
                command.push_back(objectFiles.back());

                Utils::StartProcessAndWait("g++", command);

                command.erase(command.end() - 3, command.end());
            }
        }
        else
        {
            for(std::string& file : changedFiles)
            {
                objectFiles.push_back("./obj/" + Utils::StripFileName(file) + ".obj");
                std::cout << "Compiling: " << file << " > " << objectFiles.back() << "\n";

                command.push_back(file);
                command.push_back("-o");
                command.push_back(objectFiles.back());

                Utils::StartProcessAndWait("g++", command);

                command.erase(command.end() - 3, command.end());
            }

            objectFiles.clear();

            for(std::string& file : mSourceFiles)
                objectFiles.push_back("obj/" + Utils::StripFileName(file) + ".obj");
        }        

        return objectFiles;
    }

    void ToolchainMinGW::Link(std::string outFileName, std::vector<std::string>& objectFiles)
    {
        std::vector<std::string> command;

        if(objectFiles.empty())
        {
            std::cout << "No object files available to link\n";
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

    void Compiler::SetProjectInfo(
        std::string projectRootDir,
        std::string projectCacheDir)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetProjectInfo(projectRootDir, projectCacheDir);
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetProjectInfo(projectRootDir, projectCacheDir);
            break;
        }
    }

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

    void Compiler::SetCleanFlag(bool option)
    {
        switch(mActiveToolchain)
        {
        case Toolchain::Dummy:
            mToolchainDummy.SetCleanFlag(option);
            break;

        case Toolchain::MinGW:
            mToolchainMinGW.SetCleanFlag(option);
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