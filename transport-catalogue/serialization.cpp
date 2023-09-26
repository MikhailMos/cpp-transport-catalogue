#include "serialization.h"


void Serialization::SetFileName(const std::string& name) {
    file_name_ = name;
}

const std::string& Serialization::GetFileName() const {
    return file_name_;
}

// Serialization
void Serialization::Serialize(std::ofstream& output, const transport_catalog::TransportCatalogue& tc, const transport_catalog::renderer::MapRenderer& renderer, const transport_catalog::TransportRouter& transport_router) {

    transport_catalog_serialize::TransportCatalogue tc_serialized;
   
    // сериализуем остановки
    for (const auto& [name, stop_ptr] : tc.GetAllStops()) {
        transport_catalog_serialize::Stop stop_serialized = SerializeStop(stop_ptr);
        (*tc_serialized.mutable_stops())[reinterpret_cast<uint64_t>(stop_ptr)] = std::move(stop_serialized);
    }
    
    // сериализуем автобусы    
    for (const auto& [name, bus_ptr] : tc.GetAllBuses()) {
        transport_catalog_serialize::Bus bus_serialized = SerializeBus(bus_ptr);
        tc_serialized.mutable_bus()->Add(std::move(bus_serialized));
        //*tc_serialized.add_bus() = std::move(bus_serialized);
    }
    
    // сериализуем расстояния между остановками
    for (const auto& [pair_stops, distance] : tc.GetAllDistance()) {
        transport_catalog_serialize::Distance dist_serialiezed = SerializeDistance(pair_stops, distance);
        tc_serialized.mutable_distance()->Add(std::move(dist_serialiezed));
        //*tc_serialized.add_distance() = std::move(dist_serialiezed);
    }
    
    // сериализуем настройки карты
    *tc_serialized.mutable_map_settings() = std::move(SerializeMapSettings(renderer.GetSettings()));

    // сериализуем настройки роутера
    tc_serialized.mutable_router_settings()->set_bus_wait_time(transport_router.GetWaitTime());
    tc_serialized.mutable_router_settings()->set_bus_velocity(transport_router.GetVelocity());

    tc_serialized.SerializeToOstream(&output);
}

transport_catalog_serialize::Stop Serialization::SerializeStop(const transport_catalog::Stop* stop_ptr) {

    transport_catalog_serialize::Stop stop_serialized;

    stop_serialized.set_name(stop_ptr->name);
    stop_serialized.mutable_coordinates()->set_latitude(stop_ptr->coordinates.lat);
    stop_serialized.mutable_coordinates()->set_longitude(stop_ptr->coordinates.lng);

    return stop_serialized;
}

transport_catalog_serialize::Bus Serialization::SerializeBus(const transport_catalog::Bus* bus_ptr) {

    transport_catalog_serialize::Bus bus_serialized;

    bus_serialized.set_name(bus_ptr->name);
    bus_serialized.set_is_roundtrip(bus_ptr->is_roundtrip);
    for (const auto& stop : bus_ptr->stops) {
        bus_serialized.add_stop_id(reinterpret_cast<uint64_t>(stop));
    }

    return bus_serialized;
}

transport_catalog_serialize::Distance Serialization::SerializeDistance(const std::pair<transport_catalog::Stop*, transport_catalog::Stop*> p_stops, double distance) {

    transport_catalog_serialize::Distance dist_serialized;

    dist_serialized.set_stop_id_from(reinterpret_cast<uint64_t>(p_stops.first));
    dist_serialized.set_stop_id_to(reinterpret_cast<uint64_t>(p_stops.second));
    dist_serialized.set_distance(distance);

    return dist_serialized;
}

transport_catalog_serialize::MapSettings Serialization::SerializeMapSettings(const transport_catalog::renderer::MapSettings& settings) {

    transport_catalog_serialize::MapSettings serialized_settings;
    
    serialized_settings.set_width(settings.width);
    serialized_settings.set_height(settings.height);
    serialized_settings.set_padding(settings.padding);
    serialized_settings.set_line_width(settings.line_width);
    serialized_settings.set_stop_radius(settings.stop_radius);
    serialized_settings.set_bus_label_font_size(settings.bus_label_font_size);
    //point
    serialized_settings.mutable_bus_label_offset()->set_x(settings.bus_label_offset.x);
    serialized_settings.mutable_bus_label_offset()->set_y(settings.bus_label_offset.y);
    
    serialized_settings.set_stop_label_font_size(settings.stop_label_font_size);
    //point
    serialized_settings.mutable_stop_label_offset()->set_x(settings.stop_label_offset.x);
    serialized_settings.mutable_stop_label_offset()->set_y(settings.stop_label_offset.y);
    //color
    *serialized_settings.mutable_underlayer_color() = std::move(SerializeColor(settings.underlayer_color));

    serialized_settings.set_underlayer_width(settings.underlayer_width);

    for (const auto& color : settings.color_palette) {
        serialized_settings.mutable_color_palette()->Add(std::move(SerializeColor(color)));
        //*serialized_settings.add_color_palette() = std::move(SerializeColor(color));
    }

    return serialized_settings;
}

