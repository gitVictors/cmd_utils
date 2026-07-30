#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    // Проверяем, что передан путь к файлу
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file_path>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    
    // Открываем и читаем файл
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    // Парсим JSON
    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return 1;
    }

    // Проверяем наличие поля "results"
    if (!data.contains("results") || !data["results"].is_array()) {
        std::cerr << "Error: JSON does not contain 'results' array" << std::endl;
        return 1;
    }

    auto results = data["results"];
    long long totalZeroCount = 0;
    size_t count = 0;

    // Суммируем все zeroCount
    for (const auto& item : results) {
        if (item.contains("zeroCount") && item["zeroCount"].is_number_integer()) {
            totalZeroCount += item["zeroCount"].get<int>();
            count++;
        }
    }

    if (count == 0) {
        std::cerr << "Error: No valid entries found in 'results'" << std::endl;
        return 1;
    }

    // Вычисляем и выводим среднее значение
    double average = static_cast<double>(totalZeroCount) / count;
    std::cout << "Average zeroCount: " << average << std::endl;
    std::cout << "Total entries: " << count << std::endl;

    return 0;
}