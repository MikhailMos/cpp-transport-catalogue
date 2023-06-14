#pragma once

#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <fstream>

#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalog {

	namespace reader {

		// разбирает запрос и получает данные от транспотного справочника
		void HandleRequests(const std::vector<std::string>& v_queries, TransportCatalogue& tc, bool print_to_file = false);

		// структурирует полученные данные от транспортного справочника и выводит
		void ReadAndOutBus(TransportCatalogue& tc, const std::string_view bus_name, const TransportCatalogue::Bus* v_stops, bool print_to_file);
		void ReadAndOutStop(const std::string_view stop_name, const std::tuple<bool, std::set<std::string_view>*>& tup_buses, bool print_to_file);

		namespace detail {

			// возвращает кол-во уникальных остановок
			size_t GetUniqueStops(const std::vector<TransportCatalogue::Stop*>& v_stops);

			// возвращает географическую длину маршрута
			double GetGeoLengthOfDistance(const std::vector<TransportCatalogue::Stop*>& v_stops);
			// возвращает фактическую длину маршрута
			size_t GetLengthOfDistance(TransportCatalogue& tc, const std::vector<TransportCatalogue::Stop*>& v_stops);

			// вывод информации
			void OutBus(std::ostream& output, const std::string_view bus_name, const TransportCatalogue::Bus* bus, const size_t amount_of_unique_stops, const size_t length_of_distance, const double geo_length);

			void OutStop(std::ostream& output, const std::string_view stop_name, const bool stop_is_exist, const std::set<std::string_view>* s_buses);
			

		} //detail

	} //reader

} //transport_catalog