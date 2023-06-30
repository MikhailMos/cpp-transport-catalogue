#include "json_reader.h"

namespace transport_catalog {
    
    json::Dict GetNotFoundNode(const int id) {
        json::Dict result;
        result.insert({ std::move("request_id"), json::Node(id) });

        result.insert({ std::move("error_message"), json::Node(std::string{"not found"}) });

        return result;
    }
    
    //------------ работа с базой ------------//
    
    json::Document ReadFromInput(std::istream& input, TransportCatalogue& tc, renderer::MapRenderer& map_renderer, RequestHandler& rh) {
                
        json::Document doc = json::Load(input);
        
        const json::Node& node = doc.GetRoot();

        if (!node.IsMap()) { throw std::invalid_argument("Wrong into file structure"); }

        for (const auto& [key, val] : node.AsMap()) {
            
            if (val.IsArray()) {

                const json::Array& arr_nodes = val.AsArray();

                //массив с описанием автобусных маршрутов и остановок
                if (key == "base_requests") {
                    RequestHandlerToBase(arr_nodes, tc);
                }
                //массив с запросами к транспортному справочнику
                else if (key == "stat_requests") {                                        
                    json::Array arr = RequestHandlerToTC(arr_nodes, rh);
                    return json::Document{ arr };
                }
            }
            else if (val.IsMap()) {
                
                const json::Dict& dict_node = val.AsMap();
                // словарь настроек визуализации карты
                if (key == "render_settings") {                    
                    RequestHandlerToMap(dict_node, map_renderer);
                }

            }
            
        }

        return json::Document({});
        
    }

    void RequestHandlerToBase(const json::Array& arr_nodes, TransportCatalogue& tc) {

       // первым проходом обрабатываем все остановки
        for (int i = 0; i < 2; ++i) {

            std::unordered_map<Stop*, json::Dict> um_distance_to_stops;

            for (const auto& node : arr_nodes) {
                if (!node.IsMap()) { throw std::invalid_argument("Wrong into file structure"); }

                const json::Dict& map_value = node.AsMap();
                if (map_value.count("type") == 0) { throw std::invalid_argument("Wrong into file structure"); }

                const std::string& type = map_value.at("type").AsString();

                // обработаем остановки
                if (i == 0 && type == "Stop") {
                    Stop* stop = CreateStop(map_value, tc);
                    // сохраним дорожное расстояние от этой остановки до соседних.
                    if (map_value.count("road_distances")) {
                        um_distance_to_stops[stop] = map_value.at("road_distances").AsMap();
                    }
                }

                // обработаем автобусы
                if (i == 1 && type == "Bus") {
                    CreateBus(map_value, tc);
                }

            }

            if (um_distance_to_stops.size() > 0) {
                AddDistanceForStops(um_distance_to_stops, tc);
            }

        }

    }

    Stop* CreateStop(const json::Dict map_stop, TransportCatalogue& tc) {
        return tc.AddStop(map_stop.at("name").AsString(), map_stop.at("latitude").AsDouble(), map_stop.at("longitude").AsDouble());
    }

    void AddDistanceForStops(const std::unordered_map<transport_catalog::Stop*, json::Dict>& un_distance, transport_catalog::TransportCatalogue& tc) {

        for (const auto& [stop1, map_dist] : un_distance) {

            for (const auto& [stop_name, node_dist] : map_dist) {
                
                Stop* stop2 = tc.FindStop(stop_name);
                if (stop2 != nullptr) {
                    tc.SetDistanceBetweenStops(stop1, stop2, node_dist.AsInt());
                }

            }
       
        }

    }

    void CreateBus(const json::Dict map_bus, transport_catalog::TransportCatalogue& tc) {
        
        const json::Array& stops = map_bus.at("stops").AsArray();
        std::vector<std::string> v_stops;
        v_stops.reserve(stops.size());

        for (const json::Node& n_stop : stops) {
            v_stops.push_back(n_stop.AsString());
        }
        
        tc.AddBus(map_bus.at("name").AsString(), v_stops, map_bus.at("is_roundtrip").AsBool());

    }
    
    //------------ получение информации из базы ------------//

