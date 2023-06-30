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

	svg::Document RequestHandler::RenderMap() const {
		
		svg::Document doc_svg;

		std::vector<geo::Coordinates> geo_coords;
		const std::unordered_map<std::string_view, Bus*>& un_buses = db_.GetAllBuses();
		
		// отберем только маршруты у которых есть остановки
		std::map<std::string_view, Bus*> ordered_buses;
		std::copy_if(un_buses.cbegin(), un_buses.cend(),
					std::inserter(ordered_buses, ordered_buses.begin()),
					[](const auto& el) { return !el.second->stops.empty(); });
		
		// Соберем все координаты по непустым маршрутам
		for (const auto& [bus_name, bus_ptr] : ordered_buses) {
			for (const Stop* stop_ptr : bus_ptr->stops) {
				geo_coords.push_back(stop_ptr->coordinates);
			}
		}

		const renderer::MapSettings& map_settings = renderer_.GetSettings();

		// Создаём проектор сферических координат на карту
		const renderer::SphereProjector proj{
			geo_coords.begin(), geo_coords.end(), map_settings.width, map_settings.height, map_settings.padding
		};

		size_t index_color{ 0 };
		const size_t color_max_number = map_settings.color_palette.size() - 1;
		
		std::vector<svg::Text> layer2; // содержит надписи и подложки		
		std::set<Stop*> s_stops; // содержит уникальные остановки для отрисовки третьего и четвертого слоев
		
		for (const auto& [name, bus_ptr] : ordered_buses) {

			// определим цвет маршрута
			if (index_color > color_max_number) { index_color = 0; }
			
			//---- слой первый, маршруты. Добавляется сразу ----//
			map_objects::Routes(doc_svg, proj, bus_ptr->stops, map_settings, index_color, bus_ptr->is_roundtrip);
			
			//---- слой второй, подложки и надписи ----//
			svg::Point screen_coordinates = proj(bus_ptr->stops.front()->coordinates);
			// подложка
			layer2.push_back(
				map_objects::AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, true)
			);
			// надпись
			layer2.push_back(
				map_objects::AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, false)
			);

			if (!bus_ptr->is_roundtrip && bus_ptr->stops.front() != bus_ptr->stops.back()) {
				screen_coordinates = proj(bus_ptr->stops.back()->coordinates);
				// подложка
				layer2.push_back(
					map_objects::AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, true)
				);
				// надпись
				layer2.push_back(
					map_objects::AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, false)
				);
			}

			s_stops.insert(bus_ptr->stops.begin(), bus_ptr->stops.end());

			++index_color;
		}

		// добавим второй слой на карту
		for (svg::Text& t : layer2) {
			doc_svg.Add(t);
		}

		// переложим в вектор, чтобы можно было отсортировать
		std::vector<Stop*> v_stops(s_stops.begin(), s_stops.end()); 
		
		std::sort(v_stops.begin(), v_stops.end(), [](const Stop* lhs, const Stop* rhs) { return lhs->name < rhs->name; });
		// добавим третий и четвертый слои
		map_objects::PointStops(doc_svg, proj, v_stops, map_settings);
		
		return doc_svg;
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

	namespace map_objects {

		void Routes(svg::Document& doc, const renderer::SphereProjector& proj, const std::vector<Stop*>& v_stops, const renderer::MapSettings& settings, const size_t color_number, const bool is_roundtrip) {
			svg::Polyline line_of_bus;

			svg::Color stroke = settings.color_palette[color_number];

			// зададим точки, координаты
			for (const Stop* stop_ptr : v_stops) {

				svg::Point screen_coord = proj(stop_ptr->coordinates);
				line_of_bus.AddPoint(screen_coord);

			}

			// для не кольцевого маршрута нарисуем еще и в обратном направлении(пропускаем текущую точку)
			if (!is_roundtrip) {
				for (size_t i = v_stops.size()-1; i > 0; --i) {
					svg::Point screen_coord = proj(v_stops[(i - 1)]->coordinates);
					line_of_bus.AddPoint(screen_coord);
				}

			}

			doc.Add(line_of_bus
				.SetStrokeColor(stroke)
				.SetFillColor(svg::NoneColor)
				.SetStrokeWidth(settings.line_width)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
			);
		}

		void PointStops(svg::Document& doc, const renderer::SphereProjector& proj, const std::vector<Stop*>& v_stops, const renderer::MapSettings& settings) {

			std::vector<svg::Text> v_stop_names;
			v_stop_names.reserve(v_stops.size());

			for (const Stop* stop_ptr : v_stops) {
				svg::Point screen_coordinates = proj(stop_ptr->coordinates);
				
				svg::Circle circle;
				doc.Add(circle
							.SetCenter(screen_coordinates)
							.SetRadius(settings.stop_radius)
							.SetFillColor(svg::Color("white"))
				
				);

				// сохраним подложки и имена
				v_stop_names.push_back(AddStopName(settings, "Verdana", screen_coordinates, stop_ptr->name, true));
				v_stop_names.push_back(AddStopName(settings, "Verdana", screen_coordinates, stop_ptr->name, false));
			}

			// добавим названия остановок
			for (svg::Text t : v_stop_names) {
				doc.Add(t);
			}

		}
		
		svg::Text AddRouteName(const renderer::MapSettings& settings, const std::string& font_family, const std::string& font_wight, const svg::Point screen_coord, const std::string_view& name, const size_t index_color, const bool is_substrate) {
			
			svg::Text text;
			text.SetPosition(screen_coord)
				.SetOffset(settings.bus_label_offset)
				.SetFontSize(settings.bus_label_font_size)
				.SetFontFamily(font_family)
				.SetFontWeight(font_wight)
				.SetData(static_cast<std::string>(name))
				.SetFillColor(settings.color_palette[index_color]);

			if (is_substrate) {
				text.SetFillColor(settings.underlayer_color)
					.SetStrokeColor(settings.underlayer_color)
					.SetStrokeWidth(settings.underlayer_width)
					.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			}
						
			return text;

		}

		svg::Text AddStopName(const renderer::MapSettings& settings, const std::string& font_family, const svg::Point screen_coord, const std::string_view& name, const bool is_substrate) {
			
			svg::Text text;
			text.SetPosition(screen_coord)
				.SetOffset(settings.stop_label_offset)
				.SetFontSize(settings.stop_label_font_size)
				.SetFontFamily(font_family)
				.SetData(static_cast<std::string>(name))
				.SetFillColor(svg::Color("black"));
			
			if (is_substrate) {
				text.SetFillColor(settings.underlayer_color)
					.SetStrokeColor(settings.underlayer_color)
					.SetStrokeWidth(settings.underlayer_width)
					.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			}

			return text;
		}


	} // namespace map_objects


}	// namespace transport_catalog