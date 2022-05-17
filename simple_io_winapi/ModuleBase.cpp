#include <stdexcept>
#include <tchar.h>
#include "ModuleBase.h"

std::wstring ModuleBase::getTimeOutputFormatString() {
    return std::wstring(L"%02d/%02d/%d  %02d:%02d:%02d");
}

bool ModuleBase::Exists(const Path &path, ModuleBase::ModuleType moduleType) {
    DWORD dwAttrib = GetFileAttributesW(path.absolute().c_str());
    bool check;

    if (moduleType == ModuleType::File) {
        check = !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
    } else if (moduleType == ModuleType::Directory) {
        check = dwAttrib & FILE_ATTRIBUTE_DIRECTORY;
    }

    return dwAttrib != INVALID_FILE_ATTRIBUTES && check;
}

ModuleBase::ModuleBase(const Path& path, ModuleType moduleType) {
    if (!ModuleBase::Exists(path, moduleType)) {
        EXCEPT("Module object does not exist")
    }
    modulePath = path;
}

std::wstring ModuleBase::getParent() const {
    try {
        return modulePath.getParent();
    } catch (std::runtime_error& e) {
        return modulePath.absolute();
    }
}

std::wstring ModuleBase::convertFromFileTime(FILETIME filetime, const std::wstring& pattern) {
    SYSTEMTIME stUTC, stLocal;

    FileTimeToSystemTime(&filetime, &stUTC);
    SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);
    wchar_t buffer[pattern.size()];
    swprintf(buffer, pattern.size(), pattern.c_str(),
             stLocal.wMonth, stLocal.wDay, stLocal.wYear,
             stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
    return std::wstring(buffer);
}

ModuleBase::TimeStrings ModuleBase::getTime() const {
    Handler handler(modulePath, OPEN_EXISTING);
    FILETIME creationTime, lastAccessTime, lastWriteTime;
    GetFileTime(handler.get(), &creationTime, &lastAccessTime, &lastWriteTime);
    TimeStrings ts;
    std::wstring timeOutputFormatString = ModuleBase::getTimeOutputFormatString();
    ts.creationTimeS = convertFromFileTime(creationTime, timeOutputFormatString);
    ts.lastAccessTimeS = convertFromFileTime(lastAccessTime, timeOutputFormatString);
    ts.lastWriteTimeS = convertFromFileTime(lastWriteTime, timeOutputFormatString);
    return ts;
}
std::wstring ModuleBase::getCreationTime() const {
    return getTime().creationTimeS;
}

std::wstring ModuleBase::getLastAccessTime() const {
    return getTime().lastAccessTimeS;
}

std::wstring ModuleBase::getLastWriteTime() const {
    return getTime().lastWriteTimeS;
}

void ModuleBase::move(const Path& path) {
    if (path.isFile()) {
        EXCEPT("Cannot move module object")
    }

    std::wstring moduleName;
    if (modulePath.isFile()) {
        moduleName = modulePath.getFileName() + L"." + modulePath.getExtension();
    } else {
        moduleName = modulePath.getDirectoryName();
    }
    Path newPath = Path(path.absolute() + Path::DIRECTORY_SEP + moduleName);
    if (!MoveFileExW(modulePath.absolute().c_str(),
                     newPath.absolute().c_str(),
                     MOVEFILE_WRITE_THROUGH)) {
        EXCEPT("Module object does not exist");
    }
    modulePath = newPath;
}

void ModuleBase::rename(const std::wstring& newName) {
    Path newPath = Path(modulePath.getParent() + newName);
    if (!MoveFileExW(modulePath.absolute().c_str(),
                     newPath.absolute().c_str(),
                     MOVEFILE_WRITE_THROUGH)) {
        EXCEPT("Cannot rename module object");
    }
    modulePath = newPath;
}

ModuleBase::Handler::Handler(const Path &path, size_t attributes) {
    handle = CreateFileW(path.absolute().c_str(),
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ, nullptr,
                                attributes, FILE_FLAG_BACKUP_SEMANTICS,
                                nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        EXCEPT("Cannot operate with module object")
    }
}

HANDLE ModuleBase::Handler::get() const {
    return handle;
}

ModuleBase::Handler::~Handler() {
    CloseHandle(handle);
}
