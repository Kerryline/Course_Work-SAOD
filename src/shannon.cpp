#include "shannon.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>
#include <cstring>

#define MAX_SYMBOLS 256
#define MAX_CODE_LEN 256

typedef struct {
    unsigned char symbol;
    int freq;
    char code[MAX_CODE_LEN];
    int code_len;
} SymbolInfo;

std::string cp866ToChar(unsigned char c) {
    if (c >= 32 && c <= 126) {
        return std::string(1, static_cast<char>(c));
    }
    
    switch(c) {
        case 128: return "А";
        case 129: return "Б";
        case 130: return "В";
        case 131: return "Г";
        case 132: return "Д";
        case 133: return "Е";
        case 134: return "Ж";
        case 135: return "З";
        case 136: return "И";
        case 137: return "Й";
        case 138: return "К";
        case 139: return "Л";
        case 140: return "М";
        case 141: return "Н";
        case 142: return "О";
        case 143: return "П";
        case 144: return "Р";
        case 145: return "С";
        case 146: return "Т";
        case 147: return "У";
        case 148: return "Ф";
        case 149: return "Х";
        case 150: return "Ц";
        case 151: return "Ч";
        case 152: return "Ш";
        case 153: return "Щ";
        case 154: return "Ъ";
        case 155: return "Ы";
        case 156: return "Ь";
        case 157: return "Э";
        case 158: return "Ю";
        case 159: return "Я";
        case 160: return "а";
        case 161: return "б";
        case 162: return "в";
        case 163: return "г";
        case 164: return "д";
        case 165: return "е";
        case 166: return "ж";
        case 167: return "з";
        case 168: return "и";
        case 169: return "й";
        case 170: return "к";
        case 171: return "л";
        case 172: return "м";
        case 173: return "н";
        case 174: return "о";
        case 175: return "п";
        case 224: return "р";
        case 225: return "с";
        case 226: return "т";
        case 227: return "у";
        case 228: return "ф";
        case 229: return "х";
        case 230: return "ц";
        case 231: return "ч";
        case 232: return "ш";
        case 233: return "щ";
        case 234: return "ъ";
        case 235: return "ы";
        case 236: return "ь";
        case 237: return "э";
        case 238: return "ю";
        case 239: return "я";
        case 240: return "Ё";
        case 241: return "ё";
        default: return "";
    }
}

void shannonCoding(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Ошибка открытия файла " << filename << std::endl;
        std::cout << "Нажмите Enter...";
        std::cin.get();
        return;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    char* buffer = new char[file_size];
    file.read(buffer, file_size);
    file.close();

    if (file_size == 0) {
        std::cout << "Файл пуст.\n";
        std::cout << "Нажмите Enter...";
        std::cin.get();
        delete[] buffer;
        return;
    }

    int freq[MAX_SYMBOLS] = {0};
    for (size_t i = 0; i < file_size; ++i) {
        unsigned char c = static_cast<unsigned char>(buffer[i]);
        freq[c]++;
    }

    SymbolInfo symbols[MAX_SYMBOLS];
    int symbol_count = 0;
    
    for (int i = 0; i < MAX_SYMBOLS; ++i) {
        if (freq[i] > 0) {
            symbols[symbol_count].symbol = static_cast<unsigned char>(i);
            symbols[symbol_count].freq = freq[i];
            symbols[symbol_count].code_len = 0;
            symbols[symbol_count].code[0] = '\0';
            symbol_count++;
        }
    }

    std::sort(symbols, symbols + symbol_count, [](const SymbolInfo& a, const SymbolInfo& b) {
        return a.freq > b.freq;
    });

    double* P = new double[symbol_count];
    double* Q = new double[symbol_count + 1];
    
    Q[0] = 0.0;
    for (int i = 0; i < symbol_count; ++i) {
        P[i] = static_cast<double>(symbols[i].freq) / file_size;
        Q[i + 1] = Q[i] + P[i];
    }

    double avg_length = 0.0;
    double entropy = 0.0;
    
    for (int i = 0; i < symbol_count; ++i) {
        double p = P[i];
        
        int L = static_cast<int>(std::ceil(-std::log2(p)));
        symbols[i].code_len = L;
        
        double q = Q[i];
        std::string code = "";
        
        for (int j = 0; j < L; ++j) {
            q *= 2;
            if (q >= 1.0) {
                code += '1';
                q -= 1.0;
            } else {
                code += '0';
            }
        }
        
        strncpy(symbols[i].code, code.c_str(), MAX_CODE_LEN - 1);
        symbols[i].code[MAX_CODE_LEN - 1] = '\0';
        
        avg_length += p * L;
        if (p > 0.0) {
            entropy -= p * std::log2(p);
        }
    }

    system("clear");

    std::cout << "╔═════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                     СТАТИЧЕСКОЕ КОДИРОВАНИЕ ШЕННОНА                     ║\n";
    std::cout << "╠═════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Символ (код)  Частота   Вероятность    Кодовое слово         Длина      ║\n";
    std::cout << "╠═════════════════════════════════════════════════════════════════════════╣\n";

    for (int i = 0; i < symbol_count; ++i) {
        unsigned char c = symbols[i].symbol;
        std::string display = cp866ToChar(c);
        
        if (!display.empty()) {
            std::cout << "║  '" << display << "' (" << std::setw(3) << (int)c << ") "
                      << std::setw(8) << symbols[i].freq << " "
                      << std::setw(14) << std::fixed << std::setprecision(6) << P[i] << " "
                      << std::setw(20) << symbols[i].code << " "
                      << std::setw(6) << symbols[i].code_len << "    ║\n";
        } else {
            std::cout << "║      (" << std::setw(3) << (int)c << ") "
                      << std::setw(8) << symbols[i].freq << " "
                      << std::setw(14) << std::fixed << std::setprecision(6) << P[i] << " "
                      << std::setw(20) << symbols[i].code << " "
                      << std::setw(6) << symbols[i].code_len << "    ║\n";
        }
    }

    double prob_sum = 0.0;
    for (int i = 0; i < symbol_count; ++i) {
        prob_sum += P[i];
    }

    std::cout << "╠═════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Сумма вероятностей: " << std::setw(49) << std::fixed << std::setprecision(6) << prob_sum << " ║\n";
    std::cout << "║ Средняя длина кодового слова: " << std::setw(36) << std::fixed << std::setprecision(6) << avg_length << " бит ║\n";
    std::cout << "║ Энтропия исходного файла: " << std::setw(39) << std::fixed << std::setprecision(6) << entropy << " бит ║\n";
    std::cout << "║ Разница (длина - энтропия): " << std::setw(37) << std::fixed << std::setprecision(6) << (avg_length - entropy) << " бит ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════════════════════╝\n";

    std::cout << "\nНажмите Enter для возврата в меню...";
    std::cin.get();

    delete[] buffer;
    delete[] P;
    delete[] Q;
}