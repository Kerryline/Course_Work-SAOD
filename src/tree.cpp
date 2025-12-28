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

TreeNode* buildTreeA1Recursive(const std::vector<std::pair<std::string, int>>& keyList, 
                              std::map<std::string, std::vector<Record*>>& recordsMap,
                              int start, int end) {
    if (start > end) return nullptr;
    
    int maxIndex = findMaxWeightIndex(keyList, start, end);
    if (maxIndex == -1) return nullptr;
    
    TreeNode* root = createTreeNode(keyList[maxIndex].first, 
                                   recordsMap[keyList[maxIndex].first][0],
                                   keyList[maxIndex].second);
    
    root->records = recordsMap[keyList[maxIndex].first];
    
    root->left = buildTreeA1Recursive(keyList, recordsMap, start, maxIndex - 1);
    
    root->right = buildTreeA1Recursive(keyList, recordsMap, maxIndex + 1, end);
    
    return root;
}

double calculateWeightedHeight(TreeNode* root, int depth, int& totalWeightedHeight, int& totalWeight) {
    if (root == nullptr) return 0.0;
    
    int level = depth + 1;
    
    totalWeightedHeight += root->weight * level;
    totalWeight += root->weight;
    
    calculateWeightedHeight(root->left, depth + 1, totalWeightedHeight, totalWeight);
    calculateWeightedHeight(root->right, depth + 1, totalWeightedHeight, totalWeight);
    
    if (depth == 0) {
        return totalWeight > 0 ? (double)totalWeightedHeight / totalWeight : 0.0;
    }
    return 0.0;
}

OptimalSearchTree* buildOptimalSearchTreeA1(Queue& queue) {
    if (isEmpty(queue)) {
        std::cout << "Очередь пуста, дерево не построено\n";
        return nullptr;
    }
    
    std::cout << "\n=== ПОСТРОЕНИЕ ДЕРЕВА ОПТИМАЛЬНОГО ПОИСКА (АЛГОРИТМ A1) ===\n";
    
    std::map<std::string, int> weights = countKeyWeights(queue);
    std::cout << "Найдено уникальных авторов: " << weights.size() << "\n";
    
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
    
    std::vector<std::pair<std::string, int>> keyList = createSortedKeyList(weights);
    
    std::cout << "\nВершины и их веса (упорядочено по ключу):\n";
    std::cout << "┌────────────────────┬────────┐\n";
    std::cout << "│ Автор              │ Вес    │\n";
    std::cout << "├────────────────────┼────────┤\n";
    int count = 0;
    for (const auto& pair : keyList) {
        if (count++ >= 15) {
            std::cout << "│ ...               │ ...    │\n";
            break;
        }
        std::cout << "│ " << std::left << std::setw(20) << pair.first << " │ "
                  << std::setw(6) << pair.second << " │\n";
    }
    std::cout << "└────────────────────┴────────┘\n";
    
    OptimalSearchTree* tree = new OptimalSearchTree;
    tree->root = buildTreeA1Recursive(keyList, recordsMap, 0, keyList.size() - 1);
    tree->totalKeys = keyList.size();
    tree->totalRecords = queue.size;
    tree->keyType = "автор (упорядочение по ключу)";
    
    int totalWeight = 0;
    for (const auto& pair : keyList) {
        totalWeight += pair.second;
    }
    
    int totalWeightedHeight = 0;
    int sumWeight = 0;
    double h_cps = calculateWeightedHeight(tree->root, 0, totalWeightedHeight, sumWeight);
    
    std::cout << "\n=== РЕЗУЛЬТАТЫ ПОСТРОЕНИЯ ===\n";
    std::cout << "Алгоритм: A1 (приближенный)\n";
    std::cout << "Принцип: корень - вершина с максимальным весом\n";
    std::cout << "Ключ: " << tree->keyType << "\n";
    std::cout << "Уникальных ключей: " << tree->totalKeys << "\n";
    std::cout << "Всего записей: " << tree->totalRecords << "\n";
    std::cout << "Суммарный вес W: " << totalWeight << "\n";
    std::cout << "Сумма Wi*hi: " << totalWeightedHeight << "\n";
    std::cout << "Средневзвешенная высота h_cps: " << std::fixed << std::setprecision(3) << h_cps << "\n";
    std::cout << "==========================================\n\n";
    
    return tree;
}

