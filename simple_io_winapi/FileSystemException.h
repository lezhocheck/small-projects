#pragma once
#include <stdexcept>
#include <Windows.h>

class FileSystemException : public std::exception {
public:
    FileSystemException(const char* what, const char* file,
                  const char* function, size_t line) {
        what_ = what;
        lastError_ = GetLastError();
        file_ = file;
        func_ = function;
        line_ = line;
    }

    [[nodiscard]] const char* what() const noexcept override {
        static std::string out = what_ + "\n  Last error: " +
                getLastErrorAsString() +
                "\n  file: " + file_ +
                "\n  function: " + func_ + "\n  line: " +
                std::to_string(line_) + "\n";
        return out.c_str();
    }

private:
    std::string what_;
    size_t lastError_;
    size_t line_;
    std::string file_;
    std::string func_;

    [[nodiscard]] std::string getLastErrorAsString() const {
        DWORD errorMessageID = lastError_;
        if(errorMessageID == 0) {
            return "No information";
        }

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, errorMessageID,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer, 0, nullptr);

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }
};

#define EXCEPT(what) throw FileSystemException(what, __FILE__, __FUNCTION__, __LINE__);