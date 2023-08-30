#include "json_reader.h"

namespace transport_catalog {
    

    Requests JSONReader::Read(std::istream& input) {
        
        Requests requests;
        json::Document doc = json::Load(input);

        const json::Node& node = doc.GetRoot();

        if (!node.IsDict()) { throw std::invalid_argument("Wrong into file structure"); }

        for (const auto& [key, val] : node.AsDict()) {

            if (val.IsArray()) {

                const json::Array& arr_nodes = val.AsArray();

                //массив с описанием автобусных маршрутов и остановок
                if (key == "base_requests") {                    
                    requests.base_requests = arr_nodes;
                }
                //массив с запросами к транспортному справочнику
                else if (key == "stat_requests") {                  
                    requests.stat_requests = arr_nodes;
                }
            }
            else if (val.IsDict()) {

                const json::Dict& dict_node = val.AsDict();
                // словарь настроек визуализации карты
                if (key == "render_settings") {                    
                    requests.render_settings = dict_node;
                }
                // словарь настроек маршрутизации
                else if (key == "routing_settings") {
                    requests.routing_settings = dict_node;
                }

            }

        }
        
        return requests;
    }


}	// namespace transport_catalog