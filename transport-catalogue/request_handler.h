#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <string_view>
#include <algorithm>
#include <fstream>

#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"
#include "json_reader.h"
#include "json_builder.h"
#include "transport_router.h"
#include "serialization.h"


namespace transport_catalog {
    
    struct BusStat {
        BusStat(std::string_view sv_name, double geo_length, double length, size_t count, size_t unique_count);

        std::string_view name;
        double geo_route_length;
        double route_length;        
        std::uint16_t stop_count, unique_stop_count;
        double curvature;
    };

    class RequestHandler {
    public:
        RequestHandler() = default;
        RequestHandler(TransportCatalogue& db, renderer::MapRenderer& renderer, TransportRouter& transport_router, Serialization& serializtion)
            : db_(db)
            , renderer_(renderer)
            , transport_router_(transport_router)
            , serialization_(serializtion)
        {}
        
        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;
        // Возвращает маршруты, проходящие через остановку
        std::optional<std::set<std::string_view>*> GetBusesByStop(const std::string_view& stop_name) const;
        // получает все маршруты хранящиеся в базе
        const std::unordered_map<std::string_view, Bus*>& GetAllBuses() const;

        // заполняет транспортный каталог данными и устанавливает настройки
        void MakeBaseRequests(std::istream& input);
        // возвращает ответы на запросы к транспортному каталогу
        json::Document ProcessRequests(std::istream& input);
        // обрабатывает запросы добавления в транспортный справочник
        void ToBase(const json::Array& arr_nodes);
        // обрабатывает запросы к транспортному справочнику и выводит готовый результат
        json::Array ToTransportCataloque(const json::Array& arr_nodes);
        // обрабатывает настройки карты и передает их в модуль map_renderer
        void SettingsForMap(const json::Dict& dict_node);
        // обрабатыает настройки маршрутизации
        void SetRoutingSettings(const json::Dict& dict_node);
        // обрабатыает настройки сериализации
        void SetSerialization(const json::Dict& dict_node);

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник", "Визуализатор Карты", "Маршрутизация транспорта"
        TransportCatalogue& db_;
        renderer::MapRenderer& renderer_;
        TransportRouter& transport_router_;
        Serialization& serialization_;

        // возвращает кол-во уникальных остановок
        size_t GetUniqueStops(const std::vector<Stop*>& v_stops) const;
        // возвращает географическую длину маршрута
        double GetGeoLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const;
        // возвращает фактическую длину маршрута
        double GetLengthOfDistance(const std::vector<Stop*>& v_stops, const bool is_roundtrip) const;

        // ф-и для ввода информаци
        Stop* CreateStop(const json::Dict map_stop);
        void AddDistanceForStops(const std::unordered_map<transport_catalog::Stop*, json::Dict>& un_distance);
        Bus* CreateBus(const json::Dict map_bus);
        
        // ф-и для вывода информации
        json::Dict GetNotFoundNode(const int id);
        json::Dict OutBusInfo(const int id, const std::optional<BusStat>& bus_info);
        json::Dict OutStopInfo(const int id, const std::optional<std::set<std::string_view>*>& stop_info);
        json::Dict OutMap(const int id);
        json::Dict OutRoutInfo(const int id, const std::string& stop_from, const std::string& stop_to);

        // заполняет данные из файла
        void FillOutFromFile();
    };


}   // namespace transport_catalog