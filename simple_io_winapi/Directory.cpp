#include <windows.h>
#include <stdexcept>
#include "Directory.h"

Path Directory::GetCurrent() {
    wchar_t path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, path);
    return Path(std::wstring(path));
}

void Directory::SetCurrent(const Path& path) {
    if (!Directory::Exists(path) || path.isFile()) {
        EXCEPT("Directory does not exist")
    }

    SetCurrentDirectoryW(path.absolute().c_str());
}

bool Directory::Exists(const Path& path) {
    return ModuleBase::Exists(path, ModuleType::Directory);
}

void Directory::Create(const Path& path) {
    if (Directory::Exists(path)) {
        EXCEPT("Directory already exists")
    }
    if (path.isFile()) {
        EXCEPT("Directory does not exist")
    }

    bool result = CreateDirectoryW(path.absolute().c_str(), nullptr);
    if (!result) {
        EXCEPT("Cannot create directory")
    }
}

void Directory::getObjects(std::vector<Path>& objects, const Path& path,
                unsigned type, bool recursive) {
    std::wstring tmp = path.absolute() + L"*";
    WIN32_FIND_DATAW file;
    HANDLE searchHandle = FindFirstFileW(tmp.c_str(), &file);
    if (searchHandle != INVALID_HANDLE_VALUE) {
        std::vector<Path> directories;
        do {
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!lstrcmpW(file.cFileName, L".") ||
                    !lstrcmpW(file.cFileName, L"..")) {
                    continue;
                }
            }

            bool condition = file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            if (type) {
                condition = !condition;
            }
            tmp = path.absolute() + std::wstring(file.cFileName);
            if (condition) {
                objects.emplace_back(tmp);
            }

            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                directories.emplace_back(tmp);
            }
        } while (FindNextFileW(searchHandle, &file));

        FindClose(searchHandle);

        if (recursive) {
            for (const Path& tPath : directories) {
                getObjects(objects, tPath, type, recursive);
            }
        }
    }
}

Directory::Directory(const Path& path) : ModuleBase(path, ModuleType::Directory) {
    if (path.isFile()) {
        EXCEPT("Directory does not exist")
    }
}

std::vector<Path> Directory::getFiles(bool recursive) const {
    std::vector<Path> result;
    getObjects(result, modulePath, 1, recursive);
    return result;
}

std::vector<Path> Directory::getDirectories(bool recursive) const {
    std::vector<Path> result;
    getObjects(result, modulePath, 0, recursive);
    return result;
}

void Directory::removeEmpty() {
    std::wstring path = modulePath.absolute();

    if (!RemoveDirectoryW(path.c_str())) {
        int a = GetLastError();
        EXCEPT("Cannot delete directory");
    }
    modulePath = Path();
}

