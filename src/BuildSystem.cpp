#include "BuildSystem.hpp"
#include "Compilers.hpp"
#include "ext/tinyxml2/tinyxml2.h"
#include "Utils.hpp"
using namespace tinyxml2;

namespace Leo
{
    bool BuildSystem::ReadProjectFile(std::string filepath)
    {
        if(!VerifyProjectStructure(filepath))
            return false;

        XMLDocument doc;
        XMLElement* project = nullptr;
        XMLElement* sources = nullptr;
        XMLElement* headers = nullptr;
        XMLElement* compilerFlags = nullptr;
        XMLElement* compilerDefines = nullptr;
        XMLElement* compilerIncludeDirectories = nullptr;
        XMLElement* linkerFlags = nullptr;
        XMLElement* linkerLibraries = nullptr;
        XMLElement* linkerIncludeDirectories = nullptr;
        XMLElement* item = nullptr;

        doc.LoadFile(filepath.c_str());

        project = doc.FirstChildElement("Project");
        mProjectName = project->Attribute("Name");

        if(mProjectName.empty())
        {
            std::cout << "WARNING: BuildSystem: Empty project name. Using default project name 'DUMMY'\n";
            mProjectName = "DUMMY";
        }

        sources = project->FirstChildElement("Sources");
        item = sources->FirstChildElement("Item");
        if(item != nullptr)
        {
            do
            {
                std::string text = item->GetText();
                mSourceFiles.push_back(text);
            } while((item = item->NextSiblingElement()) != nullptr);
        }

        if(mSourceFiles.empty())
            std::cout << "ERROR: BuildSystem: No source files provided\n";

        headers = project->FirstChildElement("Headers");
        item = headers->FirstChildElement("Item");
        if(item != nullptr)
        {
            do
            {
                std::string text = item->GetText();
                mHeaderFiles.push_back(text);
            } while((item = item->NextSiblingElement()) != nullptr);
        }


        compilerFlags = project->FirstChildElement("CompilerOptions")->FirstChildElement("Flags");
        if(compilerFlags != nullptr)
        {
            item = compilerFlags->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mCompilerFlags.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        compilerDefines = project->FirstChildElement("CompilerOptions")->FirstChildElement("Defines");
        if(compilerDefines != nullptr)
        {
            item = compilerDefines->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mCompilerDefines.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        compilerIncludeDirectories = project->FirstChildElement("CompilerOptions")->FirstChildElement("Include");
        if(compilerIncludeDirectories != nullptr)
        {
            item = compilerIncludeDirectories->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mCompilerIncludeDirectories.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        linkerFlags = project->FirstChildElement("LinkerOptions")->FirstChildElement("Flags");
        if(linkerFlags != nullptr)
        {
            item = linkerFlags->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mLinkerFlags.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        linkerLibraries = project->FirstChildElement("LinkerOptions")->FirstChildElement("Libraries");
        if(linkerLibraries != nullptr)
        {
            item = linkerLibraries->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mLinkerLibraries.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        linkerIncludeDirectories = project->FirstChildElement("LinkerOptions")->FirstChildElement("Include");
        if(linkerIncludeDirectories != nullptr)
        {
            item = linkerIncludeDirectories->FirstChildElement("Item");
            if(item != nullptr)
            {
                do
                {
                    std::string text = item->GetText();
                    mLinkerIncludeDirectories.push_back(text);
                } while((item = item->NextSiblingElement()) != nullptr);
            }
        }

        return true;
    }

    void BuildSystem::StartBuild()
    {
        DisplayBuildInfo();

        Compiler compiler;
        compiler.SetActiveToolchain(Compiler::Toolchain::MinGW);
        compiler.SetSources(mSourceFiles, mHeaderFiles);
        compiler.SetCompilerOptions(mCompilerFlags, mCompilerDefines, mCompilerIncludeDirectories);
        compiler.SetLinkerOptions(mLinkerFlags, mLinkerLibraries, mLinkerIncludeDirectories);
        std::vector<std::string> objects = compiler.Compile();
        compiler.Link(mProjectName, objects);
    }

    void BuildSystem::DisplayBuildInfo()
    {
        std::cout << "------------[ Build Started ]------------\n";
        std::cout << "Project name: " << mProjectName << "\n";

        std::cout << "Sources:\n";
        for(std::string& source : mSourceFiles)
            std::cout << "\t" << source << "\n";

        std::cout << "Headers:\n";
        for(std::string& header : mHeaderFiles)
            std::cout << "\t" << header << "\n";
        
        std::cout << "Compiler flags: ";
        for(std::string& option : mCompilerFlags)
            std::cout << option << " ";
        std::cout << "\n";

        std::cout << "Compiler include directories: ";
        for(std::string& option : mCompilerIncludeDirectories)
            std::cout << option << " ";
        std::cout << "\n";

        std::cout << "Compiler defines: ";
        for(std::string& option : mCompilerDefines)
            std::cout << option << " ";
        std::cout << "\n";

        std::cout << "Linker flags: ";
        for(std::string& option : mLinkerFlags)
            std::cout << option << " ";
        std::cout << "\n";

        std::cout << "Linker include directories: ";
        for(std::string& option : mLinkerIncludeDirectories)
            std::cout << option << " ";
        std::cout << "\n";

        std::cout << "Linker libraries: ";
        for(std::string& option : mLinkerLibraries)
            std::cout << option << " ";
        std::cout << "\n";
    }

    bool BuildSystem::VerifyProjectStructure(std::string filepath)
    {
        bool result = true;

        XMLDocument doc;
        XMLElement* project = nullptr;

        doc.LoadFile(filepath.c_str());
        if(doc.Error())
        {
            std::cout << "ERROR: BuildSystem: Failed to parse project file: " << filepath << "\n";
            std::cout << "Try checking the XML syntax and structure\n";
            result = false;
            return result;
        }

        project = doc.FirstChildElement("Project");

        if(project->Attribute("Name") == nullptr)
        {
            std::cout << "ERROR: BuildSystem: 'Name' attribute in 'Project' node doesn't exist\n";
            result = false;
        }

        if(project->FirstChildElement("Sources") == nullptr)
        {
            std::cout << "ERROR: BuildSystem: 'Sources' node structure doesn't exist\n";
            result = false;
        }

        if(project->FirstChildElement("Headers") == nullptr)
        {
            std::cout << "ERROR: BuildSystem: 'Headers' node structure doesn't exist\n";
            result = false;
        }

        if(project->FirstChildElement("CompilerOptions") == nullptr)
        {
            std::cout << "ERROR: BuildSystem: 'CompilerOptions' node structure doesn't exist\n";
            result = false;
        }

        if(project->FirstChildElement("LinkerOptions") == nullptr)
        {
            std::cout << "ERROR: BuildSystem: 'LinkerOptions' node structure doesn't exist\n";
            result = false;
        }

        return result;
    }
}