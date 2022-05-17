#include "Path.h"
#include <algorithm>
#include <stdexcept>
#include <windows.h>

std::vector<std::wstring> split(const std::wstring& str,
                               const std::vector<char>& delimiters) {
    std::wstring copy = str;
    if (delimiters.size() > 1) {
        for (size_t i = 1; i < delimiters.size(); i++) {
            std::replace(copy.begin(), copy.end(), delimiters[i], delimiters[0]);
        }
    }

    std::vector<std::wstring> res;
    size_t start = 0;
    size_t end = copy.find(delimiters[0]);
    while (end != std::wstring::npos) {
        std::wstring t = copy.substr(start, end - start);
        if (!t.empty()) {
            res.push_back(std::move(t));
        }
        start = end + 1;
        end = copy.find(delimiters[0], start);
    }
    std::wstring t = copy.substr(start, std::wstring::npos);
    if (!t.empty()) {
        res.push_back(std::move(t));
    }
    return res;
}

Path::Tokens Path::tokenize(const std::wstring& value) {
    Tokens result;
    std::wstring startPath = value.substr(0, value.find(Path::PATH_SEP));
    std::vector<std::wstring> spl = split(startPath, {Path::DIRECTORY_SEP, Path::ALT_DIRECTORY_SEP});
    size_t colon = spl.front().find(':');
    if (colon == std::wstring::npos) {
        size_t point = spl.front().find('.');
        if (point == std::wstring::npos) {
            result.pathValues.push_back(spl.front());
        } else {
            result.fileName = spl.front().substr(0, point);
            result.fileExtension = spl.front().substr(point + 1, std::wstring::npos);
        }
    } else {
        result.volume = spl.front().substr(0, colon);
    }

    if (spl.size() == 1) {
        return result;
    }

    if (spl.size() > 2) {
        for (size_t i = 1; i + 1 < spl.size(); i++)  {
            result.pathValues.push_back(spl[i]);
        }
    }

    size_t point = spl.back().find('.');
    if (point == std::wstring::npos) {
        result.pathValues.push_back(spl.back());
    } else {
        result.fileName = spl.back().substr(0, point);
        result.fileExtension = spl.back().substr(point + 1, std::wstring::npos);
    }
    return result;
}

Path::Path(const std::wstring& path) {
    tokens = tokenize(path);

    if (tokens.volume.empty()) {
        tokens = tokenize(absolute());
    }

    if (!tokens.isValid()) {
        EXCEPT("Path does not exist")
    }
}

Path::Path() {
    tokens = tokenize(absolute());
}


std::wstring Path::absolute() const {
    wchar_t path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, path);
    std::wstring res = tokens.volume + Path::VOLUME_SEP + Path::DIRECTORY_SEP;
    if (tokens.volume.empty()) {
        res = std::wstring(path) + Path::DIRECTORY_SEP;
    }
    for (const std::wstring& f : tokens.pathValues) {
        res += f;
        res.push_back(Path::DIRECTORY_SEP);
    }
    if (!tokens.fileName.empty()) {
        res += tokens.fileName;
        res.push_back('.');
        res += tokens.fileExtension;
    }

    return res;
}

std::wstring Path::getDirectoryName() const {
    if (tokens.pathValues.empty()) {
        EXCEPT("Directory does not exist")
    }
    return tokens.pathValues.back();
}

std::wstring Path::getExtension() const {
    if (tokens.fileExtension.empty()) {
        EXCEPT("File does not exist")
    }
    return tokens.fileExtension;
}

std::wstring Path::getFileName() const {
    if (tokens.fileName.empty()) {
        EXCEPT("File does not exist")
    }
    return tokens.fileName;
}

std::wstring Path::getParent() const {
    std::vector<std::wstring> res {tokens.volume};
    for (const std::wstring& str : tokens.pathValues) {
        res.push_back(str);
    }
    if (tokens.fileName.empty()) {
        res.pop_back();
    }
    if (res.empty()) {
        EXCEPT("Parent directory does not exist")
    }

    std::wstring str = res.front() + Path::VOLUME_SEP + Path::DIRECTORY_SEP;
    for (size_t i = 1; i < res.size(); i++) {
        str += res[i];
        str.push_back(Path::DIRECTORY_SEP);
    }
    return str;
}

std::vector<wchar_t> Path::GetInvalidPathChars() {
    std::vector<wchar_t> res = {'.', ':', '*', '?', '\\', '/', '\"', '<', '>', '|', '\0'};
    for (size_t i = 1; i < 32; i++) {
        res.push_back(static_cast<wchar_t>(i));
    }

    return res;
}

Path Path::join(const std::wstring& other) const {
    try {
        std::wstring s = absolute() + Path::DIRECTORY_SEP + other;
        return Path(s);
    } catch (FileSystemException& e) {
        EXCEPT("Cannot join paths")
    }
}

bool Path::isFile() const {
    return !tokens.fileName.empty() && !tokens.fileExtension.empty();
}

bool Path::Tokens::empty() const {
    return volume.empty() && pathValues.empty() &&
           fileName.empty() && fileExtension.empty();
}

bool Path::Tokens::isValid() {
    if (empty()) {
        return false;
    }

    if (fileName.empty() && !fileExtension.empty()) {
        return false;
    }

    if (!fileName.empty() && fileExtension.empty()) {
        return false;
    }

    std::vector forbidden = Path::GetInvalidPathChars();
    for (wchar_t fChar : forbidden) {
        if (volume.find(fChar) != std::wstring::npos) {
            return false;
        }
        if (fileName.find(fChar) != std::wstring::npos) {
            return false;
        }
        if (fileExtension.find(fChar) != std::wstring::npos) {
            return false;
        }
        for (const std::wstring& s : pathValues) {
            if (s.find(fChar) != std::wstring::npos) {
                return false;
            }
        }
    }

    return true;
}
