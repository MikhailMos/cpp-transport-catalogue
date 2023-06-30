#include "transport_catalogue.h"

#include <utility>
#include <unordered_set>

namespace transport_catalog {

    Stop* TransportCatalogue::AddStop(const std::string& name, const double latitude, const double longitude) {

        geo::Coordinates coord{ latitude, longitude };
        Stop stop{ name, std::move(coord) };
        auto& el = d_stops_.emplace_back(std::move(stop));
        um_stopname_to_stop_.emplace(el.name, &el);
        // добавляем. если только такой остановки нет
        if (um_stops_info_.count(el.name) == 0) {
            um_stops_info_.emplace(el.name, std::set<std::string_view>());
        }

        return &el;

    }

    Stop* TransportCatalogue::FindStop(const std::string& name) const {
        const auto it = um_stopname_to_stop_.find(name);
        if (it == um_stopname_to_stop_.cend()) {
            return {};
        }

        return it->second;
    }

    Bus* TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& v_stops, const bool is_roundtrip) {

        std::vector<Stop*> v_ptr_stops;

        for (const std::string& stop_name : v_stops) {
            auto& curr_stop = um_stopname_to_stop_.at(stop_name);
            v_ptr_stops.push_back(curr_stop);
        }

        Bus bus{ name, std::move(v_ptr_stops), is_roundtrip };
        auto& el = d_buses_.emplace_back(std::move(bus));
        um_busname_to_bus_.emplace(el.name, &el);

        // добавим в остановки автобусы
        for (const auto& stop : el.stops) {
            um_stops_info_.at(stop->name).insert(el.name);
        }

        return &el;

    }

    Bus* TransportCatalogue::FindBus(const std::string_view& name) const {
        const auto it = um_busname_to_bus_.find(name);
        if (it == um_busname_to_bus_.cend()) {
            return {};
        }

        return it->second;
    }

    Bus* TransportCatalogue::GetBusInfo(const std::string_view& bus_name) const {

        return FindBus(bus_name);

    }

    std::tuple<bool, std::set<std::string_view>*> TransportCatalogue::GetStopInfo(const std::string_view& stop_name) const {

        const auto it = um_stops_info_.find(stop_name);
        if (it == um_stops_info_.cend()) {
            return std::make_tuple(false, nullptr);
        }

        const std::set<std::string_view>* ptr_buses = &it->second;

        return std::make_tuple(true, const_cast<std::set<std::string_view>*>(ptr_buses));

    }

    void TransportCatalogue::SetDistanceBetweenStops(Stop* stop1, Stop* stop2, double distance) {
        um_distance_.emplace(std::make_pair(stop1, stop2), distance);
    }

    double TransportCatalogue::GetDistanceBetweenStops(Stop* stop1, Stop* stop2) const {
        double result = 0;

        auto it = um_distance_.find(std::make_pair(stop1, stop2));
        if (it == um_distance_.end()) {
            it = um_distance_.find(std::make_pair(stop2, stop1));
        }

        if (it != um_distance_.end()) {
            result += it->second;
        }

        return result;
    }

    const std::unordered_map<std::string_view, Bus*>& TransportCatalogue::GetAllBuses() const {
        return um_busname_to_bus_;
    }

} //transport_catalog