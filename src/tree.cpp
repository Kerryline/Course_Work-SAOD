#include "tree.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <queue>
#include <cmath>
#include <climits>
#include <functional>

TreeNode* createTreeNode(const std::string& key, Record* record, int weight) {
    TreeNode* node = new TreeNode;
    node->key = key;
    node->records.push_back(record);
    node->weight = weight;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

std::string extractSurnameFromAuthor(const std::string& author) {
    std::string surname = author;
    size_t start = surname.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        size_t end = surname.find_last_not_of(" \t\n\r");
        surname = surname.substr(start, end - start + 1);
    }
    
    size_t space_pos = surname.find(' ');
    if (space_pos != std::string::npos) {
        surname = surname.substr(0, space_pos);
    }
    
    size_t underscore_pos = surname.find('_');
    if (underscore_pos != std::string::npos) {
        surname = surname.substr(0, underscore_pos);
    }
    
    return surname;
}

// -------------------------------------------------------------------
// АЛГОРИТМ A1: Построение дерева оптимального поиска
// -------------------------------------------------------------------
// Псевдокод из лекции:
// Вход: Упорядоченный список ключей K[1..n] с весами W[1..n]
// Выход: Дерево оптимального поиска
// 
// function BuildA1(K, W, start, end):
//   if start > end: return nullptr
//   // 1. Найти вершину с максимальным весом
//   maxIndex = start
//   for i = start+1 to end:
//     if W[i] > W[maxIndex]:
//       maxIndex = i
//   // 2. Создать корень с этой вершиной
//   root = CreateNode(K[maxIndex], W[maxIndex])
//   // 3. Рекурсивно построить левое поддерево
//   root.left = BuildA1(K, W, start, maxIndex-1)
//   // 4. Рекурсивно построить правое поддерево
//   root.right = BuildA1(K, W, maxIndex+1, end)
//   return root
// -------------------------------------------------------------------

