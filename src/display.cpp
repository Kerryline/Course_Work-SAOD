#include "display.h"
#include "search.h"
#include <random>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>

void displayPage(const std::vector<Record*>& data, int page, int per_page, const std::string& title) {
    system("clear");

    int start = page * per_page;
    int end = std::min(start + per_page, static_cast<int>(data.size()));

    std::cout << title << std::endl << std::endl;

    std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
    std::cout << "│ " << std::left << std::setw(12) << "Автор" << " │ "
              << std::setw(32) << "Заголовок" << " │ "
              << std::setw(17) << "Издательство" << " │ "
              << std::setw(7) << "Год" << " │ "
              << std::setw(7) << "Страниц" << " │" << std::endl;
    std::cout << "├─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;

    for (int idx = start; idx < end; ++idx) {
        Record* rec = data[idx];
        std::cout << "│ " << std::left << std::setw(12) << convertToUTF8(rec->author, 12) << " │ "
                  << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                  << std::setw(17) << convertToUTF8(rec->publisher, 16) << " │ "
                  << std::setw(7) << rec->year << " │ "
                  << std::setw(7) << rec->pages << " │" << std::endl;
    }

    std::cout << "└─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;

    int total_pages = (data.size() + per_page - 1) / per_page;
    std::cout << "Страница " << (page + 1) << " из " << total_pages 
              << " (всего записей: " << data.size() << ")" << std::endl << std::endl;

    std::cout << "   Назад(b)   Вперед(Enter)   Меню(m)   " << std::endl << std::endl;
}

void displayInteractive(const std::vector<Record*>& data, const std::string& title) {
    if (data.empty()) {
        std::cout << "База данных пуста." << std::endl;
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }

    const int per_page = 20;
    const int total_pages = (data.size() + per_page - 1) / per_page;
    int current_page = 0;

    while (true) {
        displayPage(data, current_page, per_page, title);

        std::string input;
        std::getline(std::cin, input);

        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "m") {
            system("clear");
            break;
        } else if (input == "b") {
            if (current_page > 0) --current_page;
        } else if (input == "n" || input.empty()) {
            if (current_page < total_pages - 1) ++current_page;
        }
    }
}

void displayAllOnePage(const std::vector<Record*>& data, const std::string& title) {
    system("clear");
    std::cout << title << std::endl << std::endl;

    std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
    std::cout << "│ " << std::left << std::setw(12) << "Автор" << " │ "
              << std::setw(32) << "Заголовок" << " │ "
              << std::setw(17) << "Издательство" << " │ "
              << std::setw(7) << "Год" << " │ "
              << std::setw(7) << "Страниц" << " │" << std::endl;
    std::cout << "├─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;

    for (size_t i = 0; i < data.size(); ++i) {
        Record* rec = data[i];
        std::cout << "│ " << std::left << std::setw(12) << convertToUTF8(rec->author, 12) << " │ "
                  << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                  << std::setw(17) << convertToUTF8(rec->publisher, 16) << " │ "
                  << std::setw(7) << rec->year << " │ "
                  << std::setw(7) << rec->pages << " │" << std::endl;
    }

    std::cout << "└─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;
    std::cout << "Всего записей: " << data.size() << std::endl;
    std::cout << "\nНажмите Enter для возврата в меню...";
    std::cin.get();
}

void displayMenu(const std::vector<Record>& original, const std::vector<Record*>& sorted_indices) {
    int choice;
    do {
        system("clear");
        std::cout << "=== Главное меню ===\n"
                  << "1. Исходная БД\n"
                  << "2. Отсортированная БД\n"
                  << "3. Случайная запись\n"
                  << "4. Запись по номеру\n"
                  << "5. Вся отсортированная БД\n"
                  << "6. Двоичный поиск и очередь\n"
                  << "0. Выход\n"
                  << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            std::vector<Record*> orig_indices;
            for (const auto& rec : original) orig_indices.push_back(const_cast<Record*>(&rec));
            displayInteractive(orig_indices, "ИСХОДНАЯ БАЗА ДАННЫХ");
        } else if (choice == 2) {
            displayInteractive(sorted_indices, "ОТСОРТИРОВАННАЯ БАЗА ДАННЫХ");
        } else if (choice == 5) {
            displayAllOnePage(sorted_indices, "ВСЯ ОТСОРТИРОВАННАЯ БАЗА ДАННЫХ ОДНОЙ СТРАНИЦЕЙ");
        } else if (choice == 3) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, original.size() - 1);
            int idx = dis(gen);
            std::vector<Record*> single = {const_cast<Record*>(&original[idx])};
            system("clear");
            displayPage(single, 0, 1, "СЛУЧАЙНАЯ ЗАПИСЬ");
            std::cout << "\nНажмите Enter...";
            std::cin.get();
        } else if (choice == 4) {
            int num;
            system("clear");
            std::cout << "Введите номер записи (0 — " << original.size() - 1 << "): ";
            std::cin >> num;
            std::cin.ignore();
            if (num >= 0 && num < original.size()) {
                std::vector<Record*> single = {const_cast<Record*>(&original[num])};
                displayPage(single, 0, 1, "ЗАПИСЬ №" + std::to_string(num));
            } else {
                std::cout << "Неверный номер!" << std::endl;
            }
            std::cout << "\nНажмите Enter...";
            std::cin.get();
        } else if (choice == 6) {  // ДВОИЧНЫЙ ПОИСК И ОЧЕРЕДЬ
            searchAndDisplayQueue(sorted_indices);
        }
    } while (choice != 0);

    system("clear");
}