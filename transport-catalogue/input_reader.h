#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"
#include "stat_reader.h"

namespace transport_catalog {

    namespace input {


        // читаем поток и раскладываем запросы
        void ReadFromInput(std::istream& input, TransportCatalogue& tc, bool print_to_file = false);

        // обработаем запросы на создание в бд
        void HandleRequestsToCreateStop(const std::vector<std::string>& v_queries, TransportCatalogue& tc);
        void HandleRequestsToCreateBus(const std::vector<std::string>& v_queries, TransportCatalogue& tc);

        // возвращает имя (остановки или автобуса) из запроса
        std::string GetName(const std::string& str);
        // заполняет дистанцию для остановки
        void AddDistanceForStops(const std::unordered_map<TransportCatalogue::Stop*, std::vector<std::string>>& un_distance, TransportCatalogue& tc);

        namespace str_edit {
            // убирает пробелы из начала строки
            void TrimL(std::string& str);
            // убирает пробелы из конца строки
            void TrimR(std::string& str);
            // убирает пробелы из начала и из конца строки
            void TrimAll(std::string& str);

            // разбивает строку на отдельные подстроки и возвращает вектор подстрок
            //  query - строка которую нужно разбить
            //  separator - разделитель, по которому будет разбита строка на подстроки
            //  pos - позиция (индекс) с которого нужно начинать разбивать строку на подстроки
            std::vector<std::string> Separate(const std::string& query, const std::string& separator, size_t pos = 0);
        }

    } //input

} //transport_catalog