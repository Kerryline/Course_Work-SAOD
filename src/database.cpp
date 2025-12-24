#include "database.h"

std::vector<Record> loadDatabase(const std::string& filename) {
    std::vector<Record> db;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        return db;
    }
    Record rec;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(Record))) {
        db.push_back(rec);
    }
    return db;
}

std::string convertToUTF8(const char* src, size_t len) {
    iconv_t cd = iconv_open("UTF-8", "CP866");
    if (cd == (iconv_t)-1) return std::string(src, len);

    char* in = const_cast<char*>(src);
    size_t inbytes = len;
    std::string out_str(len * 2, '\0');
    char* out = &out_str[0];
    size_t outbytes = out_str.size();

    iconv(cd, &in, &inbytes, &out, &outbytes);
    iconv_close(cd);

    out_str.resize(out_str.size() - outbytes);
    size_t end = out_str.find_last_not_of(' ');
    if (end != std::string::npos) out_str.resize(end + 1);
    return out_str;
}

std::string extractSurname(const Record& rec) {
    std::string title_str(rec.title, 32);
    size_t end = title_str.find_last_not_of(' ');
    if (end != std::string::npos) title_str.resize(end + 1);

    size_t pos1 = title_str.find('_');
    size_t pos2 = std::string::npos;
    if (pos1 != std::string::npos) {
        pos2 = title_str.find('_', pos1 + 1);
    }

    if (pos2 == std::string::npos) {
        pos1 = title_str.find(' ');
        if (pos1 != std::string::npos) {
            pos2 = title_str.find(' ', pos1 + 1);
        }
    }

    if (pos2 == std::string::npos) return convertToUTF8(title_str.c_str(), title_str.size());

    std::string surname_raw = title_str.substr(pos2 + 1);
    size_t start = surname_raw.find_first_not_of(' ');
    if (start == std::string::npos) return "";
    size_t surname_end = surname_raw.find_last_not_of(' ');
    surname_raw = surname_raw.substr(start, surname_end - start + 1);

    return convertToUTF8(surname_raw.c_str(), surname_raw.size());
}
int customCompare(const std::string& a, const std::string& b) {
    // Пробуем использовать локаль для русских букв
    try {
        std::locale loc("ru_RU.UTF-8");
        const std::collate<char>& col = std::use_facet<std::collate<char>>(loc);
        return col.compare(a.data(), a.data() + a.size(), b.data(), b.data() + b.size());
    } catch (...) {
        // Если локаль недоступна, сравниваем побайтово (для CP866/UTF-8)
        // В CP866 русские буквы идут в порядке А-Я
        return a.compare(b);
    }
}