    json::Array RequestHandlerToTC(const json::Array& arr_nodes, RequestHandler& rh) {

        json::Array out_arr_nodes;

        for (const json::Node& node : arr_nodes) {
            if (!node.IsMap()) { throw std::invalid_argument("Wrong into file structure"); }

            const json::Dict& map_value = node.AsMap();
            
            if (map_value.count("type") == 0) { throw std::invalid_argument("Wrong into file structure"); }
            const std::string& type = map_value.at("type").AsString();

            if (map_value.count("id") == 0) { throw std::invalid_argument("Wrong into file structure"); }
            const int request_id = map_value.at("id").AsInt();

            std::string name;
            if (map_value.count("name")) {
                name = map_value.at("name").AsString();
            }
                                    
            json::Dict dict;

            if (type == "Stop") {
                std::optional<std::set<std::string_view>*> stop_info = rh.GetBusesByStop(name);
                if (stop_info) {
                    dict = OutStopInfo(request_id, stop_info);
                }
            }
            else if (type == "Bus") {
                
                std::optional<BusStat> bus_info = rh.GetBusStat(name);
                if (bus_info) {
                     dict = OutBusInfo(request_id, bus_info);
                }
                                                
            }
            else if (type == "Map") {
                dict = OutMap(request_id, rh);
            }
            /*else {
                throw std::invalid_argument("Wrong into file structure");
            }*/

            if (dict.size() == 0) {
                dict = GetNotFoundNode(map_value.at("id").AsInt());
            }

            out_arr_nodes.push_back(dict);
        }
        
        return out_arr_nodes;
        
    }
    
    json::Dict OutBusInfo(const int id, const std::optional<BusStat>& bus_info) {
       
        json::Dict result;
                    
        result.insert({ std::move("request_id"), json::Node(id) });
        result.insert({ std::move("curvature"), json::Node(bus_info->curvature) });
        result.insert({ std::move("route_length"), json::Node(bus_info->route_length)});
        result.insert({ std::move("stop_count"), json::Node(bus_info->stop_count)});
        result.insert({ std::move("unique_stop_count"), json::Node(bus_info->unique_stop_count) });

        return result;
       
    }

    json::Dict OutStopInfo(const int id, const std::optional<std::set<std::string_view>*>& stop_info) {
        
        json::Dict result;
        result.insert({ std::move("request_id"), json::Node(id) });
        

        json::Array arr_buses;

        for (const auto& el : *stop_info.value()) {
            arr_buses.push_back(std::move(json::Node(static_cast<std::string>(el))));
        }

        result.insert({ std::move("buses"), arr_buses});

        return result;
    }

    void RequestHandlerToMap(const json::Dict& dict_node, renderer::MapRenderer& map_renderer) {

        renderer::MapSettings settings;
        // распарсим и заполним настройки
        for (const auto& [key, val] : dict_node) {

            if ((key == "bus_label_font_size" || key == "stop_label_font_size") && val.IsInt()) {
                settings.SetValue(key, static_cast<size_t>(val.AsInt()));
            }
            else if (val.IsDouble()) {
                settings.SetValue(key, val.AsDouble());
            }
            else if (val.IsString()) {
                settings.SetValue(key, val.AsString());
            }
            else if (val.IsArray()) {
                const json::Array& arr = val.AsArray();

                if (key == "bus_label_offset" || key == "stop_label_offset") { //svg::Point
                    settings.SetValue(key, svg::Point(arr[0].AsDouble(), arr[1].AsDouble()));
                }
                else if (key == "underlayer_color") { //svg::Color
                    if (arr.size() == 3) {
                        settings.SetValue(key, svg::Rgb(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt()));
                    }
                    else {
                        settings.SetValue(key, svg::Rgba(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble()));
                    }
                }
                else if (key == "color_palette") {  //std::vector<svg::Color> 
                    std::vector<svg::Color> v_colors;
                    v_colors.reserve(arr.size());

                    for (const json::Node& elem : arr) {
                        if (elem.IsString()) {
                            v_colors.push_back(elem.AsString());
                        }
                        else if (elem.IsArray()) {
                            const json::Array& arr_color = elem.AsArray();
                            if (arr_color.size() == 3) {
                                v_colors.push_back(svg::Rgb(arr_color[0].AsInt(), arr_color[1].AsInt(), arr_color[2].AsInt()));
                            }
                            else {
                                v_colors.push_back(svg::Rgba(arr_color[0].AsInt(), arr_color[1].AsInt(), arr_color[2].AsInt(), arr_color[3].AsDouble()));
                            }
                        }
                    }

                    settings.SetValue(key, v_colors);
                }

            }

        }

        map_renderer.SetSettings(settings);
    }

    json::Dict OutMap(const int id, RequestHandler& rh) {

        json::Dict result;
        result.insert({ std::move("request_id"), json::Node(id) });

        // получим готовую карту
        std::ostringstream os;
        rh.RenderMap().Render(os);

        result.insert({ "map", os.str() });

        return result;
    }

    

}	// namespace transport_catalog