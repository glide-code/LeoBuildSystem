#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace Utils
{
    void StartProcessAndWait(std::string program, const std::vector<std::string>& args);
    
    inline std::string NormalizePath(std::string text)
    {
        // Change backslash to forward slash
        #ifdef _WIN32
            for(int i = 0; i < static_cast<int>(text.length()); i++)
                if(text[i] == '\\') text[i] = '/';
        #endif

        return text;
    }

    inline std::string StripFileName(std::string text)
    {
        text = NormalizePath(text);
        std::string::size_type pos = text.find_last_of('/');
        if(pos == std::string::npos) return text;
        return text.substr(text.find_last_of('/') + 1);
    }

    inline std::string StripFilePath(std::string text)
    {
        text = NormalizePath(text);
        std::string::size_type pos = text.find_last_of('/');
        if(pos == std::string::npos) return text;
        return text.substr(0, text.find_last_of('/'));
    }

    inline std::string GetAbsolutePath(std::string text)
    {
        return std::filesystem::absolute(text).string();
    }

    inline bool PathExists(std::string path)
    {
        return std::filesystem::exists(path);
    }

    inline bool CreateDirectory(std::string path)
    {
        return std::filesystem::create_directory(path);
    }

    inline std::filesystem::file_time_type GetFileModifiedTime(std::string path)
    {
        // It is slower than platform specific methods
        // We are trading speed with portability
        return std::filesystem::last_write_time(path);
    }

    inline bool FileModified(std::string path, std::filesystem::file_time_type referencePoint)
    {
        std::filesystem::file_time_type fileTime = GetFileModifiedTime(path);
        std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(fileTime - referencePoint);
        return (duration.count() > 0);
    }

}

#endif // UTILS_H_