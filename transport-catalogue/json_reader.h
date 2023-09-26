#pragma once

#include <sstream>

#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


namespace transport_catalog {

	struct Requests {
		json::Array base_requests; // запросы на добавление в транспортный каталог
		json::Array stat_requests; // запросы получения информации из транспортного каталога
		json::Dict render_settings;  // настройки карты
		json::Dict routing_settings; // настройки маршрутизации (время ожидания автобуса и скорость автобуса)
		json::Dict serialization_settings; // настройки сериализации
	};

	class JSONReader {
	public:		
		// читаем поток и раскладываем запросы
		Requests Read(std::istream& input);
			
	};

}	// namespace transport_catalog