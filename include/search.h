#ifndef SEARCH_H
#define SEARCH_H

#include "database.h"
#include "queue.h"
#include <vector>
#include <string>

Queue binarySearchWithIndexing(const std::vector<Record*>& indices, const std::string& prefix);

#endif