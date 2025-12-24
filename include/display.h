#ifndef DISPLAY_H
#define DISPLAY_H

#include "database.h"

void displayPage(const std::vector<Record*>& data, int page, int per_page, const std::string& title);
void displayInteractive(const std::vector<Record*>& data, const std::string& title);
void displayAllOnePage(const std::vector<Record*>& data, const std::string& title);
void displayMenu(const std::vector<Record>& original, const std::vector<Record*>& sorted_indices);

#endif