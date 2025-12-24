#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iconv.h>
#include <cstring>
#include <locale>

struct Record {
    char author[12];
    char title[32];
    char publisher[16];
    short year;
    short pages;
};

std::vector<Record> loadDatabase(const std::string& filename);
std::string convertToUTF8(const char* src, size_t len);
std::string extractSurname(const Record& rec);
int customCompare(const std::string& a, const std::string& b);

#endif