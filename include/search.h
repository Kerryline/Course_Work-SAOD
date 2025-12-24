#ifndef SEARCH_H
#define SEARCH_H

#include "database.h"
#include <vector>
#include <string>

// Вспомогательная функция для получения первых 3 букв
std::string getFirstThreeLetters(const std::string& surname);

// Двоичный поиск с формированием очереди
std::vector<Record*> binarySearchQueue(const std::vector<Record*>& indices, const std::string& key);

// Функция для поиска и отображения очереди
void searchAndDisplayQueue(const std::vector<Record*>& sorted_indices);

#endif