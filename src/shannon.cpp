#include "shannon.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

void shannonCoding(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Ошибка открытия файла " << filename << std::endl;
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (data.empty()) {
        std::cout << "Файл пуст.\n";
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }

    size_t total_bytes = data.size();

    std::map<unsigned char, int> frequency;
    for (unsigned char byte : data) {
        frequency[byte]++;
    }

    std::vector<std::pair<unsigned char, double>> symbols;
    double entropy = 0.0;
    for (const auto& pair : frequency) {
        double p = static_cast<double>(pair.second) / total_bytes;
        symbols.push_back({pair.first, p});
        if (p > 0) entropy -= p * std::log2(p);
    }

    std::sort(symbols.begin(), symbols.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::vector<double> Q(symbols.size() + 1, 0.0);
    for (size_t i = 1; i <= symbols.size(); ++i) {
        Q[i] = Q[i - 1] + symbols[i - 1].second;
    }

    double avg_length = 0.0;

    system("clear");
    std::cout << "КОДИРОВАНИЕ ШЕННОНА (файл " << filename << ", " << total_bytes << " байт)\n\n";
    std::cout << "Символ  Вероятность    L_i    Кодовое слово\n";
    std::cout << "------  -------------  ----   -------------\n";

    for (size_t i = 0; i < symbols.size(); ++i) {
        double p = symbols[i].second;
        int L = static_cast<int>(std::floor(-std::log2(p)) + 1);  // L_i = ⌊ -log₂ p_i ⌋ + 1 — по лекции
        avg_length += p * L;

        double q = Q[i];
        unsigned long long code_int = static_cast<unsigned long long>(std::floor(q * (1LL << L)));
        std::string code = "";
        for (int j = L - 1; j >= 0; --j) {
            code = ((code_int & (1LL << j)) ? "1" : "0") + code;
        }

        char sym_char = (symbols[i].first < 32 || symbols[i].first > 126) ? '.' : static_cast<char>(symbols[i].first);
        std::cout << "  " << sym_char << "      " << std::fixed << std::setprecision(6) << p
                  << "    " << L << "     " << code << "\n";
    }

    std::cout << "\nЭнтропия H = " << std::fixed << std::setprecision(4) << entropy << " бит/символ\n";
    std::cout << "Средняя длина L_ср = " << std::fixed << std::setprecision(3) << avg_length << " бит/символ\n";

    double compression_ratio = 8.0 / avg_length;
    std::cout << "Теоретический коэффициент сжатия = " << std::fixed << std::setprecision(3) << compression_ratio << "\n";

    std::cout << "\nНажмите Enter для возврата в меню...";
    std::cin.get();
}