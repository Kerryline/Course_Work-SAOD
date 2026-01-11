#include "display.h"
#include "search.h"
#include <random>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>
#include "shannon.h"

void displayPage(const std::vector<Record*>& data, int page, int per_page, const std::string& title, bool show_special_options) {
    system("clear");

    int start = page * per_page;
    int end = std::min(start + per_page, static_cast<int>(data.size()));

    std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::setw(75) << std::left << title << "║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Автор         Заглавие                     Издательство     Год   Стр   ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";

    for (int idx = start; idx < end; ++idx) {
        Record* rec = data[idx];
        std::string author = convertToUTF8(rec->author, 12);
        std::string title_str = convertToUTF8(rec->title, 32);
        std::string publisher = convertToUTF8(rec->publisher, 16);
        
        std::cout << "║ " << std::left << std::setw(12) << author << " "
                  << std::setw(32) << title_str << " "
                  << std::setw(16) << publisher << " "
                  << std::setw(4) << rec->year << " "
                  << std::setw(4) << rec->pages << " ║\n";
    }

    for (int i = end - start; i < per_page; i++) {
        std::cout << "║                                                                             ║\n";
    }

    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    
    int total_pages = (data.size() + per_page - 1) / per_page;
    
    if (show_special_options) {
        std::cout << "║ Страница " << std::setw(2) << (page + 1) << "/" << std::setw(2) << total_pages 
                  << " | N-след | P-пред | B-назад | R-случайная | I-по номеру | A-вся БД ║\n";
    } else {
        std::cout << "║ Страница " << std::setw(2) << (page + 1) << "/" << std::setw(2) << total_pages 
                  << " | N - след. | P - пред. | B - назад | Всего: " << std::setw(4) << data.size() << " ║\n";
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "Выбор: ";
}

void displayInteractive(const std::vector<Record*>& data, const std::string& title, bool is_sorted_view) {
    if (data.empty()) {
        std::cout << "База данных пуста.\n";
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }

    const int per_page = 20;
    const int total_pages = (data.size() + per_page - 1) / per_page;
    int current_page = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    while (true) {
        displayPage(data, current_page, per_page, title, is_sorted_view);

        std::string input;
        std::getline(std::cin, input);

        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "b") {
            system("clear");
            break;
        } else if (input == "n" || input.empty()) {
            if (current_page < total_pages - 1) ++current_page;
        } else if (input == "p") {
            if (current_page > 0) --current_page;
        } else if (is_sorted_view && input == "r") {
            std::uniform_int_distribution<> dis(0, data.size() - 1);
            int idx = dis(gen);
            std::vector<Record*> single = {data[idx]};
            
            system("clear");
            displayPage(single, 0, 1, "Случайная запись: " + convertToUTF8(data[idx]->title, 32), false);
            std::cout << "\nНажмите Enter...";
            std::cin.get();
        } else if (is_sorted_view && input == "i") {
            int num;
            system("clear");
            std::cout << "Введите номер записи (0 — " << data.size() - 1 << "): ";
            std::cin >> num;
            std::cin.ignore();
            if (num >= 0 && num < static_cast<int>(data.size())) {
                std::vector<Record*> single = {data[num]};
                system("clear");
                displayPage(single, 0, 1, "Запись №" + std::to_string(num) + ": " + convertToUTF8(data[num]->title, 32), false);
                std::cout << "\nНажмите Enter...";
                std::cin.get();
            } else {
                std::cout << "Неверный номер!\n";
                std::cout << "Нажмите Enter...";
                std::cin.get();
            }
        } else if (is_sorted_view && input == "a") {
            system("clear");
            std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                 ВСЯ ОТСОРТИРОВАННАЯ БАЗА ДАННЫХ                          ║\n";
            std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Автор         Заглавие                     Издательство     Год   Стр   ║\n";
            std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
            
            for (size_t i = 0; i < data.size(); ++i) {
                Record* rec = data[i];
                std::string author = convertToUTF8(rec->author, 12);
                std::string title_str = convertToUTF8(rec->title, 32);
                std::string publisher = convertToUTF8(rec->publisher, 16);
                
                std::cout << "║ " << std::left << std::setw(12) << author << " "
                          << std::setw(32) << title_str << " "
                          << std::setw(16) << publisher << " "
                          << std::setw(4) << rec->year << " "
                          << std::setw(4) << rec->pages << " ║\n";
            }
            
            std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Всего записей: " << std::setw(60) << std::left << data.size() << "║\n";
            std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
            std::cout << "\nНажмите Enter для возврата...";
            std::cin.get();
        }
    }
}