TreeNode* searchInOptimalTree(TreeNode* root, const std::string& key, int& comparisons) {
    if (root == nullptr) {
        comparisons++;
        return nullptr;
    }
    
    comparisons++;
    if (key == root->key) {
        root->weight++;
        return root;
    }
    
    comparisons++;
    if (key < root->key) {
        return searchInOptimalTree(root->left, key, comparisons);
    } else {
        return searchInOptimalTree(root->right, key, comparisons);
    }
}

TreeNode* searchInOptimalTree(OptimalSearchTree* tree, const std::string& author) {
    if (tree == nullptr) return nullptr;
    
    std::string searchKey = author;
    
    size_t start = searchKey.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        size_t end = searchKey.find_last_not_of(" \t\n\r");
        searchKey = searchKey.substr(start, end - start + 1);
    }
    
    std::replace(searchKey.begin(), searchKey.end(), '_', ' ');
    searchKey.erase(std::remove(searchKey.begin(), searchKey.end(), '\0'), searchKey.end());
    
    int comparisons = 0;
    TreeNode* result = searchInOptimalTree(tree->root, searchKey, comparisons);
    
    std::cout << "\n[ПОИСК В ДОП] Ключ: '" << searchKey << "'\n";
    std::cout << "  Выполнено сравнений: " << comparisons << "\n";
    
    if (result != nullptr) {
        TreeNode* current = tree->root;
        int level = 1;
        while (current != nullptr && current != result) {
            if (searchKey < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
            level++;
        }
        
        std::cout << "  Уровень найденной вершины: " << level 
                  << " (hi=" << level << ", Wi=" << result->weight << ")\n";
        return result;
    } else {
        std::cout << "  ✗ Не найдено\n";
        return nullptr;
    }
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
    
    std::cout << " (Wi*hi=" << root->weight * (depth + 1) << ")";
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

void printTreeTable(TreeNode* root, int depth) {
    if (root == nullptr) return;
    
    printTreeTable(root->left, depth + 1);
    
    int level = depth + 1;
    std::cout << "│ " << std::left << std::setw(20) << root->key << " │ "
              << std::setw(6) << root->weight << " │ "
              << std::setw(4) << level << " │ "
              << std::setw(8) << (root->weight * level) << " │ "
              << std::setw(7) << root->records.size() << " │ ";
    
    std::string years;
    for (size_t i = 0; i < root->records.size() && i < 2; i++) {
        if (i > 0) years += ", ";
        years += std::to_string(root->records[i]->year);
    }
    if (root->records.size() > 2) {
        years += ", ...";
    }
    
    std::cout << std::setw(20) << years << " │\n";
    
    printTreeTable(root->right, depth + 1);
}

void printOptimalTree(const OptimalSearchTree* tree) {
    if (tree == nullptr || tree->root == nullptr) {
        std::cout << "Дерево пусто или не построено\n";
        return;
    }
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│                         ДЕРЕВО ОПТИМАЛЬНОГО ПОИСКА (АЛГОРИТМ A1)                         │\n";
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Ключ поиска: " << std::setw(73) << tree->keyType << " │\n";
    std::cout << "│ Уникальных ключей: " << std::setw(66) << tree->totalKeys << " │\n";
    std::cout << "│ Всего записей: " << std::setw(71) << tree->totalRecords << " │\n";
    std::cout << "│ Алгоритм построения: " << std::setw(64) << "A1 (вершина с max весом)" << " │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘\n\n";
    
    int totalWeightedHeight = 0;
    int totalWeight = 0;
    double h_cps = calculateWeightedHeight(tree->root, 0, totalWeightedHeight, totalWeight);
    
    std::cout << "СТАТИСТИКА ДЕРЕВА:\n";
    std::cout << "  Сумма весов W = " << totalWeight << "\n";
    std::cout << "  Сумма Wi*hi = " << totalWeightedHeight << "\n";
    std::cout << "  Средневзвешенная высота h_cps = " << std::fixed << std::setprecision(3) << h_cps << "\n";
    std::cout << "  (h_cps = Σ(Wi*hi) / W)\n\n";
    
    std::cout << "ГОРИЗОНТАЛЬНОЕ ПРЕДСТАВЛЕНИЕ ДЕРЕВА:\n";
    std::cout << "(w - вес, n - количество записей, L - уровень, Wi*hi - вклад в суммарную высоту)\n\n";
    printTreeHorizontal(tree->root);
    std::cout << "\n";
    
    std::cout << "ОТСОРТИРОВАННЫЙ СПИСОК КЛЮЧЕЙ (inorder обход):\n";
    std::cout << "┌────────────────────┬────────┬──────┬──────────┬─────────┬────────────────────┐\n";
    std::cout << "│ Автор              │ Вес Wi │ Ур.  │ Wi*hi    │ Записей │ Годы изданий      │\n";
    std::cout << "├────────────────────┼────────┼──────┼──────────┼─────────┼────────────────────┤\n";
    printTreeTable(tree->root, 0);
    std::cout << "└────────────────────┴────────┴──────┴──────────┴─────────┴────────────────────┘\n";
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
        std::cout << "=== МЕНЮ ДЕРЕВА ОПТИМАЛЬНОГО ПОИСКА (АЛГОРИТМ A1) ===\n\n";
        
        int totalWeightedHeight = 0;
        int totalWeight = 0;
        double h_cps = calculateWeightedHeight(tree->root, 0, totalWeightedHeight, totalWeight);
        
        std::cout << "Информация о дереве:\n";
        std::cout << "  Ключ: " << tree->keyType << "\n";
        std::cout << "  Уникальных авторов: " << tree->totalKeys << "\n";
        std::cout << "  Всего записей: " << tree->totalRecords << "\n";
        std::cout << "  Сумма весов W: " << totalWeight << "\n";
        std::cout << "  Средневзвешенная высота h_cps: " << std::fixed << std::setprecision(3) << h_cps << "\n";
        std::cout << "  Алгоритм: A1 (корень - вершина с максимальным весом)\n\n";
        
        std::cout << "1. Вывести дерево (горизонтальный формат)\n";
        std::cout << "2. Вывести отсортированную таблицу ключей\n";
        std::cout << "3. Поиск автора в дереве (с увеличением веса)\n";
        std::cout << "4. Показать полную информацию о дереве\n";
        std::cout << "5. Вычислить средневзвешенную высоту\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВаш выбор: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                system("clear");
                std::cout << "ДЕРЕВО ОПТИМАЛЬНОГО ПОИСКА А1\n";
                std::cout << "==================================================\n\n";
                printTreeHorizontal(tree->root);
                std::cout << "\n\nЛегенда:\n";
                std::cout << "  w - вес вершины (частота поиска)\n";
                std::cout << "  n - количество записей с этим автором\n";
                std::cout << "  L - уровень вершины (корень = 1)\n";
                std::cout << "  Wi*hi - вклад вершины в суммарную высоту\n";
                std::cout << "  ├── - промежуточный узел\n";
                std::cout << "  └── - последний узел на уровне\n";
                std::cout << "  │   - продолжение ветви\n";
                std::cout << "\nНажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            
            case 2: {
                system("clear");
                std::cout << "ОТСОРТИРОВАННАЯ ТАБЛИЦА КЛЮЧЕЙ ДЕРЕВА (inorder)\n";
                std::cout << "================================================\n\n";
                
                std::cout << "┌────────────────────┬────────┬──────┬──────────┬─────────┬────────────────────┐\n";
                std::cout << "│ Автор              │ Вес Wi │ Ур.  │ Wi*hi    │ Записей │ Годы изданий      │\n";
                std::cout << "├────────────────────┼────────┼──────┼──────────┼─────────┼────────────────────┤\n";
                
                std::function<void(TreeNode*, int)> inorder = [&inorder](TreeNode* node, int depth) {
                    if (node == nullptr) return;
                    
                    inorder(node->left, depth + 1);
                    
                    int level = depth + 1;
                    std::cout << "│ " << std::left << std::setw(20) << node->key << " │ "
                              << std::setw(6) << node->weight << " │ "
                              << std::setw(4) << level << " │ "
                              << std::setw(8) << (node->weight * level) << " │ "
                              << std::setw(7) << node->records.size() << " │ ";
                    
                    std::string years;
                    for (size_t i = 0; i < node->records.size() && i < 2; i++) {
                        if (i > 0) years += ", ";
                        years += std::to_string(node->records[i]->year);
                    }
                    if (node->records.size() > 2) {
                        years += ", ...";
                    }
                    
                    std::cout << std::setw(20) << years << " │\n";
                    
                    inorder(node->right, depth + 1);
                };
                
                inorder(tree->root, 0);
                std::cout << "└────────────────────┴────────┴──────┴──────────┴─────────┴────────────────────┘\n";
                
                int totalWeight = 0;
                int totalWeightedHeight = 0;
                std::function<void(TreeNode*, int)> calc = [&](TreeNode* node, int depth) {
                    if (node == nullptr) return;
                    totalWeight += node->weight;
                    totalWeightedHeight += node->weight * (depth + 1);
                    calc(node->left, depth + 1);
                    calc(node->right, depth + 1);
                };
                calc(tree->root, 0);
                
                std::cout << "\nСтатистика:\n";
                std::cout << "  Всего уникальных авторов: " << tree->totalKeys << "\n";
                std::cout << "  Всего записей в дереве: " << tree->totalRecords << "\n";
                std::cout << "  Сумма весов ΣWi: " << totalWeight << "\n";
                std::cout << "  Сумма Wi*hi: " << totalWeightedHeight << "\n";
                std::cout << "  Средневзвешенная высота: " << std::fixed << std::setprecision(3) 
                          << (totalWeight > 0 ? (double)totalWeightedHeight / totalWeight : 0.0) << "\n";
                
                std::cout << "\nНажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            
            case 3: {
                system("clear");
                std::string author;
                std::cout << "ПОИСК АВТОРА В ДЕРЕВЕ ОПТИМАЛЬНОГО ПОИСКА\n\n";
                std::cout << "Введите имя автора (формат: 'Архипов В К' или 'Архипов_В_К'): ";
                std::getline(std::cin, author);
                
                TreeNode* found = searchInOptimalTree(tree, author);
                
                if (found != nullptr) {
                    std::cout << "\n✓ НАЙДЕНО в дереве оптимального поиска!\n\n";
                    std::cout << "Автор: " << found->key << "\n";
                    std::cout << "Вес Wi: " << found->weight << " (увеличился на 1)\n";
                    std::cout << "Количество книг: " << found->records.size() << "\n\n";
                    
                    std::cout << "Книги этого автора:\n";
                    std::cout << "┌─────┬──────────────────────────────────┬─────────┬──────────┐\n";
                    std::cout << "│ №   │ Заголовок                        │ Год     │ Страниц  │\n";
                    std::cout << "├─────┼──────────────────────────────────┼─────────┼──────────┤\n";
                    
                    for (size_t i = 0; i < found->records.size(); i++) {
                        Record* rec = found->records[i];
                        std::cout << "│ " << std::setw(3) << (i + 1) << " │ "
                                  << std::setw(32) << convertToUTF8(rec->title, 32) << " │ "
                                  << std::setw(7) << rec->year << " │ "
                                  << std::setw(8) << rec->pages << " │\n";
                    }
                    std::cout << "└─────┴──────────────────────────────────┴─────────┴──────────┘\n";
                } else {
                    std::cout << "\n✗ Автор '" << author << "' не найден в дереве\n";
                }
                
                std::cout << "\nНажмите Enter...";
                std::cin.get();
                break;
            }
            
            case 4: {
                system("clear");
                printOptimalTree(tree);
                std::cout << "\nНажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            
            case 5: {
                system("clear");
                std::cout << "ВЫЧИСЛЕНИЕ СРЕДНЕВЗВЕШЕННОЙ ВЫСОТЫ ДЕРЕВА\n\n";
                
                int totalWeightedHeight = 0;
                int totalWeight = 0;
                double h_cps = calculateWeightedHeight(tree->root, 0, totalWeightedHeight, totalWeight);
                
                std::cout << "Формула из лекции:\n";
                std::cout << "  h_cps = (W1*h1 + W2*h2 + ... + Wn*hn) / W\n";
                std::cout << "  где: Wi - вес i-й вершины\n";
                std::cout << "       hi - уровень i-й вершины (корень = 1)\n";
                std::cout << "       W = ΣWi - сумма всех весов\n\n";
                
                std::cout << "Результаты:\n";
                std::cout << "  Сумма весов W = " << totalWeight << "\n";
                std::cout << "  Сумма Wi*hi = " << totalWeightedHeight << "\n";
                std::cout << "  Средневзвешенная высота h_cps = " 
                          << std::fixed << std::setprecision(3) << h_cps << "\n\n";
                
                std::cout << "Пояснение:\n";
                std::cout << "  h_cps - это среднее время поиска по дереву\n";
                std::cout << "  с учетом частот обращения к его вершинам\n";
                
                std::cout << "\nНажмите Enter...";
                std::cin.get();
                break;
            }
        }
        
    } while (choice != 0);
    
    system("clear");
}