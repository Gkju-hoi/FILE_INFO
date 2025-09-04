#include "FileInfo.h"
#include <iostream>

int main(int argc, char* argv[]) {
    fs::path path1, path2;

    if (argc == 3) {
        path1 = argv[1];
        path2 = argv[2];
    }
    else {
        std::cout << "Использование: FolderCompare <папка1> <папка2>\n";
        std::cout << "Пожалуйста, укажите пути к папкам для сравнения:\n";

        std::cout << "Введите путь к первой папке: ";
        std::string input1;
        std::getline(std::cin, input1);
        path1 = input1;

        std::cout << "Введите путь ко второй папке: ";
        std::string input2;
        std::getline(std::cin, input2);
        path2 = input2;

        std::cout << "\n";
    }

    if (!fs::exists(path1) || !fs::is_directory(path1)) {
        std::cerr << "Ошибка: " << path1 << " не существует или не является папкой\n";
        return 1;
    }

    if (!fs::exists(path2) || !fs::is_directory(path2)) {
        std::cerr << "Ошибка: " << path2 << " не существует или не является папкой\n";
        return 1;
    }

    compareDirectories(path1, path2);

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}