#pragma once
#include <string>
#include <vector>
#include "FileSystemException.h"

class Path {
public:
    const static wchar_t DIRECTORY_SEP = '\\';
    const static wchar_t ALT_DIRECTORY_SEP = '/';
    const static wchar_t PATH_SEP = ';';
    const static wchar_t VOLUME_SEP = ':';

    explicit Path(const std::wstring& path);
    Path();

    [[nodiscard]] std::wstring getParent() const;
    [[nodiscard]] Path join(const std::wstring& other) const;
    [[nodiscard]] std::wstring getFileName() const;
    [[nodiscard]] std::wstring getDirectoryName() const;
    [[nodiscard]] std::wstring getExtension() const;
    [[nodiscard]] std::wstring absolute() const;

    [[nodiscard]] bool isFile() const;
    static std::vector<wchar_t> GetInvalidPathChars();

private:
    struct Tokens {
        std::wstring volume;
        std::vector<std::wstring> pathValues;
        std::wstring fileName;
        std::wstring fileExtension;
        [[nodiscard]] bool empty() const;
        bool isValid();
    };

    Tokens tokens;
    static Tokens tokenize(const std::wstring& value);
};