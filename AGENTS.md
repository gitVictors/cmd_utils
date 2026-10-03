# AGENTS.md — cmd_utils

Инструкции для AI-агентов. Читай перед изменениями.

## Что это за проект

Консольный C++20-проект на CMake (`project(UtilitieResearch)`), собирающий две
независимые утилиты для исследования «нулевых паттернов» SHA-256:

- **`sha256_calculator`** — калькулятор одиночного/двойного SHA-256 для 80-байтного
  заголовка блока Bitcoin (через OpenSSL EVP).
- **`ZeroCountAverage`** — усредняет поле `zeroCount` по массиву `results` из
  JSON-файла, сгенерированного `../cmd_bitcoin_analyzer`.

- Git remote: https://github.com/gitVictors/cmd_utils.git, последний коммит `30e2850`.
- Кодировка: все файлы UTF-8 **без BOM**, переносы строк **LF**.

## Структура

| Файл | Назначение |
|---|---|
| `CMakeLists.txt` | Проект, два executable-таргета, пути vcpkg, линковка OpenSSL |
| `sha256_calculator.cpp` | Исходник утилиты `sha256_calculator` |
| `zc.cpp` | Исходник утилиты `ZeroCountAverage` |
| `REAME.md` | Документация (имя с опечаткой, должно быть `README.md`) |
| `.gitignore` | Игнор сборки/VS-артефактов/`*.json` |

## Сборка

Зависимости через vcpkg (`C:/dev/vcpkg`): `openssl:x64-windows`,
`nlohmann-json:x64-windows`. Windows-библиотеки: `ws2_32`, `crypt32`, `bcrypt`.

```powershell
cmake -B build
cmake --build build --config Debug     # или --config Release
```

Выходные файлы:
- `build/Debug/ZeroCountAverage.exe`
- `build/Debug/sha256_calculator.exe`
- рядом нужен `libcrypto-3-x64.dll` (OpenSSL линкуется **динамически**, несмотря на
  комментарии про «статическую» версию и define `OPENSSL_STATIC`).

При правке путей — `CMakeLists.txt:9,11,18-20`.

## Запуск

### sha256_calculator
```
sha256_calculator.exe [options] [hex_string]
  -d, --double    двойной SHA-256 (заявлен как default)
  -s, --single    одиночный SHA-256
  -h, --help      справка
```
Вход: hex-строка аргументом (до 160 hex = 80 байт, пробелы/переносы игнорируются)
или из stdin. Вывод: `Input length`, `Hash type`, `Hash result: <64 hex>`.

### ZeroCountAverage
```
ZeroCountAverage.exe <json_file_path>
```
Вывод: `Average zeroCount: <число>` и `Total entries: <N>`.

## Формат данных

`ZeroCountAverage` читает JSON вида (генерирует `cmd_bitcoin_analyzer`):
```json
{
  "numBlock": 953705, "nonceSolution": 629180716,
  "nonceStart": 0, "nonceEnd": 1000, "nonceStep": 1,
  "results": [ { "nonce": 0, "zeroCount": 90 }, { "nonce": 1, "zeroCount": 96 } ]
}
```
Обязателен только массив `results` с целочисленным `zeroCount` в каждом элементе.

## Известные проблемы / подводные камни

- **Несоответствие default-режима SHA-256:** в коде `bool doubleHash = false;`
  (`sha256_calculator.cpp:95`) → по умолчанию считается **одиночный** SHA-256, но help
  (`:108`) и `REAME.md` утверждают, что default — двойной. Пример из README без флага
  фактически даст single.
- `REAME.md` вместо `README.md`; примеры hex разбиты переносом строки — при копировании
  нужно склеивать в один аргумент.
- OpenSSL на самом деле динамический (`libcrypto-3-x64.dll`), комментарии о статике врут.
- `find_path(... REQUIRED)` при `cmake_minimum_required(VERSION 3.10)` требует CMake ≥3.18
  (локально CMake 4.4.0 — работает).
- `CMAKE_GENERATOR_PLATFORM` задаётся после `project()` — на VS-генератор не влияет
  (нужен `-A x64`).
- Слабая валидация hex (`strtoul` принимает `+1`); неизвестные флаги трактуются как hex.
- Нет явных include `<cstdlib>`/`<stdexcept>`; `zc.cpp` учитывает только
  `is_number_integer()` и читает `zeroCount` как `int`.

## Связь с cmd_bitcoin_analyzer

`result_stat/run.sh` (в `../../cmd_bitcoin_analyzer`) вызывает
`../../utilits_for_bitcoin/cmd_utils/build/Debug/ZeroCountAverage` на JSON-файле,
который сгенерировал анализатор (`rslt_<solution>_<start>_<end>.json`). Формат `results`
должен оставаться совместимым.

## Правила работы агента

- Проект исторически в UTF-8 без BOM и LF. Для новых/изменяемых файлов придерживайся
  UTF-8 with BOM (CRLF) — как принято в `../cmd_bitcoin_analyzer`; не допускай смешанной
  кодировки внутри файла.
- Рабочие отчёты/скрипты агента — в `AI/` (создать при необходимости).
- Не коммить без явной просьбы.
- После правок — собери (`cmake --build build`) и проверь запуск обеих утилит.
