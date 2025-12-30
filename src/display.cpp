#include "display.h"
#include "search.h"
#include <random>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>
#include "shannon.h"

void displayPage(const std::vector<Record*>& data, int page, int per_page, const std::string& title) {
    system("clear");

    int start = page * per_page;
    int end = std::min(start + per_page, static_cast<int>(data.size()));

    std::cout << title << std::endl << std::endl;

    std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
    std::cout << "│ Автор       │ Заголовок                        │ Издательство       │ Год      │ Страниц  │" << std::endl;
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
    std::cout << "│ Автор       │ Заголовок                        │ Издательство       │ Год      │ Страниц  │" << std::endl;
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

void displayQueue(const Queue& q, const std::string& title) {
    system("clear");
    std::cout << title << std::endl << std::endl;
    
    std::cout << "Размер очереди: " << q.size << "\n\n";
    
    if (q.size == 0) {
        std::cout << "Очередь пуста.\n";
        std::cout << "\nНажмите Enter для возврата...";
        std::cin.get();
        return;
    }

    std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
    std::cout << "│ Автор       │ Заголовок                        │ Издательство       │ Год      │ Страниц  │" << std::endl;
    std::cout << "├─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;

    QueueNode* current = q.front;
    int counter = 0;
    
    while (current != nullptr) {
        Record* rec = current->data;
        std::cout << "│ " << std::left << std::setw(12) << convertToUTF8(rec->author, 12) << " │ "
                  << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                  << std::setw(17) << convertToUTF8(rec->publisher, 16) << " │ "
                  << std::setw(7) << rec->year << " │ "
                  << std::setw(7) << rec->pages << " │" << std::endl;
        current = current->next;
        counter++;
        
        if (counter % 20 == 0 && current != nullptr) {
            std::cout << "└─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;
            std::cout << "\nПоказано " << counter << " из " << q.size << " записей. ";
            std::cout << "Нажмите Enter для продолжения...";
            std::cin.get();
            
            system("clear");
            std::cout << title << std::endl << std::endl;
            std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
            std::cout << "│ Автор       │ Заголовок                        │ Издательство       │ Год      │ Страниц  │" << std::endl;
            std::cout << "├─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;
        }
    }

    std::cout << "└─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;
    
    std::cout << "\nВсего найдено записей: " << q.size << "\n";
    std::cout << "Показано записей: " << counter << "\n";
    
    std::cout << "\nНажмите Enter для возврата в меню...";
    std::cin.get();
}

void displayMainMenu(const std::vector<Record>& original, 
                     const std::vector<Record*>& sorted_indices,
                     Queue*& currentQueue,
                     OptimalSearchTree*& optimalTree) {
    int choice;
    do {
        system("clear");
        std::cout << "Главное меню\n"
                  << "1. Исходная БД\n"
                  << "2. Отсортированная БД\n"
                  << "3. Случайная запись\n"
                  << "4. Запись по номеру\n"
                  << "5. Вся отсортированная БД\n"
                  << "6. Двоичный поиск по фамилии (первые 3 буквы)\n"
                  << "7. Построить дерево из результатов поиска\n"
                  << "8. Работа с деревом оптимального поиска\n"
                  << "9. Кодирование Шеннона\n"
                  << "0. Выход\n"
                  << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            std::vector<Record*> orig_indices;
            for (const auto& rec : original) orig_indices.push_back(const_cast<Record*>(&rec));
            displayInteractive(orig_indices, "Исходная база данных");
        } 
        else if (choice == 2) {
            displayInteractive(sorted_indices, "Отсортированная база данных");
        } 
        else if (choice == 5) {
            displayAllOnePage(sorted_indices, "Вся отсортированная база данных одной страницей");
        } 
        else if (choice == 3) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, original.size() - 1);
            int idx = dis(gen);
            std::vector<Record*> single = {const_cast<Record*>(&original[idx])};
            system("clear");
            displayPage(single, 0, 1, "Случайная запись");
            std::cout << "\nНажмите Enter...";
            std::cin.get();
        } 
        else if (choice == 4) {
            int num;
            system("clear");
            std::cout << "Введите номер записи (0 — " << original.size() - 1 << "): ";
            std::cin >> num;
            std::cin.ignore();
            if (num >= 0 && num < original.size()) {
                std::vector<Record*> single = {const_cast<Record*>(&original[num])};
                displayPage(single, 0, 1, "Запись №" + std::to_string(num));
            } else {
                std::cout << "Неверный номер!" << std::endl;
            }
            std::cout << "\nНажмите Enter...";
            std::cin.get();
        } 
        else if (choice == 6) {
            system("clear");
            std::string prefix;
            std::cout << "Введите первые 3 буквы фамилии (на русском): ";
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
                displayQueue(*currentQueue, "Результаты двоичного поиска (очередь)");
                
                if (optimalTree != nullptr) {
                    clearOptimalTree(optimalTree);
                    optimalTree = nullptr;
                }
            }
        } 
        else if (choice == 7) {
            if (currentQueue->size == 0) {
                std::cout << "Очередь пуста! Сначала выполните поиск (пункт 6).\n";
                std::cout << "\nНажмите Enter...";
                std::cin.get();
            } else {
                if (optimalTree != nullptr) {
                    clearOptimalTree(optimalTree);
                }
                
                optimalTree = buildOptimalSearchTreeA1(*currentQueue);
                
                std::cout << "\nДерево оптимального поиска построено!\n";
                std::cout << "Ключ дерева: " << optimalTree->keyType << "\n";
                std::cout << "\nНажмите Enter...";
                std::cin.get();
            }
        } 
        else if (choice == 8) {
            if (optimalTree == nullptr) {
                std::cout << "Дерево не построено! Сначала постройте дерево (пункт 7).\n";
                std::cout << "\nНажмите Enter...";
                std::cin.get();
            } else {
                displayOptimalTreeMenu(optimalTree);
            }
        }

        else if (choice == 9) {
            shannonCoding("testBase1.dat");
        }
    } while (choice != 0);

    system("clear");
}