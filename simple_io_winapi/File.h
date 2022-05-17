#pragma once
#include <vector>
#include <string>
#include "ModuleBase.h"
#include "Path.h"

class File : public ModuleBase {
public:
    explicit File(const Path& path);
    explicit File(const std::wstring& path) : File(Path(path)) { }

    void copy(const Path& path) const;
    void remove();
    void writeLine(const std::wstring& line);
    void writeLines(const std::vector<std::wstring>& lines);
    [[nodiscard]] std::vector<std::wstring> readAllLines() const;

    static bool Exists(const Path& path);
    static void Create(const Path& path);

};