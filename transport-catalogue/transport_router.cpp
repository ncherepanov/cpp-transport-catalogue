
#include "transport_router.h"

namespace router {

	TransportRouter::TransportRouter(const Catalogue& catalogue, const Reader& reader)
		: catalogue_(catalogue), reader_(reader), graph_(STOPS.size() * 2) {
		GetRoutingSettings();
		BuildGraph();
	}

	const std::optional<Route> TransportRouter::GetRoute(RoutingPoints points) {
		if (points.from_ == points.to_) return { {{}, 0.} };
		auto route = router_->BuildRoute(vertex_wait_[points.from_], vertex_wait_[points.to_]);
		if (!route) return std::nullopt;
		Route result;
		for (const auto& section : route->edges) {
			auto edge_data = graph_.GetEdge(section);
			result.total_time_ += edge_data.weight;
			std::string_view  section_type = edge_data.type == graph::EdgeType::WAIT ? "Wait"sv : "Bus"sv;
			result.route_.emplace_back(RouteSection{ section_type, edge_data.edge_name, edge_data.weight, edge_data.span_count });
		}
		return result;
	}

	void TransportRouter::GetRoutingSettings() {
		static const double km_hour_to_metr_min = 1000. / 60;
		const json::Dict& map = reader_.Request("routing_settings"s).AsDict();
		settings_.bus_wait_time_ = map.at("bus_wait_time"s).AsDouble();
		settings_.bus_velocity_ = map.at("bus_velocity"s).AsDouble() * km_hour_to_metr_min;
	}

	void TransportRouter::BuildGraph() {
		size_t vertex_id = 0;
		for (const auto& stop : STOPS) {
			vertex_wait_[stop.stop_] = vertex_id++;
			vertex_goes_[stop.stop_] = vertex_id++;
			graph_.AddEdge({ vertex_id - 2, vertex_id - 1, settings_.bus_wait_time_,
							 stop.stop_, graph::EdgeType::WAIT, 0 });
		}
		for (const auto& route : BUSESSTOPS) {
			for (size_t from = 0; from < route.second.size() - 1; ++from) {
				int span_count = 0;
				double road_distance_ = 0.;
				int j = 0;
				for (size_t to = from + 1; to < route.second.size(); ++to) {
					Pairs_sv span = { route.second[from + j++], route.second[to] };
					road_distance_ += DISTANCES.at(span) / settings_.bus_velocity_;
					graph_.AddEdge({ vertex_goes_[route.second[from]], vertex_wait_[span.second], road_distance_,
									route.first, graph::EdgeType::BUS, ++span_count });
				}
			}
		}
		router_ = std::make_unique<graph::Router<double>>(graph_);
	}

}