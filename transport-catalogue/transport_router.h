#pragma once

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>


namespace transport_catalog {

	class TransportRouter {
	public:

		struct RouteWeight {
			std::string_view name = "";
			double weight = 0;
			bool is_waiting = false;
			int span_count = 0;

			bool operator<(const RouteWeight& rhs) const {
				return weight < rhs.weight;
			}

			bool operator>(const RouteWeight& rhs) const {
				return weight > rhs.weight;
			}

			RouteWeight operator+(const RouteWeight& rhs) const {
				return RouteWeight{
							rhs.name,
							weight + rhs.weight,
							is_waiting == rhs.is_waiting,
							span_count + rhs.span_count
				};
			}
		};

		struct RouteInfoResponse {
			double total_time = 0.;
			std::vector<RouteWeight> items;
		};

		TransportRouter() = default;
		TransportRouter(const size_t bus_wait_time, const double bus_velocity);

		void SetWaitTime(size_t time);
		void SetVelocity(double velocity);

		double GetWaitTime() const;
		double GetVelocity() const;

		void BuildGraph(const TransportCatalogue& tc);
		std::optional<RouteInfoResponse> GetRouteInfo(const std::string& stop_from, const std::string& stop_to);

	private:
		using CurrentGraph = graph::DirectedWeightedGraph<RouteWeight>;

		double bus_wait_time_ = 0; // время ожидания автобуса в минутах
		double bus_velocity_ = 0; // скорость автобуса км/ч

		std::unordered_map<std::string_view, graph::VertexId> um_vertexes_of_stops_; // вершины входа в ожидание по остановкам
		std::unique_ptr<CurrentGraph> graph_ptr_;
		std::unique_ptr<graph::Router<RouteWeight>> router_ptr_;

		// добавляет ребра остановок (ожиданий)
		void AddEdgeStops(const std::unordered_map<std::string_view, Stop*> all_stops);
		// добавляет ребра поездок
		void AddEdgeBuses(const TransportCatalogue& tc);
		// рассчитывает вес
		double CalculateWeight(double distance);
		// устанавливает указатель на роутер
		void SetRouter();
	};

} // namespace transport_catalog