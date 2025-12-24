#include "sort.h"

int partition(std::vector<Record*>& indices, int left, int right) {
    int mid = left + (right - left) / 2;
    Record* pivot_rec = indices[mid];
    std::string pivot_surname = extractSurname(*pivot_rec);

    int i = left - 1;
    int j = right + 1;

    while (true) {
        do {
            i++;
        } while (customCompare(extractSurname(*indices[i]), pivot_surname) < 0);

        do {
            j--;
        } while (customCompare(extractSurname(*indices[j]), pivot_surname) > 0);

        if (i >= j) {
            return j;
        }

        std::swap(indices[i], indices[j]);
    }
}

void quickSortHoare(std::vector<Record*>& indices, int left, int right) {
    while (left < right) {
        int split_pos = partition(indices, left, right);

        if (split_pos - left < right - split_pos) {
            quickSortHoare(indices, left, split_pos);
            left = split_pos + 1;
        } else {
            quickSortHoare(indices, split_pos + 1, right);
            right = split_pos;
        }
    }
}