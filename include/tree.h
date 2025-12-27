#ifndef TREE_H
#define TREE_H

#include "database.h"
#include "queue.h"
#include <vector>
#include <string>
#include <map>

struct TreeNode {
    std::string key;
    std::vector<Record*> records;
    int weight;
    TreeNode* left;
    TreeNode* right;
};

struct OptimalSearchTree {
    TreeNode* root;
    int totalKeys;
    int totalRecords;
    std::string keyType;
};

OptimalSearchTree* buildOptimalSearchTreeA1(Queue& queue);
void printOptimalTree(const OptimalSearchTree* tree);
void printTreeStructure(TreeNode* root, int depth = 0);
TreeNode* searchInOptimalTree(OptimalSearchTree* tree, const std::string& key);
void displayOptimalTreeMenu(OptimalSearchTree* tree);
void clearOptimalTree(OptimalSearchTree* tree);

#endif