transport_catalog_serialize::Color Serialization::SerializeColor(const svg::Color& color) {

    transport_catalog_serialize::Color color_serialized;

    if (const auto ptr = std::get_if<std::string>(&color)) {
        color_serialized.set_no_color(false);
        color_serialized.set_color_name(*ptr);
    }
    else if (const auto ptr = std::get_if<svg::Rgb>(&color)) {
        color_serialized.set_no_color(false);
        
        transport_catalog_serialize::RGB* rgb = color_serialized.mutable_rgb();
        rgb->set_red(ptr->red);
        rgb->set_green(ptr->green);
        rgb->set_blue(ptr->blue);
    }
    else if (const auto ptr = std::get_if<svg::Rgba>(&color)) {
        color_serialized.set_no_color(false);
        
        transport_catalog_serialize::RGBA* rgba = color_serialized.mutable_rgba();
        rgba->set_red(ptr->red);
        rgba->set_green(ptr->green);
        rgba->set_blue(ptr->blue);
        rgba->set_opacity(ptr->opacity);
    }
    else {
        color_serialized.set_no_color(true);
    }

    return color_serialized;
}


// Deserialization
void Serialization::Deserialize(std::ifstream& input, transport_catalog::TransportCatalogue& tc, transport_catalog::renderer::MapRenderer& renderer, transport_catalog::TransportRouter& transport_router) {

    transport_catalog_serialize::TransportCatalogue tc_serialized;
    if (!tc_serialized.ParseFromIstream(&input)) {
        return;
    }

    // десериализуем остановки
    for (const auto& [stop_id, stop] : tc_serialized.stops()) {
        DeserializeStop(stop, tc);
    }
    
    // десериализуем автобусы
    for (const auto& bus_to_deserialaze : tc_serialized.bus()) {
        DeserializeBus(bus_to_deserialaze, tc_serialized, tc);
    }
    
    // десериализуем дистанции
    for (const auto& distance_to_deserialaze : tc_serialized.distance()) {

        tc.SetDistanceBetweenStops(tc.FindStop(GetStopName(tc_serialized, distance_to_deserialaze.stop_id_from())),
            tc.FindStop(GetStopName(tc_serialized, distance_to_deserialaze.stop_id_to())),
            distance_to_deserialaze.distance());
    }
    
    // десериализуем настройки MapRenderer
    DeserializeMapSettings(tc_serialized.map_settings(), renderer);

    // десериализуем настройки TransportRouter
    transport_router.SetWaitTime(tc_serialized.router_settings().bus_wait_time());
    transport_router.SetVelocity(tc_serialized.router_settings().bus_velocity());
}

void Serialization::DeserializeStop(const transport_catalog_serialize::Stop& stop, transport_catalog::TransportCatalogue& tc) {

    tc.AddStop(stop.name(), stop.coordinates().latitude(), stop.coordinates().longitude());
}

void Serialization::DeserializeBus(const transport_catalog_serialize::Bus& bus, const transport_catalog_serialize::TransportCatalogue& tc_serialized, transport_catalog::TransportCatalogue& tc) {

    std::vector<std::string> stops;

    for (const auto& stop_id : bus.stop_id()) {
        stops.emplace_back(GetStopName(tc_serialized, stop_id));
    }

    tc.AddBus(bus.name(), stops, bus.is_roundtrip());

}

std::string Serialization::GetStopName(const transport_catalog_serialize::TransportCatalogue& tc_serialized, const uint64_t stop_id) {
    return tc_serialized.stops().at(stop_id).name();
}

void Serialization::DeserializeMapSettings(const transport_catalog_serialize::MapSettings& map_settings, transport_catalog::renderer::MapRenderer& renderer) {

    transport_catalog::renderer::MapSettings map_settings_deserealized;

    map_settings_deserealized.width = map_settings.width();
    map_settings_deserealized.height = map_settings.height();
    map_settings_deserealized.padding = map_settings.padding();
    map_settings_deserealized.line_width = map_settings.line_width();
    map_settings_deserealized.stop_radius = map_settings.stop_radius();
    map_settings_deserealized.bus_label_font_size = map_settings.bus_label_font_size();
    map_settings_deserealized.bus_label_offset.x = map_settings.bus_label_offset().x();
    map_settings_deserealized.bus_label_offset.y = map_settings.bus_label_offset().y();
    map_settings_deserealized.stop_label_font_size = map_settings.stop_label_font_size();
    map_settings_deserealized.stop_label_offset.x = map_settings.stop_label_offset().x();
    map_settings_deserealized.stop_label_offset.y = map_settings.stop_label_offset().y();
    map_settings_deserealized.underlayer_color = std::move(DeserializeColor(map_settings.underlayer_color()));
    map_settings_deserealized.underlayer_width = map_settings.underlayer_width();
    // очистим цвета по умолчанию, если есть другие цвета
    if (map_settings.color_palette().size()) {
        map_settings_deserealized.color_palette.clear();
    }

    for (const auto& color_proto : map_settings.color_palette()) {
        map_settings_deserealized.color_palette.push_back(std::move(DeserializeColor(color_proto)));
    }

    renderer.SetSettings(map_settings_deserealized);

}

svg::Color Serialization::DeserializeColor(const transport_catalog_serialize::Color& color_proto) {

    if (color_proto.no_color()) {
        return {};
    }   
    else if (color_proto.has_rgb()) {
        svg::Rgb color;
        color.red = color_proto.rgb().red();
        color.green = color_proto.rgb().green();
        color.blue = color_proto.rgb().blue();
        
        return color;
    }   
    else if (color_proto.has_rgba()) {
        svg::Rgba color;
        color.red = color_proto.rgba().red();
        color.green = color_proto.rgba().green();
        color.blue = color_proto.rgba().blue();
        color.opacity = color_proto.rgba().opacity();

        return color;
    }
    else {
        return color_proto.color_name();
    }

}
