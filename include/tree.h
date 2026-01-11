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
std::vector<Record*> searchInTreeByPages(OptimalSearchTree* tree, int pages);
void displayTreeSearchResults(const std::vector<Record*>& results, int search_pages);
void displayTreeTraversals(OptimalSearchTree* tree);
void clearOptimalTree(OptimalSearchTree* tree);
void inorderTraversalWithRecords(TreeNode* root, std::vector<Record*>& result);

#endif