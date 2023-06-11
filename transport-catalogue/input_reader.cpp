#include "input_reader.h"

#include <utility>

namespace transport_catalog {

    namespace input {


        void ReadFromInput(std::istream& input, TransportCatalogue& tc, bool print_to_file) {

            std::string line; // считанная строка из потока    
            int count{ 0 };     // количество запросов
            std::getline(input, line);

            count = std::stoi(line);

            std::vector<std::string> v_queries_to_create_buses;
            v_queries_to_create_buses.reserve(count);

            std::vector<std::string> v_queries_to_create_stops;
            v_queries_to_create_stops.reserve(count);

            std::vector<std::string> v_queries_to_db;

            bool b_query_to_create_db = true;

            // соберем все запросы
            while (std::getline(input, line)) {
                if (b_query_to_create_db) {
                    if (line.find("Bus") != std::string::npos) {
                        v_queries_to_create_buses.push_back(std::move(line));
                    }
                    else if (line.find("Stop") != std::string::npos) {
                        v_queries_to_create_stops.push_back(std::move(line));
                    }
                    else {
                        //уже пошел раздел запросов к БД
                        count = std::stoi(line);
                        v_queries_to_db.reserve(count);
                        b_query_to_create_db = false;
                    }
                }
                else {
                    v_queries_to_db.push_back(std::move(line));
                }
            }

            // сначала обработаем остановки
            HandleRequestsToCreateStop(v_queries_to_create_stops, tc);
            // обработаем маршруты
            HandleRequestsToCreateBus(v_queries_to_create_buses, tc);

            reader::HandleRequests(v_queries_to_db, tc, print_to_file);
        }

        void HandleRequestsToCreateStop(const std::vector<std::string>& v_queries, TransportCatalogue& tc) {

            std::unordered_map<TransportCatalogue::Stop*, std::vector<std::string>> um_distance_to_stops;

            for (const std::string& query : v_queries) {

                std::string name = GetName(query);

                std::vector<std::string> v_substr = str_edit::Separate(query, ",", query.find(":"));

                double lat, lng;

                lat = std::stod(v_substr[0]);
                lng = std::stod(v_substr[1]);

                TransportCatalogue::Stop* stop = tc.AddStop(name, lat, lng);

                // сохраним данные по дистанциям для остановки если есть
                if (v_substr.size() > 2) {
                    for (size_t i = 2; i < v_substr.size(); ++i) {
                        um_distance_to_stops[stop].push_back(std::move(v_substr[i]));
                    }

                }


            }

            AddDistanceForStops(um_distance_to_stops, tc);
        }

        void HandleRequestsToCreateBus(const std::vector<std::string>& v_queries, TransportCatalogue& tc) {

            for (const std::string& query : v_queries) {

                std::string name = GetName(query);

                std::string separate = ">";
                if (query.find('>') == std::string::npos) {
                    separate = "-";
                }
                std::vector<std::string> v_buffer = str_edit::Separate(query, separate, query.find(":"));
                std::vector<std::string> v_stops(v_buffer);

                //    // дополним маршрут
                if (separate == "-") {
                    for (auto it = v_buffer.rbegin() + 1; it != v_buffer.rend(); ++it) {
                        v_stops.push_back(*it);
                    }
                }

                tc.AddBus(name, v_stops);

            }

        }

        std::string GetName(const std::string& str) {

            size_t pos_start = str.find("Bus");
            const size_t pass_symbols = (pos_start == std::string::npos ? 5 : 4);
            // если первое слово не "Bus", значит это "Stop"
            if (pos_start == std::string::npos) {
                pos_start = str.find("Stop");
            }

            pos_start = str.find_first_not_of(' ', pos_start + pass_symbols);
            size_t pos_end = str.find(":");

            std::string name = str.substr(pos_start, (pos_end - pos_start));
            //str_edit::TrimR(name);
            str_edit::TrimAll(name);

            return name;

        }

        void AddDistanceForStops(const std::unordered_map<TransportCatalogue::Stop*, std::vector<std::string>>& un_distance, TransportCatalogue& tc) {

            for (const auto& [stop1, v_dist] : un_distance) {

                for (const std::string& str : v_dist) {
                    std::string stop_name;
                    size_t dist = 0;
                    // найдем дистанцию
                    size_t pos = str.find('m');
                    if (pos != std::string::npos) {
                        dist = std::stoi(str.substr(0, pos));
                    }
                    // найдем имя остановки
                    pos = str.find(" to ");
                    if (pos != std::string::npos) {
                        stop_name = str.substr(pos + 4);
                    }

                    if (stop_name == "") { continue; }

                    str_edit::TrimL(stop_name);

                    TransportCatalogue::Stop* stop2 = tc.FindStop(stop_name);

                    if (stop2 != nullptr) {
                        tc.SetDistanceBetweenStops(stop1, stop2, dist);
                    }

                }
            }

        }

        namespace str_edit {

            void TrimL(std::string& str) {
                size_t pos = str.find_first_not_of(' ');

                if (pos != std::string::npos) {
                    str = str.substr(pos);
                }
            }

            void TrimR(std::string& str) {
                size_t pos = str.find_last_not_of(' ');

                if (pos != std::string::npos) {
                    str = str.substr(0, pos + 1);
                }
            }

            void TrimAll(std::string& str) {
                TrimL(str);
                TrimR(str);
            }

            std::vector<std::string> Separate(const std::string& query, const std::string& separator, size_t pos) {
                std::vector<std::string> v_result;

                size_t pos_start = pos; // начальный индекс

                while (pos_start != std::string::npos) {
                    size_t pos_end = query.find(separator, pos_start + 1); // конечный индекс
                    std::string str;
                    // pos_start + 1 - исключаем попадение разделителей в вектор, они не нужны
                    if (pos_end == std::string::npos) {
                        str = query.substr(pos_start + 1);
                    }
                    else {
                        str = query.substr(pos_start + 1, (pos_end - pos_start) - 1); // -1 - исключаем разделитель
                    }

                    str_edit::TrimAll(str);
                    v_result.push_back(str);
                    pos_start = pos_end; // перемещаем индекс на позицию после завершения слова в тексте
                }

                return v_result;
            }

        }


    }  //input

} //transport_catalog