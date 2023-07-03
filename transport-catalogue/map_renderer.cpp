#include "map_renderer.h"


namespace transport_catalog {

    namespace renderer {

        void MapSettings::SetValue(const std::string& name, double value) {

            if (name == "width") {
                width = value;
            }
            else if (name == "height") {
                height = value;
            }
            else if (name == "padding") {
                padding = value;
            }
            else if (name == "line_width") {
                line_width = value;
            }
            else if (name == "stop_radius") {
                stop_radius = value;
            }
            else if (name == "underlayer_width") {
                underlayer_width = value;
            }
            else {
                throw std::out_of_range("Invalid property name: " + name);
            }

        }

        void MapSettings::SetValue(const std::string& name, size_t value) {
            
            if (name == "bus_label_font_size") {
                bus_label_font_size = value;
            }
            else if (name == "stop_label_font_size") {
                stop_label_font_size = value;
            }
            else {
                throw std::out_of_range("Invalid property name: " + name);
            }

        }

        void MapSettings::SetValue(const std::string& name, svg::Point value) {

            if (name == "bus_label_offset") {
                bus_label_offset = value;
            }
            else if (name == "stop_label_offset") {
                stop_label_offset = value;
            }
            else {
                throw std::out_of_range("Invalid property name: " + name);
            }

        }

        void MapSettings::SetValue(const std::string& name, svg::Color value) {

            if (name == "underlayer_color") {
                underlayer_color = value;
            }
            else {
                throw std::out_of_range("Invalid property name: " + name);
            }

        }

        void MapSettings::SetValue(const std::string& name, std::vector<svg::Color>& value) {

            if (name == "color_palette") {
                color_palette = value;
            }
            else {
                throw std::out_of_range("Invalid property name: " + name);
            }

        }

        bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }

        svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
            return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_
            };
        }

        MapRenderer::MapRenderer(MapSettings& new_settings)
            : map_settings_(new_settings)
        {}

        void MapRenderer::SetSettings(MapSettings& new_settings) {
            map_settings_ = new_settings;
        }

        const MapSettings& MapRenderer::GetSettings() const {
            return map_settings_;
        }

        double MapRenderer::GetWidth() const {
            return map_settings_.width;
        }

        double MapRenderer::GetHeight() const {
            return map_settings_.height;
        }

        double MapRenderer::GetPadding() const {
            return map_settings_.padding;
        }

        svg::Document MapRenderer::RenderMap(const std::unordered_map<std::string_view, Bus*>& un_buses) const {
            
            svg::Document doc_svg;

            std::vector<geo::Coordinates> geo_coords;
           
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

            const MapSettings& map_settings = GetSettings();

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
                Routes(doc_svg, proj, bus_ptr->stops, map_settings, index_color, bus_ptr->is_roundtrip);

                //---- слой второй, подложки и надписи ----//
                svg::Point screen_coordinates = proj(bus_ptr->stops.front()->coordinates);
                // подложка
                layer2.push_back(
                    AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, true)
                );
                // надпись
                layer2.push_back(
                    AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, false)
                );

                if (!bus_ptr->is_roundtrip && bus_ptr->stops.front() != bus_ptr->stops.back()) {
                    screen_coordinates = proj(bus_ptr->stops.back()->coordinates);
                    // подложка
                    layer2.push_back(
                        AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, true)
                    );
                    // надпись
                    layer2.push_back(
                        AddRouteName(map_settings, "Verdana", "bold", screen_coordinates, name, index_color, false)
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
            PointStops(doc_svg, proj, v_stops, map_settings);

            return doc_svg;
        }

        void MapRenderer::Routes(svg::Document& doc, const SphereProjector& proj, const std::vector<Stop*>& v_stops, const MapSettings& settings, const size_t color_number, const bool is_roundtrip) const {
            
            svg::Polyline line_of_bus;

            svg::Color stroke = settings.color_palette[color_number];

            // зададим точки, координаты
            for (const Stop* stop_ptr : v_stops) {

                svg::Point screen_coord = proj(stop_ptr->coordinates);
                line_of_bus.AddPoint(screen_coord);

            }

            // для не кольцевого маршрута нарисуем еще и в обратном направлении(пропускаем текущую точку)
            if (!is_roundtrip) {
                for (size_t i = v_stops.size() - 1; i > 0; --i) {
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

        void MapRenderer::PointStops(svg::Document& doc, const renderer::SphereProjector& proj, std::vector<Stop*>& v_stops, const renderer::MapSettings& settings) const {

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

        svg::Text MapRenderer::AddRouteName(const renderer::MapSettings& settings, const std::string& font_family, const std::string& font_wight, const svg::Point screen_coord, const std::string_view& name, const size_t index_color, const bool is_substrate) const {

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

        svg::Text MapRenderer::AddStopName(const renderer::MapSettings& settings, const std::string& font_family, const svg::Point screen_coord, const std::string_view& name, const bool is_substrate) const {

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

}   //namespace renderer

} // namespace transport_catalog