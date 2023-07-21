#include <string>
#include <sstream>
#include <fstream>

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"


int main() {
    
    using namespace transport_catalog;

    TransportCatalogue tc;
    renderer::MapRenderer map_renderer;
    RequestHandler rh{ tc, map_renderer };
        
    json::Document doc = rh.ReadAndExecuteRequests(std::cin);
    json::Print(doc, std::cout);

    //// чтение из файла и вывод в файл
    //std::ifstream s_in("s10_final_opentest_3.json");
    //json::Document doc = rh.ReadAndExecuteRequests(s_in);
    //json::Print(doc, std::cout);

    //// просто отрисуем карту
    ////svg::Document my_map = map_renderer.RenderMap(rh.GetAllBuses());
    ////json::Print(my_map, std::cout);

    /*std::ofstream fout;
    fout.open("my_result.txt", std::ios::app);    
    json::Print(doc, fout);
    
    fout.close();*/
    
    return 0;
}