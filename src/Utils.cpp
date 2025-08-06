#include "Utils.hpp"
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace Utils
{
#ifdef _WIN32

    void StartProcessAndWait(std::string program, const std::vector<std::string>& args)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

        char argv[32767]; // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa

        strcat_s(argv, sizeof(argv), program.c_str());
        for(int i = 0; i < static_cast<int>(args.size()); i++)
        {
            strcat_s(argv, sizeof(argv), " ");
            strcat_s(argv, sizeof(argv), args[i].c_str());
        }

        if(!CreateProcessA(program.c_str(), argv, NULL, NULL, FALSE, 0, NULL, NULL, &si &pi))
        {
            std::cout << "CreateProcess failed: " << GetLastError() << "\n";
            return;
        }

        WaitForSingleObject( pi.hProcess, INFINITE );

        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }

#else
    
    void StartProcessAndWait(std::string program, const std::vector<std::string>& args)
    {
        unsigned int size = args.size() + 2;
        char* argv[size];

        argv[0] = const_cast<char*>(program.c_str());
        for(int i = 0; i < static_cast<int>(args.size()); i++)
            argv[i+1] = const_cast<char*>(args[i].c_str());
        argv[args.size() + 1] = nullptr;

        pid_t pid;
        int status;
        pid = fork();

        switch(pid)
        {
        case -1:
            std::cout << "Failed to fork process\n";
            break;

        case 0:
            execvp(program.c_str(), argv);
            std::cout << "Failed to execute \"" << program << "\"\n";
            exit(EXIT_FAILURE);
            break;

        default:
            wait(&status);
            break;
        }
    }

#endif

}