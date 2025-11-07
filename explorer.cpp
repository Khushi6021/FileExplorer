#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sys/stat.h>

namespace fs = std::filesystem;

void listDirectory(const fs::path& current) {
    std::cout << "\nCurrent directory: " << current << "\n";
    std::cout << "Contents:\n";
    for (const auto& entry : fs::directory_iterator(current)) {
        std::cout << (entry.is_directory() ? "[DIR]  " : "       ");
        std::cout << entry.path().filename().string() << "\n";
    }
}

void searchFile(const fs::path& current, const std::string& name) {
    for (const auto& entry : fs::recursive_directory_iterator(current)) {
        if (entry.path().filename().string().find(name) != std::string::npos) {
            std::cout << (entry.is_directory() ? "[DIR]  " : "       ");
            std::cout << entry.path().string() << "\n";
        }
    }
}

// 🔍 Helper to display file permissions
void showPermissions(const fs::path& filePath) {
    struct stat info;
    if (stat(filePath.c_str(), &info) != 0) {
        std::cout << "❌ Cannot access file: " << filePath << "\n";
        return;
    }

    std::cout << "Permissions for " << filePath.filename().string() << ": ";
    std::cout << ((info.st_mode & S_IRUSR) ? "r" : "-");
    std::cout << ((info.st_mode & S_IWUSR) ? "w" : "-");
    std::cout << ((info.st_mode & S_IXUSR) ? "x" : "-");
    std::cout << ((info.st_mode & S_IRGRP) ? "r" : "-");
    std::cout << ((info.st_mode & S_IWGRP) ? "w" : "-");
    std::cout << ((info.st_mode & S_IXGRP) ? "x" : "-");
    std::cout << ((info.st_mode & S_IROTH) ? "r" : "-");
    std::cout << ((info.st_mode & S_IWOTH) ? "w" : "-");
    std::cout << ((info.st_mode & S_IXOTH) ? "x" : "-");
    std::cout << "\n";
}

// 🧩 Change permissions (like chmod 755)
void changePermissions(const fs::path& filePath, int mode) {
    if (chmod(filePath.c_str(), mode) == 0)
        std::cout << "✅ Permissions updated successfully.\n";
    else
        std::cout << "❌ Failed to change permissions.\n";
}

int main() {
    fs::path current = fs::current_path();
    std::cout << "🗂️  Final File Explorer (with Permissions)\n";
    std::cout << "Commands:\n";
    std::cout << " cd <dir>\n create <file>\n mkdir <dir>\n copy <src> <dst>\n move <src> <dst>\n delete <name>\n search <name>\n perm <file>\n chmod <file> <mode>\n exit\n\n";

    while (true) {
        listDirectory(current);
        std::cout << "\n> ";
        std::string command;
        std::cin >> command;

        if (command == "exit") break;

        else if (command == "cd") {
            std::string dir;
            std::cin >> dir;
            if (dir == ".." && current.has_parent_path()) {
                current = current.parent_path();
            } else if (fs::is_directory(current / dir)) {
                current /= dir;
            } else {
                std::cout << "❌ Directory not found.\n";
            }
        }

        else if (command == "create") {
            std::string filename;
            std::cin >> filename;
            std::ofstream file(current / filename);
            if (file) std::cout << "✅ File created: " << filename << "\n";
            else std::cout << "❌ Failed to create file.\n";
        }

        else if (command == "mkdir") {
            std::string dirname;
            std::cin >> dirname;
            if (fs::create_directory(current / dirname))
                std::cout << "✅ Directory created: " << dirname << "\n";
            else
                std::cout << "❌ Could not create directory.\n";
        }

        else if (command == "copy") {
            std::string src, dst;
            std::cin >> src >> dst;
            try {
                fs::copy_file(current / src, current / dst, fs::copy_options::overwrite_existing);
                std::cout << "✅ File copied successfully.\n";
            } catch (...) {
                std::cout << "❌ Copy failed.\n";
            }
        }

        else if (command == "move") {
            std::string src, dst;
            std::cin >> src >> dst;
            try {
                fs::rename(current / src, current / dst);
                std::cout << "✅ File moved/renamed.\n";
            } catch (...) {
                std::cout << "❌ Move failed.\n";
            }
        }

        else if (command == "delete") {
            std::string target;
            std::cin >> target;
            try {
                fs::remove_all(current / target);
                std::cout << "✅ Deleted successfully.\n";
            } catch (...) {
                std::cout << "❌ Delete failed.\n";
            }
        }

        else if (command == "search") {
            std::string name;
            std::cin >> name;
            std::cout << "🔍 Searching for: " << name << "\n";
            searchFile(current, name);
        }

        else if (command == "perm") {
            std::string filename;
            std::cin >> filename;
            showPermissions(current / filename);
        }

        else if (command == "chmod") {
            std::string filename;
            int mode;
            std::cin >> filename >> std::oct >> mode;  // read as octal
            changePermissions(current / filename, mode);
        }

        else {
            std::cout << "❌ Unknown command.\n";
        }
    }

    std::cout << "Exiting File Explorer...\n";
    return 0;
}
