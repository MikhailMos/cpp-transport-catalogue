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
    
    json::Document doc = ReadFromInput(std::cin, tc, map_renderer, rh);    
    json::Print(doc, std::cout);

    // чтение из файла и вывод в файл
    //std::ifstream s_in("s10_final_opentest_3.json");
    //json::Document doc = ReadFromInput(s_in, tc, map_renderer, rh);

    //// просто отрисуем карту
    ////svg::Document my_map = rh.RenderMap();
    //////my_map.Render(std::cout);

    //std::ofstream fout;
    //fout.open("my_result.txt", std::ios::app);
    ////my_map.Render(fout);
    //json::Print(doc, fout);
    //
    //fout.close();
    
    return 0;
}