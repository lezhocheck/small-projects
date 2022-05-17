#include "File.h"

bool File::Exists(const Path& path) {
    return ModuleBase::Exists(path, ModuleType::File);
}

void File::Create(const Path& path) {
    if (!path.isFile()) {
        EXCEPT("Path is invalid")
    }

    if (File::Exists(path)) {
        EXCEPT("Cannot create file")
    }
    Handler handler(path, CREATE_ALWAYS);
}

void File::copy(const Path& path) const {
    if (path.isFile() && !modulePath.isFile()) {
        EXCEPT("Cannot copy file")
    } else if (!path.isFile() && modulePath.isFile()) {
        EXCEPT("Cannot copy file")
    }
    if(!CopyFileW(modulePath.absolute().c_str(),
                  path.absolute().c_str(), true)) {
        EXCEPT("Cannot copy file")
    }
}

void File::remove() {
    std::wstring path = modulePath.absolute();
    if(!SetFileAttributesW(path.c_str(),
                           GetFileAttributesW(path.c_str()) & ~FILE_ATTRIBUTE_READONLY) ) {
        EXCEPT("Cannot delete file");
    }

    if (!DeleteFileW(path.c_str())) {
        EXCEPT("Cannot delete file")
    }
    modulePath = Path();
}

File::File(const Path& path) : ModuleBase(path, ModuleType::File) {
    if (!path.isFile()) {
        EXCEPT("File does not exist")
    }
}

void File::writeLine(const std::wstring& line) {
    Handler handler(modulePath, OPEN_EXISTING);
    std::wstring data = line + L"\n";
    size_t count = lstrlenW(data.c_str()) * sizeof(wchar_t);
    if(!WriteFile(handler.get(), data.c_str(),
            count, nullptr, nullptr)) {
        EXCEPT("An error occurred on writing file")
    }
}

void File::writeLines(const std::vector<std::wstring>& lines) {
    for (const std::wstring& line : lines) {
        writeLine(line);
    }
}

std::vector<std::wstring> File::readAllLines() const {
    Handler handler(modulePath, OPEN_EXISTING);
    LARGE_INTEGER size;
    if (!GetFileSizeEx(handler.get(), &size)) {
        EXCEPT("An error occurred on reading file")
    }
    auto bufSize = static_cast<size_t>(size.QuadPart);
    void* buffer = new wchar_t[bufSize];
    DWORD readSize;
    if (!ReadFile(handler.get(), buffer, bufSize, &readSize, nullptr)) {
        EXCEPT("An error occurred on reading file")
    }
    std::wstring str = (const wchar_t*)buffer;
    str = str.substr(0, readSize / sizeof(wchar_t));
    std::vector<std::wstring> result;

    size_t start = 0;
    size_t end = str.find(L'\n');
    while (end != std::wstring::npos) {
        std::wstring t = str.substr(start, end - start);
        if (!t.empty()) {
            result.push_back(std::move(t));
        }
        start = end + 1;
        end = str.find(L'\n', start);
    }
    std::wstring t = str.substr(start, std::wstring::npos);
    if (!t.empty()) {
        result.push_back(std::move(t));
    }

    free(buffer);
    return result;
}
