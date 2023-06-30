#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


namespace transport_catalog {
    
    struct BusStat {
        BusStat(std::string_view sv_name, double geo_length, double length, size_t count, size_t unique_count);

        std::string_view& name;
        double geo_route_length;
        double route_length;        
        std::uint8_t stop_count, unique_stop_count;
        double curvature;
    };

    class RequestHandler {
    public:
        RequestHandler() = default;
        RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer)
            : db_(db)
            , renderer_(renderer)
        {}
        
        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;
        // Возвращает маршруты, проходящие через остановку
        std::optional<std::set<std::string_view>*> GetBusesByStop(const std::string_view& stop_name) const;
                
        svg::Document RenderMap() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const TransportCatalogue& db_{};
        const renderer::MapRenderer& renderer_{};

        // возвращает кол-во уникальных остановок
        size_t GetUniqueStops(const std::vector<Stop*>& v_stops) const;
        // возвращает географическую длину маршрута
        double GetGeoLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const;
        // возвращает фактическую длину маршрута
        double GetLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const;

    };

    namespace map_objects {
        
        void Routes(svg::Document& doc, const renderer::SphereProjector& proj, const std::vector<Stop*>& v_stops, const renderer::MapSettings& settings, const size_t color_number, const bool is_roundtrip);
        void PointStops(svg::Document& doc, const renderer::SphereProjector& proj, const std::vector<Stop*>& v_stops, const renderer::MapSettings& settings);
        svg::Text AddRouteName(const renderer::MapSettings& settings, const std::string& font_family, const std::string& font_wight, const svg::Point screen_coord, const std::string_view& name, const size_t index_color, const bool is_substrate);
        svg::Text AddStopName(const renderer::MapSettings& settings, const std::string& font_family, const svg::Point screen_coord, const std::string_view& name, const bool is_substrate);

    }   // namespace map_objects
    

}   // namespace transport_catalog