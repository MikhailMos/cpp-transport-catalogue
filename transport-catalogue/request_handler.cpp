#include "request_handler.h"

namespace transport_catalog {
	
	BusStat::BusStat(std::string_view sv_name, double geo_length, double length, size_t count, size_t unique_count)
		: name(sv_name)
		, geo_route_length(geo_length)
		, route_length(length)
		, stop_count(count)
		, unique_stop_count(unique_count)
		, curvature(0.0)
	{
		if (route_length != 0.0 && geo_route_length != 0.0) {
			curvature = route_length / geo_route_length;
		}
	}

	std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
		
		const Bus* bus = db_.GetBusInfo(bus_name);
		if (!bus) {
			return std::nullopt;
		}

		
		size_t count = bus->stops.size();
		if (!bus->is_roundtrip) { count = (count * 2) - 1; }

		size_t unique_count = GetUniqueStops(bus->stops);
		double route_length = GetLengthOfDistance(bus->stops, bus->is_roundtrip);
		double geo_route_length = GetGeoLengthOfDistance(bus->stops, bus->is_roundtrip);
		
		return std::optional<BusStat>({bus_name, geo_route_length, route_length, count, unique_count});
	}

	std::optional<std::set<std::string_view>*> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
		std::tuple<bool, std::set<std::string_view>*> tup_buses = db_.GetStopInfo(stop_name);

		if (std::get<0>(tup_buses)) {
			return std::optional<std::set<std::string_view>*>(std::get<1>(tup_buses));
		}
		
		return std::optional<std::set<std::string_view>*>();
	}

	const std::unordered_map<std::string_view, Bus*>& RequestHandler::GetAllBuses() const {
		return db_.GetAllBuses();
	}

	json::Document RequestHandler::ReadAndExecuteRequests(std::istream& input) {
		JSONReader json_reader;
		const Requests requests = json_reader.Read(input);
		
		// запросы на добавление информации в базу
		if (requests.base_requests.size()) {
			ToBase(requests.base_requests);
		}

		// запрос настроек для карты
		if (requests.render_settings.size()) {
			SettingsForMap(requests.render_settings);
		}

		// запросы к траспортному каталогу
		json::Array arr_answers;
		if (requests.stat_requests.size()) {
			arr_answers = ToTransportCataloque(requests.stat_requests);
		}

		return json::Document{ arr_answers };
	}

	void RequestHandler::ToBase(const json::Array& arr_nodes) {

		// первым проходом обрабатываем все остановки
		for (int i = 0; i < 2; ++i) {

			std::unordered_map<Stop*, json::Dict> um_distance_to_stops;

			for (const auto& node : arr_nodes) {
				if (!node.IsDict()) { throw std::invalid_argument("Wrong into file structure"); }

				const json::Dict& map_value = node.AsDict();
				if (map_value.count("type") == 0) { throw std::invalid_argument("Wrong into file structure"); }

				const std::string& type = map_value.at("type").AsString();

				// обработаем остановки
				if (i == 0 && type == "Stop") {
					Stop* stop = CreateStop(map_value);
					// сохраним дорожное расстояние от этой остановки до соседних.
					if (map_value.count("road_distances")) {
						um_distance_to_stops[stop] = map_value.at("road_distances").AsDict();
					}
				}

				// обработаем автобусы
				if (i == 1 && type == "Bus") {
					CreateBus(map_value);
				}

			}

			if (um_distance_to_stops.size() > 0) {
				AddDistanceForStops(um_distance_to_stops);
			}

		}

	}

	json::Array RequestHandler::ToTransportCataloque(const json::Array& arr_nodes) {
		
		json::Array out_arr_nodes;

		for (const json::Node& node : arr_nodes) {
			if (!node.IsDict()) { throw std::invalid_argument("Wrong into file structure"); }

			const json::Dict& map_value = node.AsDict();

			if (map_value.count("type") == 0) { throw std::invalid_argument("Wrong into file structure"); }
			const std::string& type = map_value.at("type").AsString();

			if (map_value.count("id") == 0) { throw std::invalid_argument("Wrong into file structure"); }
			const int request_id = map_value.at("id").AsInt();

			std::string name;
			if (map_value.count("name")) {
				name = map_value.at("name").AsString();
			}

			json::Dict dict;

			if (type == "Stop") {
				std::optional<std::set<std::string_view>*> stop_info = GetBusesByStop(name);
				if (stop_info) {
					dict = OutStopInfo(request_id, stop_info);
				}
			}
			else if (type == "Bus") {

				std::optional<BusStat> bus_info = GetBusStat(name);
				if (bus_info) {
					dict = OutBusInfo(request_id, bus_info);
				}

			}
			else if (type == "Map") {
				dict = OutMap(request_id);
			}
			/*else {
				throw std::invalid_argument("Wrong into file structure");
			}*/

			if (dict.size() == 0) {
				dict = GetNotFoundNode(map_value.at("id").AsInt());
			}

			out_arr_nodes.push_back(dict);
		}

		return out_arr_nodes;
	}

	void RequestHandler::SettingsForMap(const json::Dict& dict_node) {

		renderer::MapSettings settings;
		// распарсим и заполним настройки
		for (const auto& [key, val] : dict_node) {

			if ((key == "bus_label_font_size" || key == "stop_label_font_size") && val.IsInt()) {
				settings.SetValue(key, static_cast<size_t>(val.AsInt()));
			}
			else if (val.IsDouble()) {
				settings.SetValue(key, val.AsDouble());
			}
			else if (val.IsString()) {
				settings.SetValue(key, val.AsString());
			}
			else if (val.IsArray()) {
				const json::Array& arr = val.AsArray();

				if (key == "bus_label_offset" || key == "stop_label_offset") { //svg::Point
					settings.SetValue(key, svg::Point(arr[0].AsDouble(), arr[1].AsDouble()));
				}
				else if (key == "underlayer_color") { //svg::Color
					if (arr.size() == 3) {
						settings.SetValue(key, svg::Rgb(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt()));
					}
					else {
						settings.SetValue(key, svg::Rgba(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble()));
					}
				}
				else if (key == "color_palette") {  //std::vector<svg::Color> 
					std::vector<svg::Color> v_colors;
					v_colors.reserve(arr.size());

					for (const json::Node& elem : arr) {
						if (elem.IsString()) {
							v_colors.push_back(elem.AsString());
						}
						else if (elem.IsArray()) {
							const json::Array& arr_color = elem.AsArray();
							if (arr_color.size() == 3) {
								v_colors.push_back(svg::Rgb(arr_color[0].AsInt(), arr_color[1].AsInt(), arr_color[2].AsInt()));
							}
							else {
								v_colors.push_back(svg::Rgba(arr_color[0].AsInt(), arr_color[1].AsInt(), arr_color[2].AsInt(), arr_color[3].AsDouble()));
							}
						}
					}

					settings.SetValue(key, v_colors);
				}

			}

		}

		renderer_.SetSettings(settings);

	}

	size_t RequestHandler::GetUniqueStops(const std::vector<Stop*>& v_stops) const {
		
		std::unordered_set<std::string_view> un_set_unique_stops;

		for (const auto& stop : v_stops) {
			un_set_unique_stops.emplace(stop->name);
		}

		return un_set_unique_stops.size();

	}

	double RequestHandler::GetGeoLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const {
		double dbl_result = 0;
		// начинаем со второй остановки, т.к. нужно подсчитать сумму от первой до второй
		for (size_t i = 0; i < v_stops.size() - 1; ++i) {
			double dbl_dist = ComputeDistance(v_stops[i]->coordinates, v_stops[i + 1]->coordinates);
			dbl_result += dbl_dist;
		}

		// если не кольцевой, сразу же учтем и обратный путь, кроме последнего отрезка
		if (!is_roundtrip) { dbl_result *= 2; }

		return dbl_result;
	}

	double RequestHandler::GetLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const {
		double result = 0;

		for (size_t i = 0; i < v_stops.size() - 1; ++i) {
			double distance = db_.GetDistanceBetweenStops(v_stops[i], v_stops[i + 1]);
			result += distance;
		}

		// если не кольцевой, пройдем отрезки в обратную сторону (вдруг расстояние изменилось)
		if (!is_roundtrip) { 
			for (size_t i = 0; i < v_stops.size() - 1; ++i) {
				double distance = db_.GetDistanceBetweenStops(v_stops[i + 1], v_stops[i]);
				result += distance;
			}
		}

		return result;
	}

	Stop* RequestHandler::CreateStop(const json::Dict map_stop) {
		return db_.AddStop(map_stop.at("name").AsString(), map_stop.at("latitude").AsDouble(), map_stop.at("longitude").AsDouble());
	}

	void RequestHandler::AddDistanceForStops(const std::unordered_map<transport_catalog::Stop*, json::Dict>& un_distance) {

		for (const auto& [stop1, map_dist] : un_distance) {

			for (const auto& [stop_name, node_dist] : map_dist) {

				Stop* stop2 = db_.FindStop(stop_name);
				if (stop2 != nullptr) {
					db_.SetDistanceBetweenStops(stop1, stop2, node_dist.AsInt());
				}

			}

		}

	}

	Bus* RequestHandler::CreateBus(const json::Dict map_bus) {
		
		const json::Array& stops = map_bus.at("stops").AsArray();
		std::vector<std::string> v_stops;
		v_stops.reserve(stops.size());

		for (const json::Node& n_stop : stops) {
			v_stops.push_back(n_stop.AsString());
		}

		return db_.AddBus(map_bus.at("name").AsString(), v_stops, map_bus.at("is_roundtrip").AsBool());
		
	}

	json::Dict RequestHandler::GetNotFoundNode(const int id) {
		
		using namespace std::literals;

		return json::Builder{}
			.StartDict()
				.Key("request_id"s).Value(id)
				.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build()
			.AsDict();
	}

	json::Dict RequestHandler::OutBusInfo(const int id, const std::optional<BusStat>& bus_info) {
		
		using namespace std::literals;

		return json::Builder{}
			.StartDict()
				.Key("request_id"s).Value(id)
				.Key("curvature"s).Value(bus_info->curvature)
				.Key("route_length"s).Value(bus_info->route_length)
				.Key("stop_count"s).Value(bus_info->stop_count)
				.Key("unique_stop_count"s).Value(bus_info->unique_stop_count)
			.EndDict()
			.Build()
			.AsDict();

	}

	json::Dict RequestHandler::OutStopInfo(const int id, const std::optional<std::set<std::string_view>*>& stop_info) {
		
		using namespace std::literals;

		json::Builder builder;

		builder.StartDict()
			.Key("request_id"s).Value(id)
			.Key("buses"s).StartArray();

		for (const auto& el : *stop_info.value()) {
			builder.Value(static_cast<std::string>(el));
		}
		
		builder.EndArray()
			.EndDict();
			
		return builder.Build().AsDict();
	}

	json::Dict RequestHandler::OutMap(const int id) {

		using namespace std::literals;

		json::Builder builder;

		builder.StartDict()
			.Key("request_id"s).Value(id);

		// получим готовую карту
		std::ostringstream os;
		renderer_.RenderMap(GetAllBuses()).Render(os);

		builder.Key("map"s).Value(os.str())
			.EndDict();

		return builder.Build().AsDict();
	}


}	// namespace transport_catalog