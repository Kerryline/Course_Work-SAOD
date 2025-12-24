#include "search.h"
#include "display.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <cctype>

// Функция для получения первых 3 букв фамилии
std::string getFirstThreeLetters(const std::string& surname) {
    if (surname.empty()) return "   ";
    
    if (surname.length() >= 3) {
        return surname.substr(0, 3);
    } else {
        std::string result = surname;
        while (result.length() < 3) {
            result += ' ';
        }
        return result;
    }
}

// Вторая версия двоичного поиска из лекции (одно сравнение на итерацию)
// Находит самый левый элемент с заданным ключом
std::vector<Record*> binarySearchQueue(const std::vector<Record*>& indices, const std::string& key) {
    std::vector<Record*> queue;
    
    if (indices.empty() || key.empty()) return queue;
    
    // Подготавливаем ключ (ровно 3 символа)
    std::string search_key = key;
    if (search_key.length() > 3) {
        search_key = search_key.substr(0, 3);
    }
    while (search_key.length() < 3) {
        search_key += ' ';
    }
    
    // Алгоритм из лекции (вторая версия)
    int L = 0;
    int R = indices.size();
    
    while (L < R) {
        int m = (L + R) / 2;
        std::string surname = extractSurname(*indices[m]);
        std::string first_three = getFirstThreeLetters(surname);
        
        if (customCompare(first_three, search_key) < 0) {
            L = m + 1;
        } else {
            R = m;
        }
    }
    
    // Теперь L = R - индекс первого элемента >= key
    if (R < indices.size()) {
        std::string surname = extractSurname(*indices[R]);
        std::string first_three = getFirstThreeLetters(surname);
        
        // Проверяем, что найденный элемент действительно имеет нужный ключ
        if (customCompare(first_three, search_key) == 0) {
            // Собираем ВСЕ записи с таким же ключом в очередь
            for (int i = R; i < indices.size(); i++) {
                std::string cur_surname = extractSurname(*indices[i]);
                std::string cur_first_three = getFirstThreeLetters(cur_surname);
                
                if (customCompare(cur_first_three, search_key) == 0) {
                    queue.push_back(indices[i]);
                } else {
                    break; // Ключ изменился - больше нет нужных записей
                }
            }
        }
    }
    
    return queue;
}

// Функция для поиска и отображения очереди
void searchAndDisplayQueue(const std::vector<Record*>& sorted_indices) {
    system("clear");
    
    if (sorted_indices.empty()) {
        std::cout << "База данных пуста." << std::endl;
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }
    
    std::cout << "=== ДВОИЧНЫЙ ПОИСК И ОЧЕРЕДЬ ===" << std::endl;
    std::cout << "Поиск по первым трем буквам фамилии" << std::endl;
    std::cout << "Алгоритм: вторая версия из лекции (одно сравнение на итерацию)" << std::endl;
    std::cout << "=================================" << std::endl;
    
    std::cout << "\nВведите первые три буквы фамилии: ";
    std::string search_key;
    std::getline(std::cin, search_key);
    
    // Подготавливаем ключ
    if (search_key.length() > 3) {
        search_key = search_key.substr(0, 3);
    }
    
    // Выполняем поиск
    std::vector<Record*> queue = binarySearchQueue(sorted_indices, search_key);
    
    // Отображаем результаты
    system("clear");
    std::cout << "=== РЕЗУЛЬТАТЫ ПОИСКА ===" << std::endl;
    std::cout << "Ключ поиска: '" << search_key << "'" << std::endl;
    std::cout << "Найдено записей: " << queue.size() << std::endl;
    std::cout << "=========================" << std::endl;
    
    if (queue.empty()) {
        std::cout << "\nЗаписей с фамилией, начинающейся на '" << search_key << "' не найдено." << std::endl;
    } else {
        std::cout << "\nОчередь сформирована. Размер: " << queue.size() << " элементов" << std::endl;
        
        // Выводим очередь
        std::cout << "\n┌─────┬─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
        std::cout << "│ " << std::left << std::setw(3) << "№" << " │ "
                  << std::setw(11) << "Автор" << " │ "
                  << std::setw(32) << "Заголовок" << " │ "
                  << std::setw(18) << "Издательство" << " │ "
                  << std::setw(8) << "Год" << " │ "
                  << std::setw(8) << "Страниц" << " │" << std::endl;
        std::cout << "├─────┼─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;
        
        for (size_t i = 0; i < queue.size(); ++i) {
            Record* rec = queue[i];
            
            std::cout << "│ " << std::right << std::setw(3) << (i+1) << " │ "
                      << std::left << std::setw(11) << convertToUTF8(rec->author, 11) << " │ "
                      << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                      << std::setw(18) << convertToUTF8(rec->publisher, 16) << " │ "
                      << std::setw(8) << rec->year << " │ "
                      << std::setw(8) << rec->pages << " │" << std::endl;
        }
        
        std::cout << "└─────┴─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;
        
        // Информация о сортировке очереди
        bool is_sorted = true;
        for (size_t i = 1; i < queue.size(); i++) {
            std::string s1 = extractSurname(*queue[i-1]);
            std::string s2 = extractSurname(*queue[i]);
            if (customCompare(s1, s2) > 0) {
                is_sorted = false;
                break;
            }
        }
        std::cout << "\nОчередь отсортирована по фамилии: " << (is_sorted ? "ДА" : "НЕТ") << std::endl;
    }
    
    std::cout << "\nНажмите Enter для возврата в меню...";
    std::cin.get();
}