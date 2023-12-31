# cpp-transport-catalogue
Финальный проект: транспортный справочник

Графический вывод, поиск маршрутов и вычисление времени поездки.
Результатом работы программы будет файл в формате JSON, который содержит SVG-изображение карты, а так же данные по запросам. Картинка подобна этой:

![картинка](https://github.com/MikhailMos/MikhailMos/blob/main/images/TrasportCatalogue.png)

### Системные требования

[Protobuf](https://protobuf.dev/ "Документация по протоколу") (версии 3.21.12)</br>
[CMake](https://cmake.org/download/ "Cкачать") (версия — не ниже 3.11.)

### Собрать пакет protobuf

Собрать пакет для этого нужно [скачать необходимый релиз](https://github.com/protocolbuffers/protobuf).
Релиз для проекта нужен версии 3.21.12, код доступен по [ссылке](https://github.com/protocolbuffers/protobuf/releases/tag/v21.12)

Создайте директории build-debug и build-release для сборки двух конфигураций Protobuf. Если вы используете Visual Studio, будет достаточно одной директории build. А если CLion или QT Creator, IDE автоматически создаст директории при открытии файла CMakeLists.txt. 

Если вы собираете не через IDE, в директории build-debug выполните следующие команды:

```
cmake <путь к распакованному архиву>/protobuf-3.21.12  \
      -DCMAKE_BUILD_TYPE=Debug                         \
      -Dprotobuf_BUILD_TESTS=OFF                       \
      -DCMAKE_INSTALL_PREFIX=<путь к месту установки пакета>/Protobuf
cmake --build .
```

Для Visual Studio команды немного другие. Конфигурация указывается не при генерации, а при сборке:

```
cmake <путь к распакованному архиву>/protobuf-3.21.12 ^
      -Dprotobuf_BUILD_TESTS=OFF ^
      -DCMAKE_INSTALL_PREFIX=<путь к месту установки пакета>/Protobuf ^
      -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake --build . --config Debug
```

Теперь запустим установку командой:

```
cmake --install . 
```

Под Visual Studio нужно указать конфигурацию, поскольку она не задавалась во время генерации:

```
cmake --install . --config Debug 
```

И CMake скопирует все необходимые файлы в заранее подготовленное место <путь к месту установки пакета>/Protobuf. Структура директории должна получиться такой:

```
Protobuf/
├── bin/
│   └── protoc (или protoc.exe для Windows)
├── include/google/protobuf/
│   └── include-файлы
└── lib/
    ├── pkgconfig/
    ├── lib*.a (lib*.lib при сборке под Visual Studio)
    └── ...
```

Проделайте те же шаги для конфигурации Release. Устанавливайте в ту же директорию. Так как статические библиотеки, собранные в конфигурации Debug, имеют суффикс `d`, конфликта не возникнет.


### Сборка проекта припомощи CMake

Для сборки проекта понадобится отдельная папка, в которой будут размещаться вспомогательные файлы CMake, промежуточные файлы компиляции и итоговая программа. Важно, чтобы эта папка не совпадала с папкой, где размещён исходный код, чтобы не засорять его лишними файлами. Создайте папку cmake_build. Структура каталогов может получиться такой:

```
cmake_project/
├── hello_cmake/ # папка с исходным кодом
│   ├── src/ # вспомогательная папка для структурирования исходного кода
│   │   ├── hello_printer.cpp
│   │   ├── hello_printer.h
│   │   └── main.cpp
│   └── CMakeLists.txt # файл проекта CMake
└── cmake_build/ # папка для сборки
    └── # тут будут файлы сборки
```

В консоли перейдите в папку cmake_build.
В качестве первого параметра задается путь к папке, в которой находится CMakeLists.txt.
Чтобы задать конфигурацию, нужно добавить дополнительный параметр `-DCMAKE_BUILD_TYPE=...` при генерации файлов сборки и выбрать одну из четырёх поддерживаемых конфигураций:

- Release — сборка с полной оптимизацией и без отладочной информации, предназначена для конечного пользователя.
- Debug — сборка без оптимизации и с отладочной информацией.
- RelWithDebInfo — сборка с оптимизацией и с отладочной информацией. Нужна, если в Release версии есть баги, которые не получается выявить в Debug версии.
- MinSizeRel — аналог Release, но оптимизируется в первую очередь размер итогового файла, а не скорость. Имеет смысл для сборки программ под устройства, у которых жёстко ограничено дисковое пространство.

Запустите команду генерации:

```
cmake ../hello_cmake -DCMAKE_BUILD_TYPE=Debug
```

В примере показан способо с генератором по умолчанию. Под Windows используйте генератор "MinGW Makefiles". Команда запуска будет выглядеть так:

```
cmake ../hello_cmake -G "MinGW Makefiles"
```

Вторая команда — универсальный механизм, позволяющий выполнить сборку.
В качестве параметра CMake, помимо ключа `--build`, указана директория, содержащая сборочные файлы — в данном случае указана `.`, то есть текущая директория.

```
cmake --build . 
```
