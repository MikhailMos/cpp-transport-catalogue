#include "stat_reader.h"


namespace transport_catalog {

    namespace reader {

        void HandleRequests(const std::vector<std::string>& v_queries, TransportCatalogue& tc, bool print_to_file) {

            for (const std::string& query : v_queries) {
                size_t pos_start = query.find("Bus");

                if (pos_start != std::string::npos) {
                    pos_start = query.find_first_not_of(' ', pos_start + 4);

                    std::string name = query.substr(pos_start);
                    // проверим есть ли пробелы вконце
                    pos_start = name.find_last_not_of(' ');
                    if (pos_start != std::string::npos) {
                        name = name.substr(0, pos_start + 1);
                    }

                    const TransportCatalogue::Bus* bus = tc.GetBusInfo(name);

                    transport_catalog::reader::ReadAndOutBus(tc, name, bus, print_to_file);
                }
                else {
                    pos_start = query.find("Stop");
                    pos_start = query.find_first_not_of(' ', pos_start + 5);

                    std::string name = query.substr(pos_start);
                    // проверим есть ли пробелы вконце
                    pos_start = name.find_last_not_of(' ');
                    if (pos_start != std::string::npos) {
                        name = name.substr(0, pos_start + 1);
                    }

                    const std::tuple<bool, std::set<std::string_view>*> tup_buses = tc.GetStopInfo(name);

                    ReadAndOutStop(name, tup_buses, print_to_file);

                }

            }

        }

        

        void ReadAndOutBus(TransportCatalogue& tc, const std::string_view bus_name, const TransportCatalogue::Bus* bus, bool print_to_file) {
            /*
            Bus X: R stops on route, U unique stops, L route length, C curvature
            Здесь

                X — название маршрута. Оно совпадает с названием, переданным в запрос Bus.
                R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
                U — количество уникальных остановок, на которых останавливается автобус. Одинаковыми считаются остановки, имеющие одинаковые названия.
                L — длина маршрута в метрах (фактическая длина, в метрах).
                С — извилистость, то есть отношение фактической длины маршрута к географическому расстоянию. Географическая длина вычисляется по координатам

            !!! Если маршрут X не найден, выведите Bus X: not found. !!!
            */

            size_t amount_of_unique_stops = 0;
            double geo_length_of_distance = 0.0;
            size_t length_of_distance = 0;

            if (bus != nullptr) {
                amount_of_unique_stops = detail::GetUniqueStops(bus->stops);
                geo_length_of_distance = detail::GetGeoLengthOfDistance(bus->stops);
                length_of_distance = detail::GetLengthOfDistance(tc, bus->stops);
            }

            detail::OutBus(std::cout, bus_name, bus, amount_of_unique_stops, length_of_distance, geo_length_of_distance);

            if (print_to_file) {
                std::ofstream fout;
                fout.open("my_result.txt", std::ios::app);

                detail::OutBus(fout, bus_name, bus, amount_of_unique_stops, length_of_distance, geo_length_of_distance);

                fout.close();
            }


        }

        void ReadAndOutStop(const std::string_view stop_name, const std::tuple<bool, std::set<std::string_view>*>& tup_buses, bool print_to_file) {

            bool stop_is_exist = std::get<0>(tup_buses);
            std::set<std::string_view>* s_buses = std::get<1>(tup_buses);

            detail::OutStop(std::cout, stop_name, stop_is_exist, s_buses);

            if (print_to_file) {
                std::ofstream fout;
                fout.open("my_result.txt", std::ios::app);

                detail::OutStop(fout, stop_name, stop_is_exist, s_buses);

                fout.close();
            }

        }

        namespace detail {

            void OutBus(std::ostream& output, const std::string_view bus_name, const TransportCatalogue::Bus* bus, const size_t amount_of_unique_stops, const size_t length_of_distance, const double geo_length) {

                if (bus == nullptr) {
                    output << "Bus " << bus_name << ": not found" << std::endl;
                    return;
                }

                output << "Bus " << bus->name << ": "
                    << bus->stops.size()
                    << " stops on route, "
                    << amount_of_unique_stops
                    << " unique stops, "
                    << length_of_distance
                    << " route length, "
                    << (static_cast<double>(length_of_distance) / geo_length)
                    << " curvature"
                    << std::endl;

            }

            void OutStop(std::ostream& output, const std::string_view stop_name, const bool stop_is_exist, const std::set<std::string_view>* s_buses) {

                output << "Stop " << stop_name << ": ";

                if (stop_is_exist == false) {
                    output << "not found" << std::endl;
                    return;
                }
                else if (s_buses->size() == 0) {
                    output << "no buses" << std::endl;
                    return;
                }

                output << "buses ";

                for (const std::string_view& bus : *s_buses) {
                    output << bus << " ";
                }

                output << std::endl;

            }

            size_t GetUniqueStops(const std::vector<TransportCatalogue::Stop*>& v_stops) {

                std::unordered_set<std::string_view> un_set_unique_stops;

                for (const auto& stop : v_stops) {
                    un_set_unique_stops.emplace(stop->name);
                }

                return un_set_unique_stops.size();
            }

            double GetGeoLengthOfDistance(const std::vector<TransportCatalogue::Stop*>& v_stops) {
                double dbl_result = 0;
                // начинаем со второй остановки, т.к. нужно подсчитать сумму от первой до второй
                for (size_t i = 0; i < v_stops.size() - 1; ++i) {
                    dbl_result += transport_catalog::detail::ComputeDistance(v_stops[i]->coordinates, v_stops[i + 1]->coordinates);
                }

                return dbl_result;
            }

            size_t GetLengthOfDistance(TransportCatalogue& tc, const std::vector<TransportCatalogue::Stop*>& v_stops) {

                size_t result = 0;

                for (size_t i = 0; i < v_stops.size() - 1; ++i) {
                    result += tc.GetDistanceBetweenStops(v_stops[i], v_stops[i + 1]);
                }

                return result;
            }

        } //detail

    } //reader

} //transport_catalog