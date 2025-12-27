#include "database.h"
#include "sort.h"
#include "display.h"
#include "search.h"
#include "queue.h"
#include "tree.h"

int main() {
    std::vector<Record> db = loadDatabase("testBase1.dat");
    if (db.empty()) {
        std::cout << "Ошибка: не удалось загрузить базу данных 'testBase1.dat'!" << std::endl;
        return 1;
    }

    std::vector<Record*> indices;
    for (auto& rec : db) {
        indices.push_back(&rec);
    }

    quickSortHoare(indices, 0, indices.size() - 1);

    Queue* currentQueue = new Queue;
    initQueue(*currentQueue);
    
    OptimalSearchTree* optimalTree = nullptr;

    displayMainMenu(db, indices, currentQueue, optimalTree);

    clearQueue(*currentQueue);
    delete currentQueue;
    
    if (optimalTree != nullptr) {
        clearOptimalTree(optimalTree);
    }

    return 0;
}