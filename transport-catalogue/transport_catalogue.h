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

#include "geo.h"

namespace transport_catalog {

    class TransportCatalogue {
    public:
        struct Stop {
            std::string name{};
            detail::Coordinates coordinates{};
        };

        struct Bus {
            std::string name{};
            std::vector<Stop*> stops{};
        };

        struct StopHasher {
            template <typename T>
            size_t operator()(const T* ptr) const {
                return hasher(ptr);
            }

            std::hash<const void*> hasher{};
        };

        struct StopsPairHasher {
            size_t operator()(const std::pair<Stop*, Stop*>& p_s) const {
                size_t h1 = hasher_(p_s.first);
                size_t h2 = hasher_(p_s.second);

                return h1 * 37 + h2;
            }
        private:
            //std::hash<const void*> hasher_{};
            StopHasher hasher_;
        };

        TransportCatalogue() = default;
        // добавляет остановку
        Stop* AddStop(const std::string& name, const double latitude, const double longitude);
        // ищет и возвращает остановку по имени
        Stop* FindStop(const std::string& name) const;
        // добавляет автобус с маршрутом
        Bus* AddBus(const std::string& name, const std::vector<std::string>& v_stops);
        // ищет и возвращает автобус по имени
        Bus* FindBus(const std::string& name) const;
        // возвращает маршрут поимени автобуса
        Bus* GetBusInfo(const std::string& bus_name) const;
        // возвращает информацию об остановке
        std::tuple<bool, std::set<std::string_view>*> GetStopInfo(const std::string& stop_name) const;
        // установить расстояние между остановками
        void SetDistanceBetweenStops(Stop* stop1, Stop* stop2, size_t distance);
        // получить расстояние между остановками
        size_t GetDistanceBetweenStops(Stop* stop1, Stop* stop2) const;

    private:
        std::deque<Stop> d_stops_{};
        std::deque<Bus> d_buses_{};
        // хеш-таблицы
        std::unordered_map<std::string_view, Stop*> um_stopname_to_stop_{};
        std::unordered_map<std::string_view, Bus*> um_busname_to_bus_{};
        std::unordered_map<std::string_view, std::set<std::string_view>> um_stops_info_{};
        std::unordered_map<std::pair<Stop*, Stop*>, size_t, StopsPairHasher> um_distance_{};

    };

} //transport_catalog