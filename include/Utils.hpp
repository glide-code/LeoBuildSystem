#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

namespace Utils
{
    inline void StartProcessAndWait(std::string program, const std::vector<std::string>& args)
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

    inline std::string StripFileName(std::string text)
    {
        std::string::size_type pos = text.find_last_of('/');
        if(pos == std::string::npos) return text;
        return text.substr(text.find_last_of('/') + 1);
    }
}

#endif // UTILS_H_