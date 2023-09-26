#include "transport_router.h"

namespace transport_catalog {


	TransportRouter::TransportRouter(const size_t bus_wait_time, const double bus_velocity)
		: bus_wait_time_(bus_wait_time)
		, bus_velocity_(bus_velocity)
	{
	}

	void TransportRouter::SetWaitTime(size_t time) {
		bus_wait_time_ = time;
	}

	void TransportRouter::SetVelocity(double velocity) {
		bus_velocity_ = velocity;
	}

	double TransportRouter::GetWaitTime() const
	{
		return bus_wait_time_;
	}

	double TransportRouter::GetVelocity() const
	{
		return bus_velocity_;
	}

	void TransportRouter::BuildGraph(const TransportCatalogue& tc) {
		const size_t number_edges = tc.GetNumberOfStops() * 2; // т.к. у каждой остановки по две вершины

		graph_ptr_ = std::make_unique<CurrentGraph>(number_edges);

		AddEdgeStops(tc.GetAllStops());
		AddEdgeBuses(tc);

		SetRouter();

	}

	std::optional<TransportRouter::RouteInfoResponse> TransportRouter::GetRouteInfo(const std::string& stop_from, const std::string& stop_to) {
		if (!router_ptr_) { return RouteInfoResponse{}; }

		const auto route_info = router_ptr_->BuildRoute(um_vertexes_of_stops_.at(stop_from), um_vertexes_of_stops_.at(stop_to));
		if (!route_info) { return std::nullopt; }

		std::vector<RouteWeight> v_result;
		v_result.reserve(route_info->edges.size());

		for (const graph::EdgeId edge_id : route_info->edges) {
			const auto& edge = graph_ptr_->GetEdge(edge_id);
			v_result.emplace_back(edge.weight);
		}

		return RouteInfoResponse{ route_info->weight.weight, std::move(v_result) };
	}

	void TransportRouter::AddEdgeStops(const std::unordered_map<std::string_view, Stop*> all_stops) {

		graph::VertexId vertex = 0;
		for (const auto& [name_stop, stop] : all_stops) {
			um_vertexes_of_stops_[name_stop] = vertex;
			graph_ptr_->AddEdge({ vertex, ++vertex, {name_stop, GetWaitTime(), true, 0} });
			++vertex; // вершина для следующей остановки
		}

	}

	void TransportRouter::AddEdgeBuses(const TransportCatalogue& tc) {
		
		for (const auto& [name_bus, bus] : tc.GetAllBuses()) {
			const int count_stops = bus->stops.size();
			double sum_distance = 0.;
			
			for (int i = 0; i < (count_stops - 1); ++i) {

				for (int j = (i + 1); j < count_stops; ++j) {
					sum_distance += tc.GetDistanceBetweenStops(bus->stops[j - 1], bus->stops[j]);
					graph_ptr_->AddEdge({ um_vertexes_of_stops_.at(bus->stops[i]->name) + 1, 
									 um_vertexes_of_stops_.at(bus->stops[j]->name), 
									 { bus->name, CalculateWeight(sum_distance), false, (j - i) } 
					});

				}
				// для новой точки рассчет сначала
				sum_distance = 0.;
			}

			// если не кольцевой маршрут простроим ребра в обратную сторону
			if (!bus->is_roundtrip) {
				for (int i = (count_stops - 1); i > 0 ; --i) {

					for (int j = (i - 1); j >= 0; --j) {
						sum_distance += tc.GetDistanceBetweenStops(bus->stops[j + 1], bus->stops[j]);
						graph_ptr_->AddEdge({ um_vertexes_of_stops_.at(bus->stops[i]->name) + 1,
										 um_vertexes_of_stops_.at(bus->stops[j]->name),
										 { bus->name, CalculateWeight(sum_distance), false, (i - j) }
							});

					}
					// для новой точки рассчет сначала
					sum_distance = 0.;
				}
			}

		}

	}

	double TransportRouter::CalculateWeight(double distance) {
		static const size_t METERS_TO_KM = 1000;
		static const double HOURS_TO_MINETS = 60;

		return ((distance / METERS_TO_KM) / bus_velocity_) * HOURS_TO_MINETS;
	}

	void TransportRouter::SetRouter() {
		router_ptr_ = std::make_unique<graph::Router<RouteWeight>>(*graph_ptr_);
	}

} // namespace transport_catalog