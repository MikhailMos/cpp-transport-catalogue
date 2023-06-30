#pragma once

#include <vector>
#include <string>

#include "geo.h"

namespace transport_catalog {

    struct Stop {
        std::string name{};
        geo::Coordinates coordinates{};
    };

    struct Bus {
        std::string name{};
        std::vector<Stop*> stops{};
        bool is_roundtrip{ false };
    };

    struct StopHasher {
        template <typename T>
        size_t operator()(const T* ptr) const {
            return hasher(ptr);
        }

        std::hash<const void*> hasher{};
    };

    struct StopsPairHasher {
        size_t operator()(const std::pair<Stop*, Stop*>& p_s) const;
    private:
        StopHasher hasher_;
    };


} // transport_catalog