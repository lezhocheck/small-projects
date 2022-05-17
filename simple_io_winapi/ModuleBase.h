#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include "Path.h"
#include "FileSystemException.h"

class ModuleBase {
public:
    static std::wstring getTimeOutputFormatString();

    enum class ModuleType {
        File,
        Directory
    };

    explicit ModuleBase(const Path& path, ModuleType moduleType);
    explicit ModuleBase(const std::wstring& path, ModuleType moduleType) :
        ModuleBase(Path(path), moduleType) { }
    void move(const Path& path);
    void rename(const std::wstring& newName);
    [[nodiscard]] std::wstring getParent() const;
    [[nodiscard]] std::wstring getCreationTime() const;
    [[nodiscard]] std::wstring getLastAccessTime() const;
    [[nodiscard]] std::wstring getLastWriteTime() const;

protected:
    static bool Exists(const Path& path, ModuleType moduleType);
    Path modulePath;

    class Handler {
    public:
        explicit Handler(const Path& path, size_t attributes);
        [[nodiscard]] HANDLE get() const;
        ~Handler();

    private:
        HANDLE handle;
    };

private:
    static std::wstring convertFromFileTime(FILETIME filetime, const std::wstring& pattern);

    struct TimeStrings {
        std::wstring creationTimeS;
        std::wstring lastAccessTimeS;
        std::wstring lastWriteTimeS;
    };

    [[nodiscard]] TimeStrings getTime() const;
};