#pragma once

#include <fstream>
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "svg.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

class Serialization {
public:
	Serialization() = default;
	~Serialization() = default;

	void SetFileName(const std::string& name);
	const std::string& GetFileName() const;
	// Сериализуем траспортный каталог
	void Serialize(std::ofstream& output, const transport_catalog::TransportCatalogue& tc, const transport_catalog::renderer::MapRenderer& renderer, const transport_catalog::TransportRouter& transport_router);
	// Десериализуем и заполним транспортный каталог
	void Deserialize(std::ifstream& input, transport_catalog::TransportCatalogue& tc, transport_catalog::renderer::MapRenderer& renderer, transport_catalog::TransportRouter& transport_router);

private:
	std::string file_name_;

	// Serialize
	transport_catalog_serialize::Stop SerializeStop(const transport_catalog::Stop* stop_ptr);
	transport_catalog_serialize::Bus SerializeBus(const transport_catalog::Bus* bus_ptr);
	transport_catalog_serialize::Distance SerializeDistance(const std::pair<transport_catalog::Stop*, transport_catalog::Stop*> p_stops, double distance);
	transport_catalog_serialize::MapSettings SerializeMapSettings(const transport_catalog::renderer::MapSettings& settings);
	transport_catalog_serialize::Color SerializeColor(const svg::Color& color);
	// Deserialization
	void DeserializeStop(const transport_catalog_serialize::Stop& stop, transport_catalog::TransportCatalogue& tc);
	void DeserializeBus(const transport_catalog_serialize::Bus& bus, const transport_catalog_serialize::TransportCatalogue& tc_serialized, transport_catalog::TransportCatalogue& tc);
	std::string GetStopName(const transport_catalog_serialize::TransportCatalogue& tc_serialized, const uint64_t stop_id);
	void DeserializeMapSettings(const transport_catalog_serialize::MapSettings& map_settings, transport_catalog::renderer::MapRenderer& renderer);
	svg::Color DeserializeColor(const transport_catalog_serialize::Color& color);
};
