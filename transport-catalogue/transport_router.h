
#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "domain.h"
#include "json_reader.h"
#include "router.h"
#include "transport_catalogue.h"

namespace router {
    
#define STOPS catalogue_.GetStops()
#define DISTANCES catalogue_.GetDistances()
#define BUSESSTOPS catalogue_.GetBusesStops()

using Reader = json_reader::JsonReader;
using Catalogue = catalogue::TransportCatalogue;
using Pairs_sv = std::pair<std::string_view, std::string_view>;

class TransportRouter {
public:
	TransportRouter(const Catalogue& catalogue, const Reader& reader);
	const std::optional<Route> GetRoute(RoutingPoints points);
    
private:
    const Catalogue& catalogue_;
    const Reader& reader_;
    graph::DirectedWeightedGraph<double> graph_;
    RoutingSettings settings_;
    std::unique_ptr<graph::Router<double>> router_ = nullptr;
    std::unordered_map<std::string_view, size_t, catalogue::Hasher> vertex_wait_;
    std::unordered_map<std::string_view, size_t, catalogue::Hasher> vertex_goes_;
    
	void GetRoutingSettings();
	void BuildGraph();
};

}
