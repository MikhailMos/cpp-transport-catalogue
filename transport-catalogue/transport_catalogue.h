#pragma once

#include <unordered_map>
#include <deque>
#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <functional>
#include <type_traits>

#include "domain.h"

namespace transport_catalog {

    class TransportCatalogue {
    public:       
        TransportCatalogue() = default;
        // добавляет остановку
        Stop* AddStop(const std::string& name, const double latitude, const double longitude);
        // ищет и возвращает остановку по имени
        Stop* FindStop(const std::string& name) const;
        // добавляет автобус с маршрутом
        Bus* AddBus(const std::string& name, const std::vector<std::string>& v_stops, const bool is_roundtrip);
        // ищет и возвращает автобус по имени
        Bus* FindBus(const std::string_view& name) const;
        // возвращает маршрут поимени автобуса
        Bus* GetBusInfo(const std::string_view& bus_name) const;
        // возвращает информацию об остановке
        std::tuple<bool, std::set<std::string_view>*> GetStopInfo(const std::string_view& stop_name) const;
        // установить расстояние между остановками
        void SetDistanceBetweenStops(Stop* stop1, Stop* stop2, double distance);
        // получить расстояние между остановками
        double GetDistanceBetweenStops(Stop* stop1, Stop* stop2) const;
        // получает все маршруты хранящиеся в базе
        const std::unordered_map<std::string_view, Bus*>& GetAllBuses() const;

    private:
        std::deque<Stop> d_stops_{}; // содержит все остановки
        std::deque<Bus> d_buses_{}; // содержит все автобусы
        // хеш-таблицы
        std::unordered_map<std::string_view, Stop*> um_stopname_to_stop_{}; // содержит имя остановки (представление строки) и указатель на остановку
        std::unordered_map<std::string_view, Bus*> um_busname_to_bus_{};  // содержит имя автобуса (представление строки) и указатель на автобус
        std::unordered_map<std::string_view, std::set<std::string_view>> um_stops_info_{}; // хранит все маршруты проходящие, через остановку
        std::unordered_map<std::pair<Stop*, Stop*>, double, StopsPairHasher> um_distance_{}; // хранит расстояние между остановками

    };

} //transport_catalog