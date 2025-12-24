#include "database.h"
#include "sort.h"
#include "display.h"
#include "search.h"  // ДОБАВИЛИ

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

    displayMenu(db, indices);

    return 0;
}