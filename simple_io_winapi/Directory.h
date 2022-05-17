#pragma once
#include <vector>
#include <string>
#include "ModuleBase.h"
#include "Path.h"

class Directory : public ModuleBase {
public:
    explicit Directory(const Path& path);
    explicit Directory(const std::wstring& path) : Directory(Path(path)) { }

    [[nodiscard]] std::vector<Path> getFiles(bool recursive = false) const;
    [[nodiscard]] std::vector<Path> getDirectories(bool recursive = false) const;

    void removeEmpty();
    static void SetCurrent(const Path& path);
    static Path GetCurrent();
    static bool Exists(const Path& path);
    static void Create(const Path& path);

private:
    static void getObjects(std::vector<Path>& objects, const Path& path,
                    unsigned type = 0, bool recursive = false);
};