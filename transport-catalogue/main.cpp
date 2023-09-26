
#include <fstream>
#include <iostream>
#include <string_view>

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    using namespace transport_catalog;

    const std::string_view mode(argv[1]);

    TransportCatalogue tc;
    TransportRouter transport_router;
    renderer::MapRenderer map_renderer;
    Serialization serializetion;
    RequestHandler rh{ tc, map_renderer, transport_router, serializetion };

    if (mode == "make_base"sv) {

        rh.MakeBaseRequests(std::cin);

        std::ofstream outfile(serializetion.GetFileName(), std::ios::binary);
        if (outfile.is_open()) {
            serializetion.Serialize(outfile, tc, map_renderer, transport_router);
        }

        outfile.close();

    }
    else if (mode == "process_requests"sv) {
        
        json::Document doc = rh.ProcessRequests(std::cin);
        json::Print(doc, std::cout);

    }
    else {
        PrintUsage();
        return 1;
    }

    system("pause");

}