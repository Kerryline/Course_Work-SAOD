#ifndef DISPLAY_H
#define DISPLAY_H

#include "database.h"
#include "queue.h"
#include "tree.h"
#include <vector>
#include <string>

void displayPage(const std::vector<Record*>& data, int page, int per_page, const std::string& title, bool show_special_options);
void displayInteractive(const std::vector<Record*>& data, const std::string& title, bool is_sorted_view);
void displayQueueWithTreeOption(const Queue& q, const std::string& title, OptimalSearchTree*& optimalTree);
void displayMainMenu(const std::vector<Record>& original, 
                     const std::vector<Record*>& sorted_indices,
                     Queue*& currentQueue,
                     OptimalSearchTree*& optimalTree);

#endif