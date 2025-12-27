#include "search.h"
#include <iostream>
#include <cstring>
#include <iomanip>

std::string getSurnamePrefix(const Record& rec) {
    std::string surname = extractSurname(rec);
    if (surname.length() >= 3) {
        return surname.substr(0, 3);
    }
    return surname;
}

unsigned char toUpperCP866(unsigned char c) {
    if (c >= 0xA0 && c <= 0xAF) return c - 0x20;
    if (c >= 0xE0 && c <= 0xEF) return c - 0x50;
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

bool compareLess(const std::string& a, const std::string& b) {
    for (size_t i = 0; i < 3; i++) {
        if (i >= a.length()) return true;
        if (i >= b.length()) return false;
        
        unsigned char ca = toUpperCP866((unsigned char)a[i]);
        unsigned char cb = toUpperCP866((unsigned char)b[i]);
        
        if (ca < cb) return true;
        if (ca > cb) return false;
    }
    return false;
}

bool exactMatch(const std::string& a, const std::string& b) {
    for (size_t i = 0; i < 3; i++) {
        if (i >= a.length() || i >= b.length()) return false;
        
        unsigned char ca = toUpperCP866((unsigned char)a[i]);
        unsigned char cb = toUpperCP866((unsigned char)b[i]);
        
        if (ca != cb) return false;
    }
    return true;
}

Queue binarySearchWithIndexing(const std::vector<Record*>& indices, const std::string& prefix) {
    Queue resultQueue;
    initQueue(resultQueue);
    
    if (indices.empty() || prefix.empty()) {
        return resultQueue;
    }
    
    std::string target = prefix;
    if (target.length() > 3) target = target.substr(0, 3);
    
    for (char& c : target) {
        c = toUpperCP866((unsigned char)c);
    }
    
    std::cout << "\n[ПОИСК] Ищем префикс: '" << target << "'\n";
    
    int L = 0;
    int R = indices.size() - 1;
    int comparisons = 0;
    
    while (L < R) {
        int m = (L + R) / 2;
        comparisons++;
        
        std::string current = getSurnamePrefix(*indices[m]);
        
        for (char& c : current) {
            c = toUpperCP866((unsigned char)c);
        }
        
        if (compareLess(current, target)) {
            L = m + 1;
        } else {
            R = m;
        }
    }
    
    std::cout << "[ПОИСК] Выполнено сравнений: " << comparisons << "\n";
    std::cout << "[ПОИСК] Граница найдена на позиции: " << L << "\n";
    
    if (L >= 0 && L < (int)indices.size()) {
        std::string found = getSurnamePrefix(*indices[L]);
        
        for (char& c : found) {
            c = toUpperCP866((unsigned char)c);
        }
        
        std::cout << "[ПОИСК] На границе: '" << found.substr(0, 3) << "'\n";
        
        if (exactMatch(found, target)) {
            std::cout << "[ПОИСК] Точное совпадение найдено!\n";
            
            int queue_size = 0;
            
            int start = L;
            while (start > 0) {
                std::string prev = getSurnamePrefix(*indices[start - 1]);
                for (char& c : prev) c = toUpperCP866((unsigned char)c);
                
                if (exactMatch(prev, target)) {
                    start--;
                } else {
                    break;
                }
            }
            
            for (int i = start; i < (int)indices.size(); i++) {
                std::string current = getSurnamePrefix(*indices[i]);
                for (char& c : current) c = toUpperCP866((unsigned char)c);
                
                if (exactMatch(current, target)) {
                    enqueue(resultQueue, indices[i]);
                    queue_size++;
                } else {
                    break;
                }
            }
            
            std::cout << "[ОЧЕРЕДЬ] Добавлено записей: " << queue_size << "\n";
            std::cout << "[ОЧЕРЕДЬ] Размер очереди: " << resultQueue.size << "\n";
            
        } else {
            std::cout << "[ПОИСК] Не найдено! '" << found.substr(0, 3) 
                     << "' != '" << target << "'\n";
        }
    }
    
    std::cout << "===============================\n\n";
    return resultQueue;
}