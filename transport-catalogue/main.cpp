#include <string>
#include <sstream>

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_router.h"


int main() {
    
    using namespace transport_catalog;

    TransportCatalogue tc;
    TransportRouter transport_router;
    renderer::MapRenderer map_renderer;
    RequestHandler rh{ tc, map_renderer, transport_router };
        
    json::Document doc = rh.ReadAndExecuteRequests(std::cin);
    json::Print(doc, std::cout);

    return 0;
}