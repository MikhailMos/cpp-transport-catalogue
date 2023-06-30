#pragma once

#include <sstream>

#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


namespace transport_catalog {

	// читаем поток и раскладываем запросы
	json::Document ReadFromInput(std::istream& input, TransportCatalogue& tc, renderer::MapRenderer& map_renderer, RequestHandler& rh);
	// обрабатывает запросы добавления в транспортный справочник
	void RequestHandlerToBase(const json::Array& arr_nodes, TransportCatalogue& tc);
	// обрабатывает запросы к транспортному справочнику и выводит готовый результат
	json::Array RequestHandlerToTC(const json::Array& arr_nodes, RequestHandler& rh);
	// обрабатывает настройки карты и передает их в модуль map_renderer
	void RequestHandlerToMap(const json::Dict& dict_node, renderer::MapRenderer& map_renderer);
	
	// добавляет дистнации между остановками
	void AddDistanceForStops(const std::unordered_map<Stop*, json::Dict>& un_distance, TransportCatalogue& tc);
	// добавляет остановки в транспортный справочник
	Stop* CreateStop(const json::Dict map_stop, TransportCatalogue& tc);
	// добавляет автобусы в трансопортный справочник
	void CreateBus(const json::Dict map_bus, TransportCatalogue& tc);
	// возвращает информацию об автобусе
	json::Dict OutBusInfo(const int id, const std::optional<BusStat>& bus_info);
	// возвращает информацию об остановке
	json::Dict OutStopInfo(const int id, const std::optional<std::set<std::string_view>*>& stop_info);
	// возвращает карту
	json::Dict OutMap(const int id, RequestHandler& rh);

}	// namespace transport_catalog