void displayQueueWithTreeOption(const Queue& q, const std::string& title, OptimalSearchTree*& optimalTree) {
    if (q.size == 0) {
        system("clear");
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ " << std::setw(75) << std::left << title << "║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║                                                                            ║\n";
        std::cout << "║                          Очередь пуста.                                  ║\n";
        std::cout << "║                                                                            ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "\nНажмите Enter...";
        std::cin.get();
        return;
    }

    const int per_page = 20;
    int total_pages = (q.size + per_page - 1) / per_page;
    int current_page = 0;
    
    while (true) {
        system("clear");
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ " << std::setw(75) << std::left << title << "║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Автор         Заглавие                     Издательство     Год   Стр   ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        
        QueueNode* current = q.front;
        int counter = 0;
        int skipped = current_page * per_page;
        
        while (current != nullptr && skipped > 0) {
            current = current->next;
            skipped--;
        }
        
        while (current != nullptr && counter < per_page) {
            Record* rec = current->data;
            std::string author = convertToUTF8(rec->author, 12);
            std::string title_str = convertToUTF8(rec->title, 32);
            std::string publisher = convertToUTF8(rec->publisher, 16);
            
            std::cout << "║ " << std::left << std::setw(12) << author << " "
                      << std::setw(32) << title_str << " "
                      << std::setw(16) << publisher << " "
                      << std::setw(4) << rec->year << " "
                      << std::setw(4) << rec->pages << " ║\n";
            
            current = current->next;
            counter++;
        }
        
        for (int i = counter; i < per_page; i++) {
            std::cout << "║                                                                             ║\n";
        }
        
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        
        std::cout << "║ Страница " << std::setw(2) << (current_page + 1) << "/" << std::setw(2) << total_pages 
                  << " | N - след. | P - пред. | B - назад | T - обход дерева A1 ║\n";
        
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "Выбор: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        if (input == "t") {
            if (optimalTree != nullptr) {
                clearOptimalTree(optimalTree);
                optimalTree = nullptr;
            }
            
            optimalTree = buildOptimalSearchTreeA1(const_cast<Queue&>(q));
            
            if (optimalTree != nullptr) {
                displayTreeTraversals(optimalTree);
            }
            return;
        } 
        else if (input == "b") {
            return;
        } 
        else if (input == "n") {
            if (current_page < total_pages - 1) current_page++;
        } 
        else if (input == "p") {
            if (current_page > 0) current_page--;
        }
    }
}

void displayMainMenu(const std::vector<Record>& original, 
                     const std::vector<Record*>& sorted_indices,
                     Queue*& currentQueue,
                     OptimalSearchTree*& optimalTree) {
    int choice;
    do {
        system("clear");
        std::cout << "╔══════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                               ГЛАВНОЕ МЕНЮ                              ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ 1. Исходная БД                                                         ║\n";
        std::cout << "║ 2. Отсортированная БД                                                  ║\n";
        std::cout << "║ 3. Двоичный поиск по фамилии (первые 3 буквы)                          ║\n";
        std::cout << "║ 4. Кодирование Шеннона                                                 ║\n";
        std::cout << "║ 0. Выход                                                               ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            std::vector<Record*> orig_indices;
            for (const auto& rec : original) orig_indices.push_back(const_cast<Record*>(&rec));
            displayInteractive(orig_indices, "Исходная база данных", false);
        } 
        else if (choice == 2) {
            displayInteractive(sorted_indices, "Отсортированная база данных", true);
        } 
        else if (choice == 3) {
            system("clear");
            std::string prefix;
            std::cout << "Введите первые 3 буквы фамилии: ";
            std::getline(std::cin, prefix);
            
            clearQueue(*currentQueue);
            
            Queue tempQueue = binarySearchWithIndexing(sorted_indices, prefix);
            
            QueueNode* current = tempQueue.front;
            while (current != nullptr) {
                enqueue(*currentQueue, current->data);
                current = current->next;
            }
            
            if (currentQueue->size == 0) {
                std::cout << "\nЗаписей с префиксом '" << prefix << "' не найдено.\n";
                std::cout << "\nНажмите Enter...";
                std::cin.get();
            } else {
                displayQueueWithTreeOption(*currentQueue, 
                                          "РЕЗУЛЬТАТЫ ПОИСКА ПО КЛЮЧУ (очередь)", 
                                          optimalTree);
            }
        } 
        else if (choice == 4) {
            shannonCoding("testBase1.dat");
        }
    } while (choice != 0);

    system("clear");
}