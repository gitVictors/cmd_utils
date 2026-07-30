#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <openssl/evp.h>  // Вместо sha.h

// Функция для преобразования hex строки в байты
std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    std::string cleanHex;
    
    for (char c : hex) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            cleanHex += c;
        }
    }
    
    if (cleanHex.length() % 2 != 0) {
        throw std::runtime_error("Invalid hex string length (must be even)");
    }
    
    if (cleanHex.length() > 160) {
        throw std::runtime_error("Hex string is too long (max 160 characters for 80 bytes)");
    }
    
    bytes.reserve(cleanHex.length() / 2);
    
    for (size_t i = 0; i < cleanHex.length(); i += 2) {
        std::string byteStr = cleanHex.substr(i, 2);
        char* endptr;
        unsigned long byte = strtoul(byteStr.c_str(), &endptr, 16);
        if (*endptr != '\0') {
            throw std::runtime_error("Invalid hex character in string");
        }
        bytes.push_back(static_cast<unsigned char>(byte));
    }
    
    return bytes;
}

// Функция для преобразования байтов в hex строку
std::string bytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');  
    for (size_t i = 0; i < len; i++) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

// Функция для вычисления SHA-256 с использованием EVP API (новый способ)
std::string sha256(const unsigned char* data, size_t len) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP context");
    }
    
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize digest");
    }
    
    if (EVP_DigestUpdate(ctx, data, len) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update digest");
    }
    
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize digest");
    }
    
    EVP_MD_CTX_free(ctx);
    return bytesToHex(hash, hashLen);
}

// Функция для вычисления двойного SHA-256
std::string doubleSha256(const unsigned char* data, size_t len) {
    std::string firstHash = sha256(data, len);
    std::vector<unsigned char> firstHashBytes = hexToBytes(firstHash);
    return sha256(firstHashBytes.data(), firstHashBytes.size());
}


int main(int argc, char* argv[]) {
    
    std::cout << "SHA-256 Hash Calculator for 80-byte number" << std::endl;
    std::cout << "============================================" << std::endl;
    
    bool doubleHash = false;
    std::string inputHex;
    
    // Парсим аргументы
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-d" || arg == "--double") {
            doubleHash = true;
        } else if (arg == "-s" || arg == "--single") {
            doubleHash = false;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options] [hex_string]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -d, --double    Compute double SHA-256 hash (default)" << std::endl;
            std::cout << "  -s, --single    Compute single SHA-256 hash" << std::endl;
            std::cout << "  -h, --help      Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "If hex_string is not provided, it will be read from stdin." << std::endl;
            std::cout << "Input must be 80 bytes (160 hex characters) or less." << std::endl;
            return 0;
        } else {
            inputHex = arg;
        }
    }
    
    // Если hex строка не передана как аргумент, читаем из stdin
    if (inputHex.empty()) {
        std::cout << "Enter 80-byte number in hex (max 160 characters): ";
        std::getline(std::cin, inputHex);
        
        if (inputHex.empty() && !std::cin.eof()) {
            std::getline(std::cin, inputHex);
        }
    }
    
    try {
        std::vector<unsigned char> bytes = hexToBytes(inputHex);
        
        if (bytes.size() > 80) {
            std::cerr << "Error: Input is " << bytes.size() 
                      << " bytes, but maximum is 80 bytes" << std::endl;
            return 1;
        }
        
        std::cout << "Input length: " << bytes.size() << " bytes" << std::endl;
        std::cout << "Input (hex): " << bytesToHex(bytes.data(), bytes.size()) << std::endl;
        
        std::string result;
        if (doubleHash) {
            result = doubleSha256(bytes.data(), bytes.size());
            std::cout << "Hash type: DOUBLE SHA-256" << std::endl;
        } else {
            result = sha256(bytes.data(), bytes.size());
            std::cout << "Hash type: SINGLE SHA-256" << std::endl;
        }
        
        std::cout << "Hash result: " << result << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}