// Шаг 1: Подсчет весов ключей
std::map<std::string, int> countKeyWeights(Queue& queue) {
    std::map<std::string, int> weights;
    QueueNode* current = queue.front;
    
    while (current != nullptr) {
        std::string key = convertToUTF8(current->data->author, 12);
        
        size_t end = key.find_last_not_of(' ');
        if (end != std::string::npos) key = key.substr(0, end + 1);
        
        key.erase(std::remove(key.begin(), key.end(), '\0'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\n'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
        
        weights[key]++;
        current = current->next;
    }
    
    return weights;
}

// Шаг 2: Создание упорядоченного списка ключей
std::vector<std::pair<std::string, int>> createSortedKeyList(const std::map<std::string, int>& weights) {
    std::vector<std::pair<std::string, int>> keyList;
    
    for (const auto& pair : weights) {
        std::string cleanKey = pair.first;
        cleanKey.erase(std::remove(cleanKey.begin(), cleanKey.end(), '\0'), cleanKey.end());
        cleanKey.erase(std::remove(cleanKey.begin(), cleanKey.end(), '\r'), cleanKey.end());
        cleanKey.erase(std::remove(cleanKey.begin(), cleanKey.end(), '\n'), cleanKey.end());
        keyList.push_back({cleanKey, pair.second});
    }
    
    std::sort(keyList.begin(), keyList.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
    
    return keyList;
}

// Функция поиска индекса с максимальным весом (Шаг 1 из псевдокода)
int findMaxWeightIndex(const std::vector<std::pair<std::string, int>>& keyList, int start, int end) {
    if (start > end) return -1;
    
    int maxIndex = start;
    int maxWeight = keyList[start].second;
    
    for (int i = start + 1; i <= end; i++) {
        if (keyList[i].second > maxWeight) {
            maxWeight = keyList[i].second;
            maxIndex = i;
        }
    }
    
    return maxIndex;
}

// РЕАЛИЗАЦИЯ АЛГОРИТМА A1 для построения ДОП
TreeNode* buildTreeA1Recursive(const std::vector<std::pair<std::string, int>>& keyList, 
                              std::map<std::string, std::vector<Record*>>& recordsMap,
                              int start, int end) {
    // Базовый случай
    if (start > end) return nullptr;
    
    // Шаг 1: Найти вершину с максимальным весом
    int maxIndex = findMaxWeightIndex(keyList, start, end);
    if (maxIndex == -1) return nullptr;
    
    // Шаг 2: Создать корень с этой вершиной
    TreeNode* root = createTreeNode(keyList[maxIndex].first, 
                                   recordsMap[keyList[maxIndex].first][0],
                                   keyList[maxIndex].second);
    
    root->records = recordsMap[keyList[maxIndex].first];
    
    // Шаг 3: Рекурсивно построить левое поддерево
    root->left = buildTreeA1Recursive(keyList, recordsMap, start, maxIndex - 1);
    
    // Шаг 4: Рекурсивно построить правое поддерево
    root->right = buildTreeA1Recursive(keyList, recordsMap, maxIndex + 1, end);
    
    return root;
}

// Основная функция построения ДОП по алгоритму A1
OptimalSearchTree* buildOptimalSearchTreeA1(Queue& queue) {
    if (isEmpty(queue)) {
        std::cout << "Очередь пуста, дерево не построено\n";
        return nullptr;
    }
    
    std::cout << "\nПостроение дерева оптимального поиска (алгоритм A1)...\n";
    
    // Подготовка данных: подсчет весов
    std::map<std::string, int> weights = countKeyWeights(queue);
    
    // Создание карты записей для быстрого доступа
    std::map<std::string, std::vector<Record*>> recordsMap;
    QueueNode* current = queue.front;
    while (current != nullptr) {
        std::string key = convertToUTF8(current->data->author, 12);
        size_t end = key.find_last_not_of(' ');
        if (end != std::string::npos) key = key.substr(0, end + 1);
        
        key.erase(std::remove(key.begin(), key.end(), '\0'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\n'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
        
        recordsMap[key].push_back(current->data);
        current = current->next;
    }
    
    // Создание упорядоченного списка ключей
    std::vector<std::pair<std::string, int>> keyList = createSortedKeyList(weights);
    
    // Построение дерева по алгоритму A1
    OptimalSearchTree* tree = new OptimalSearchTree;
    tree->root = buildTreeA1Recursive(keyList, recordsMap, 0, keyList.size() - 1);
    tree->totalKeys = keyList.size();
    tree->totalRecords = queue.size;
    tree->keyType = "автор (дерево оптимального поиска, алгоритм A1)";
    
    std::cout << "Дерево построено. Уникальных ключей: " << tree->totalKeys 
              << ", записей: " << tree->totalRecords << "\n";
    
    return tree;
}

// -------------------------------------------------------------------
// ПОИСК В ДЕРЕВЕ ОПТИМАЛЬНОГО ПОИСКА
// -------------------------------------------------------------------
// Поиск в ДОП отличается от обычного бинарного поиска:
// 1. Вес вершины увеличивается при каждом успешном поиске
// 2. Дерево строится с учетом частот поиска
// 3. Поиск может быть не бинарным, если ищем по частичному ключу
// -------------------------------------------------------------------

// Рекурсивный поиск по фамилии в дереве
void searchBySurnameInTree(TreeNode* root, const std::string& surname, 
                          std::vector<Record*>& results, int& comparisons) {
    if (root == nullptr) {
        comparisons++;
        return;
    }
    
    comparisons++;
    
    std::string nodeSurname = extractSurnameFromAuthor(root->key);
    
    // Если фамилии совпадают, добавляем все записи этого автора
    if (nodeSurname == surname) {
        for (auto& rec : root->records) {
            results.push_back(rec);
        }
    }
    
    // Рекурсивный поиск в обоих поддеревьях
    searchBySurnameInTree(root->left, surname, results, comparisons);
    searchBySurnameInTree(root->right, surname, results, comparisons);
}

// Поиск в дереве оптимального поиска по фамилии автора
std::vector<Record*> searchInOptimalTreeBySurname(OptimalSearchTree* tree, const std::string& surname) {
    if (tree == nullptr || tree->root == nullptr) {
        return std::vector<Record*>();
    }
    
    // Очистка и нормализация входной строки
    std::string searchSurname = surname;
    
    size_t start = searchSurname.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        size_t end = searchSurname.find_last_not_of(" \t\n\r");
        searchSurname = searchSurname.substr(start, end - start + 1);
    }
    
    std::replace(searchSurname.begin(), searchSurname.end(), '_', ' ');
    searchSurname.erase(std::remove(searchSurname.begin(), searchSurname.end(), '\0'), searchSurname.end());
    
    size_t space_pos = searchSurname.find(' ');
    if (space_pos != std::string::npos) {
        searchSurname = searchSurname.substr(0, space_pos);
    }
    
    // Поиск в дереве
    std::vector<Record*> results;
    int comparisons = 0;
    
    searchBySurnameInTree(tree->root, searchSurname, results, comparisons);
    
    // ОСОБЕННОСТЬ ДОП: увеличение веса найденных вершин
    std::function<void(TreeNode*, const std::string&)> increaseWeight = 
        [&increaseWeight](TreeNode* node, const std::string& surname) {
            if (node == nullptr) return;
            
            std::string nodeSurname = extractSurnameFromAuthor(node->key);
            if (nodeSurname == surname) {
                node->weight++;  // УВЕЛИЧЕНИЕ ВЕСА - важная особенность ДОП
            }
            
            increaseWeight(node->left, surname);
            increaseWeight(node->right, surname);
        };
    
    increaseWeight(tree->root, searchSurname);
    
    return results;
}

// -------------------------------------------------------------------
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// -------------------------------------------------------------------

void displayRecordsTable(const std::vector<Record*>& records, const std::string& title) {
    system("clear");
    std::cout << title << std::endl << std::endl;
    
    if (records.empty()) {
        std::cout << "Записей не найдено.\n";
        return;
    }
    
    std::cout << "┌─────────────┬──────────────────────────────────┬────────────────────┬──────────┬──────────┐" << std::endl;
    std::cout << "│ " << std::left << std::setw(12) << "Автор" << " │ "
              << std::setw(32) << "Заголовок" << " │ "
              << std::setw(17) << "Издательство" << " │ "
              << std::setw(7) << "Год" << " │ "
              << std::setw(7) << "Страниц" << " │" << std::endl;
    std::cout << "├─────────────┼──────────────────────────────────┼────────────────────┼──────────┼──────────┤" << std::endl;

    for (size_t i = 0; i < records.size(); ++i) {
        Record* rec = records[i];
        std::cout << "│ " << std::left << std::setw(12) << convertToUTF8(rec->author, 12) << " │ "
                  << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                  << std::setw(17) << convertToUTF8(rec->publisher, 16) << " │ "
                  << std::setw(7) << rec->year << " │ "
                  << std::setw(7) << rec->pages << " │" << std::endl;
    }

    std::cout << "└─────────────┴──────────────────────────────────┴────────────────────┴──────────┴──────────┘" << std::endl;
    std::cout << "Всего записей: " << records.size() << std::endl;
}

// Обходы дерева для проверки структуры
void inorderTraversal(TreeNode* root) {
    if (root == nullptr) return;
    
    inorderTraversal(root->left);
    std::cout << root->key << " (w=" << root->weight << ") ";
    inorderTraversal(root->right);
}

void preorderTraversal(TreeNode* root) {
    if (root == nullptr) return;
    
    std::cout << root->key << " (w=" << root->weight << ") ";
    preorderTraversal(root->left);
    preorderTraversal(root->right);
}

void postorderTraversal(TreeNode* root) {
    if (root == nullptr) return;
    
    postorderTraversal(root->left);
    postorderTraversal(root->right);
    std::cout << root->key << " (w=" << root->weight << ") ";
}

void displayTraversals(TreeNode* root) {
    if (root == nullptr) {
        std::cout << "Дерево пусто\n";
        return;
    }
    
    std::cout << "\n=== ОБХОДЫ ДЕРЕВА ОПТИМАЛЬНОГО ПОИСКА ===\n\n";
    
    std::cout << "Inorder (Л-К-П):\n";
    std::cout << "  ";
    inorderTraversal(root);
    std::cout << "\n\n";
    
    std::cout << "Preorder (К-Л-П):\n";
    std::cout << "  ";
    preorderTraversal(root);
    std::cout << "\n\n";
    
    std::cout << "Postorder (Л-П-К):\n";
    std::cout << "  ";
    postorderTraversal(root);
    std::cout << "\n\n";
}

void printTreeHorizontal(TreeNode* root, int depth, const std::string& indent, bool last) {
    if (root == nullptr) return;
    
    std::cout << indent;
    if (depth > 0) {
        std::cout << (last ? "└── " : "├── ");
    }
    
    std::cout << root->key << " [w:" << root->weight 
              << ", n:" << root->records.size() 
              << ", L:" << (depth + 1) << "]";
    
    std::cout << "\n";
    
    std::string newIndent = indent;
    if (depth > 0) {
        newIndent += (last ? "    " : "│   ");
    }
    
    if (root->left != nullptr) {
        bool isLast = (root->right == nullptr);
        printTreeHorizontal(root->left, depth + 1, newIndent, isLast);
    }
    
    if (root->right != nullptr) {
        printTreeHorizontal(root->right, depth + 1, newIndent, true);
    }
}

void printTreeHorizontal(TreeNode* root) {
    if (root == nullptr) {
        std::cout << "(пустое дерево)\n";
        return;
    }
    printTreeHorizontal(root, 0, "", true);
}

void printOptimalTree(const OptimalSearchTree* tree) {
    if (tree == nullptr || tree->root == nullptr) {
        std::cout << "Дерево пусто или не построено\n";
        return;
    }
    
    std::cout << "\n=== ДЕРЕВО ОПТИМАЛЬНОГО ПОИСКА (алгоритм A1) ===\n";
    std::cout << "Ключ: " << tree->keyType << "\n";
    std::cout << "Уникальных ключей: " << tree->totalKeys << "\n";
    std::cout << "Всего записей: " << tree->totalRecords << "\n\n";
    
    std::cout << "Структура дерева (w - вес вершины, n - записей, L - уровень):\n";
    printTreeHorizontal(tree->root);
}

void clearTreeNodes(TreeNode* root) {
    if (root == nullptr) return;
    
    clearTreeNodes(root->left);
    clearTreeNodes(root->right);
    delete root;
}

void clearOptimalTree(OptimalSearchTree* tree) {
    if (tree == nullptr) return;
    
    clearTreeNodes(tree->root);
    delete tree;
}

void displayOptimalTreeMenu(OptimalSearchTree* tree) {
    if (tree == nullptr) {
        std::cout << "Дерево не построено! Сначала выполните поиск и постройте дерево.\n";
        std::cout << "\nНажмите Enter...";
        std::cin.get();
        return;
    }
    
    int choice;
    do {
        system("clear");
        std::cout << "=== МЕНЮ ДЕРЕВА ОПТИМАЛЬНОГО ПОИСКА ===\n\n";
        std::cout << "Алгоритм построения: A1\n";
        std::cout << "Ключ: автор\n";
        std::cout << "Уникальных авторов: " << tree->totalKeys << "\n";
        std::cout << "Всего записей: " << tree->totalRecords << "\n\n";
        
        std::cout << "1. Вывести структуру дерева\n";
        std::cout << "2. Показать обходы дерева\n";
        std::cout << "3. Поиск по фамилии автора\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВаш выбор: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                system("clear");
                printOptimalTree(tree);
                std::cout << "\nНажмите Enter...";
                std::cin.get();
                break;
            }
            
            case 2: {
                system("clear");
                displayTraversals(tree->root);
                std::cout << "\nНажмите Enter...";
                std::cin.get();
                break;
            }
            
            case 3: {
                system("clear");
                std::string surname;
                std::cout << "ПОИСК ПО ФАМИЛИИ АВТОРА\n\n";
                std::cout << "Введите фамилию автора: ";
                std::getline(std::cin, surname);
                
                std::vector<Record*> foundRecords = searchInOptimalTreeBySurname(tree, surname);
                
                displayRecordsTable(foundRecords, "РЕЗУЛЬТАТЫ ПОИСКА ПО ФАМИЛИИ: " + surname);
                
                std::cout << "\nНажмите Enter...";
                std::cin.get();
                break;
            }
        }
        
    } while (choice != 0);
}