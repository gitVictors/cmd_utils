cmake_minimum_required(VERSION 3.10)
project(UtilitieResearch)

set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Platform" FORCE)

set(CMAKE_CXX_STANDARD 20)

# Путь к библиотекам vcpkg
set(VCPKG_ROOT "C:/dev/vcpkg")

set(NLOHMANN_JSON_INCLUDE_DIR "C:/dev/vcpkg/packages/nlohmann-json_x64-windows/include" CACHE PATH "nlohmann/json include directory")

# find_package(nlohmann_json CONFIG REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR NAMES nlohmann/json.hpp PATHS ${NLOHMANN_JSON_INCLUDE_DIR} REQUIRED)


# Настройка для OpenSSL (статическая библиотека)
set(OPENSSL_ROOT_DIR "${VCPKG_ROOT}/packages/openssl_x64-windows-static" CACHE PATH "OpenSSL root directory")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARY_DIR "${OPENSSL_ROOT_DIR}/lib")

#=========================================================

add_executable(ZeroCountAverage zc.cpp)
add_executable(sha256_calculator sha256_calculator.cpp)

#==========================================================

target_include_directories(ZeroCountAverage PRIVATE ${NLOHMANN_JSON_INCLUDE_DIR})

# Для sha256_calculator (нужны заголовки и библиотеки OpenSSL)
target_include_directories(sha256_calculator PRIVATE ${OPENSSL_INCLUDE_DIR})

# Линкуем OpenSSL (статическая версия)
target_link_libraries(sha256_calculator PRIVATE
    ${OPENSSL_LIBRARY_DIR}/libcrypto.lib
    ${OPENSSL_LIBRARY_DIR}/libssl.lib
    ws2_32.lib
    crypt32.lib
    bcrypt.lib
)

# Опционально: добавляем определение для статической линковки
target_compile_definitions(sha256_calculator PRIVATE OPENSSL_STATIC)