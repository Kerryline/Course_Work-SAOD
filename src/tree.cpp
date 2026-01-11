#include "tree.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <queue>
#include <cmath>
#include <climits>
#include <functional>
#include <sstream>

TreeNode* createTreeNode(const std::string& key, Record* record, int weight) {
    TreeNode* node = new TreeNode;
    node->key = key;
    node->records.push_back(record);
    node->weight = weight;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

std::string getKeyFromRecord(Record* record) {
    return std::to_string(record->pages);
}

std::map<std::string, int> countKeyWeights(Queue& queue) {
    std::map<std::string, int> weights;
    QueueNode* current = queue.front;
    
    while (current != nullptr) {
        std::string key = getKeyFromRecord(current->data);
        weights[key]++;
        current = current->next;
    }
    
    return weights;
}

std::vector<std::pair<std::string, int>> createSortedKeyList(const std::map<std::string, int>& weights) {
    std::vector<std::pair<std::string, int>> keyList;
    
    for (const auto& pair : weights) {
        keyList.push_back({pair.first, pair.second});
    }
    
    std::sort(keyList.begin(), keyList.end(),
        [](const auto& a, const auto& b) {
            return std::stoi(a.first) < std::stoi(b.first);
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

OptimalSearchTree* buildOptimalSearchTreeA1(Queue& queue) {
    if (isEmpty(queue)) {
        return nullptr;
    }
    
    std::map<std::string, int> weights = countKeyWeights(queue);
    std::map<std::string, std::vector<Record*>> recordsMap;
    
    QueueNode* current = queue.front;
    while (current != nullptr) {
        std::string key = getKeyFromRecord(current->data);
        recordsMap[key].push_back(current->data);
        current = current->next;
    }
    
    std::vector<std::pair<std::string, int>> keyList = createSortedKeyList(weights);
    
    OptimalSearchTree* tree = new OptimalSearchTree;
    tree->root = buildTreeA1Recursive(keyList, recordsMap, 0, keyList.size() - 1);
    tree->totalKeys = keyList.size();
    tree->totalRecords = queue.size;
    tree->keyType = "количество страниц (дерево оптимального поиска, алгоритм A1)";
    
    return tree;
}

std::vector<Record*> searchInTreeByPages(OptimalSearchTree* tree, int pages) {
    std::vector<Record*> results;
    
    if (tree == nullptr || tree->root == nullptr) {
        return results;
    }
    
    std::string search_key = std::to_string(pages);
    std::function<void(TreeNode*)> searchRecursive = [&](TreeNode* node) {
        if (node == nullptr) return;
        
        if (node->key == search_key) {
            results.insert(results.end(), node->records.begin(), node->records.end());
        }
        
        int node_pages = std::stoi(node->key);
        if (pages < node_pages) {
            searchRecursive(node->left);
        } else if (pages > node_pages) {
            searchRecursive(node->right);
        } else {
            searchRecursive(node->left);
            searchRecursive(node->right);
        }
    };
    
    searchRecursive(tree->root);
    return results;
}

void displayTreeSearchResults(const std::vector<Record*>& results, int search_pages) {
    system("clear");
    
    if (results.empty()) {
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    ПОИСК В ДЕРЕВЕ (страниц: " << std::setw(4) << search_pages << ")                   ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║                                                                            ║\n";
        std::cout << "║                    Записей с " << std::setw(4) << search_pages << " страницами не найдено.              ║\n";
        std::cout << "║                                                                            ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "\nНажмите Enter...";
        std::cin.get();
        return;
    }
    
    system("clear");
    
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    ПОИСК В ДЕРЕВЕ (страниц: " << std::setw(4) << search_pages << ")                   ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Автор         Заглавие                     Издательство     Год   Стр   ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    
    for (int i = 0; i < results.size(); i++) {
        Record* rec = results[i];
        
        std::string author = convertToUTF8(rec->author, 12);
        std::string title = convertToUTF8(rec->title, 32);
        std::string publisher = convertToUTF8(rec->publisher, 16);
        
        size_t author_end = author.find_last_not_of(' ');
        if (author_end != std::string::npos) author = author.substr(0, author_end + 1);
        
        size_t title_end = title.find_last_not_of(' ');
        if (title_end != std::string::npos) title = title.substr(0, title_end + 1);
        
        size_t publisher_end = publisher.find_last_not_of(' ');
        if (publisher_end != std::string::npos) publisher = publisher.substr(0, publisher_end + 1);
        
        std::cout << "║ " << std::left << std::setw(12) << author << " "
                  << std::setw(32) << title << " "
                  << std::setw(16) << publisher << " "
                  << std::setw(4) << rec->year << " "
                  << std::setw(4) << rec->pages << " ║\n";
    }
    
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Найдено записей: " << std::setw(58) << std::left << results.size() << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\nНажмите Enter для возврата...";
    std::cin.get();
}

void inorderTraversalWithRecords(TreeNode* root, std::vector<Record*>& result) {
    if (root == nullptr) return;
    
    inorderTraversalWithRecords(root->left, result);
    
    for (Record* rec : root->records) {
        result.push_back(rec);
    }
    
    inorderTraversalWithRecords(root->right, result);
}

void displayTreeTraversals(OptimalSearchTree* tree) {
    if (tree == nullptr || tree->root == nullptr) {
        std::cout << "Дерево не построено\n";
        return;
    }
    
    std::vector<Record*> inorder_records;
    inorderTraversalWithRecords(tree->root, inorder_records);
    
    const int per_page = 20;
    int total_pages = (inorder_records.size() + per_page - 1) / per_page;
    int current_page = 0;
    
    do {
        system("clear");
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                     ОБХОД ДЕРЕВА A1 (Inorder Л-К-П)                      ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Автор         Заглавие                     Издательство     Год   Стр   ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        
        int start = current_page * per_page;
        int end = std::min(start + per_page, (int)inorder_records.size());
        
        for (int i = start; i < end; i++) {
            Record* rec = inorder_records[i];
            
            std::string author = convertToUTF8(rec->author, 12);
            std::string title = convertToUTF8(rec->title, 32);
            std::string publisher = convertToUTF8(rec->publisher, 16);
            
            size_t author_end = author.find_last_not_of(' ');
            if (author_end != std::string::npos) author = author.substr(0, author_end + 1);
            
            size_t title_end = title.find_last_not_of(' ');
            if (title_end != std::string::npos) title = title.substr(0, title_end + 1);
            
            size_t publisher_end = publisher.find_last_not_of(' ');
            if (publisher_end != std::string::npos) publisher = publisher.substr(0, publisher_end + 1);
            
            std::cout << "║ " << std::left << std::setw(12) << author << " "
                      << std::setw(32) << title << " "
                      << std::setw(16) << publisher << " "
                      << std::setw(4) << rec->year << " "
                      << std::setw(4) << rec->pages << " ║\n";
        }
        
        for (int i = end - start; i < per_page; i++) {
            std::cout << "║                                                                             ║\n";
        }
        
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Страница " << std::setw(2) << (current_page + 1) << "/" << std::setw(2) << total_pages 
                  << " | N - след. | P - пред. | B - назад | T - поиск в дереве | Всего: " 
                  << std::setw(4) << inorder_records.size() << " ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "Выбор: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        if (input == "t") {
            int search_pages;
            std::cout << "\nВведите количество страниц для поиска: ";
            std::cin >> search_pages;
            std::cin.ignore();
            
            std::vector<Record*> results = searchInTreeByPages(tree, search_pages);
            displayTreeSearchResults(results, search_pages);
            
            current_page = 0;
            continue;
        }
        else if (input == "b") {
            break;
        } 
        else if (input == "n") {
            if (current_page < total_pages - 1) current_page++;
        } 
        else if (input == "p") {
            if (current_page > 0) current_page--;
        }
        
    } while (true);
}

void printTreeHorizontal(TreeNode* root, int depth, const std::string& indent, bool last) {
    if (root == nullptr) return;
    
    std::cout << indent;
    if (depth > 0) {
        std::cout << (last ? "└── " : "├── ");
    }
    
    std::cout << root->key << " стр. [w:" << root->weight 
              << ", n:" << root->records.size() 
              << "]";
    
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

void printOptimalTree(const OptimalSearchTree* tree) {
    if (tree == nullptr || tree->root == nullptr) {
        std::cout << "Дерево пусто или не построено\n";
        return;
    }
    
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                     ДЕРЕВО ОПТИМАЛЬНОГО ПОИСКА A1                         ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Ключ: " << std::setw(73) << std::left << tree->keyType << "║\n";
    std::cout << "║ Уникальных ключей: " << std::setw(55) << std::left << tree->totalKeys << "║\n";
    std::cout << "║ Всего записей: " << std::setw(59) << std::left << tree->totalRecords << "║\n";
    std::cout << "║ w - вес вершины, n - количество записей                                 ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║                               СТРУКТУРА ДЕРЕВА                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝\n\n";
    
    printTreeHorizontal(tree->root, 0, "", true);
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