#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

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

}   //namespace renderer

} // namespace transport_catalog