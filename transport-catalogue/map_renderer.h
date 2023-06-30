#pragma once

#include "svg.h"
#include "geo.h"

#include <algorithm>

namespace transport_catalog {
    
    namespace renderer {

        // настройки карты
        struct MapSettings {
            
            double width{ 1200.0 }; //ширина изображения в пикселях. Вещественное число в диапазоне от 0 до 100000.
            double height{ 1200.0 };    //высота изображения в пикселях.Вещественное число в диапазоне от 0 до 100000.
            double padding{ 50.0 }; //отступ краёв карты от границ SVG-документа. Вещественное число не меньше 0 и меньше min(width, height)/2.
            double line_width{ 14.0 }; //толщина линий, которыми рисуются автобусные маршруты. Вещественное число в диапазоне от 0 до 100000.
            double stop_radius{ 5.0 }; //радиус окружностей, которыми обозначаются остановки. Вещественное число в диапазоне от 0 до 100000.
            size_t bus_label_font_size{ 20 }; //размер текста, которым написаны названия автобусных маршрутов. Целое число в диапазоне от 0 до 100000.
            svg::Point bus_label_offset{ 7.0, 15.0 }; //смещение надписи с названием маршрута относительно координат конечной остановки на карте. Задаёт значения свойств dx и dy SVG-элемента <text>.
            size_t stop_label_font_size{ 20 }; //размер текста, которым отображаются названия остановок. Целое число в диапазоне от 0 до 100000.
            svg::Point stop_label_offset{ 7.0, -3.0 }; //смещение названия остановки относительно её координат на карте. Задаёт значения свойств dx и dy SVG-элемента <text>.
            svg::Color underlayer_color{ svg::Rgba(255, 255, 255, 0.85) }; //цвет подложки под названиями остановок и маршрутов.
            double underlayer_width{ 3.0 }; //толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута stroke-width элемента <text>.
            std::vector<svg::Color> color_palette{ "green",  svg::Rgb(255, 160, 0), "red" }; //цветовая палитра.

            void SetValue(const std::string& name, double value);
            void SetValue(const std::string& name, size_t value);
            void SetValue(const std::string& name, svg::Point value);
            void SetValue(const std::string& name, svg::Color value);
            void SetValue(const std::string& name, std::vector<svg::Color>& value);
               
        };


        inline const double EPSILON = 1e-6;
        bool IsZero(double value);

        class SphereProjector {
        public:
            // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                double max_width, double max_height, double padding);

            // Проецирует широту и долготу в координаты внутри SVG-изображения
            svg::Point operator()(geo::Coordinates coords) const;

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        class MapRenderer {
        public:
            MapRenderer() = default;
            MapRenderer (MapSettings& new_settings);
            ~MapRenderer () = default;

            void SetSettings(MapSettings& new_settings);
            const MapSettings& GetSettings() const;
            double GetWidth() const;
            double GetHeight() const;
            double GetPadding() const;
        private:
            MapSettings map_settings_;
        };

        //----- шаблонные функции -----//

        template<typename PointInputIt>
        inline SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding)            
                : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end) {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
            

        }

    }	// namespace renderer 

}   // namespace transport_catalog
