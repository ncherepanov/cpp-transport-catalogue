
#include "transport_catalogue.h"

using namespace  std;

namespace catalogue{

void TransportCatalogue::AddStop(string_view stop, const geo::Coordinates& location) {
    if (stops_.find(stop) != stops_.end()) {
        return;
    }
    auto it_stop = stops_.emplace(move(stop), location);
    stops_buses_[it_stop.first->stop_] = {};
}

void TransportCatalogue::AddDistance(string_view stop_1, string_view stop_2, uint32_t distance) {
    stop_1 = stops_.find(stop_1)->stop_;
    stop_2 = stops_.find(stop_2)->stop_;
    distances_buf_[{stop_1, stop_2}] = distance;
    if (distances_buf_.find({stop_2, stop_1}) == distances_buf_.end()) {
        distances_buf_[{stop_2, stop_1}] = distance;
    }
}

void TransportCatalogue::AddBus(string_view bus, vector<string_view>& bus_stops, bool roundtrip) {
    if (buses_.find(bus) != buses_.end()) {
        return;
    }
    for (auto& stop : bus_stops){
        auto it_stop = stops_buses_.find(stop);
        stop = it_stop->first;
        it_stop->second.insert(bus);      
    }
    buses_stops_[bus] = bus_stops;
    std::map<std::pair<std::string_view, std::string_view>, int> span_num;
    for (size_t i = 0; i < bus_stops.size() - 1; ++i) {
        span_bus_[{bus_stops[i], bus_stops[i + 1]}].emplace(bus);
        span_num[{bus_stops[i], bus_stops[i + 1]}] = i;
        auto it_frwrd = distances_buf_.find({bus_stops[i], bus_stops[i+1]});
        auto it_bwrd = distances_buf_.find({bus_stops[i + 1], bus_stops[i]});
        bool finded_frwrd = it_frwrd != distances_buf_.end();
        bool finded_bwrd = it_bwrd != distances_buf_.end();
        distances_[{bus_stops[i], bus_stops[i + 1]}] = !finded_frwrd ? it_bwrd->second : it_frwrd->second;
        if (!roundtrip) {
            distances_[{bus_stops[i+1], bus_stops[i]}] = !finded_bwrd ? it_frwrd->second : it_bwrd->second;
        }
    }
    span_num_bus_[bus] = span_num;
    buses_.emplace(move(bus), TransportCatalogue::GetLength(bus_stops), GetDistanceBus(bus_stops), roundtrip);
}

vector<string_view> TransportCatalogue::GetBusStops(string_view bus) const {
    if(auto iter = buses_stops_.find(bus); iter != buses_stops_.end()) {
        return iter->second;
    }
    return {};
}

set<string_view> TransportCatalogue::GetStopBuses(string_view stop) const {
    if(auto iter = stops_buses_.find(stop); iter != stops_buses_.end()){
            return iter->second;
    }
    return {};
}

double TransportCatalogue::GetBusLen(string_view bus) const {
    return buses_.find(bus) -> length_;
}
    
uint32_t TransportCatalogue::GetBusDist(string_view bus) const {
    return buses_.find(bus) -> distance_;
}

uint32_t TransportCatalogue::GetDistance(string_view stop_1, string_view stop_2) const {
    uint32_t distance = 0;
    if (auto it = distances_.find({stop_1, stop_2}); it != distances_.end()){
        distance = it->second;
    }
    return distance;
}

StopStatistic TransportCatalogue::GetStopStatistic(std::string_view stop) const {
    auto it_stop = stops_.find(stop);
    if(it_stop == stops_.end()){
        return {};
    }
    StopStatistic stop_stat;
    stop_stat.stop = it_stop->stop_;
    stop_stat.location = it_stop->location_;
    stop_stat.stop_buses = GetStopBuses(stop);
    return stop_stat;
}

BusRouteStatistic TransportCatalogue::GetRouteStatistic(std::string_view bus) const {
    auto it_bus = buses_.find(bus);
    if(it_bus == buses_.end()){
        return {};
    }
    BusRouteStatistic route_stat;
    route_stat.bus = it_bus->bus_;
    vector <string_view> stops = GetBusStops(bus);
    route_stat.stops = stops.size();
    set<string_view> set_stops(stops.begin(), stops.end());
    route_stat.unique_stops = set_stops.size();
    route_stat.distance = GetBusDist(bus);
    route_stat.curvature = GetBusDist(bus)/GetBusLen(bus);
    return route_stat;
}

uint32_t TransportCatalogue::GetDistanceBus(vector<string_view> stops) const {
    uint32_t distance = 0;
    for (size_t i = 0; i < stops.size()-1; ++i) {
        distance += GetDistance(stops[i], stops[i+1]);
    }
    return distance;
}

double TransportCatalogue::GetLength(vector<string_view> bus_stops) const{
    double length = 0.;
        for(auto it = bus_stops.begin(); it != prev(bus_stops.end()); it = next(it)){
            length += geo::ComputeDistance( (*(stops_.find(*it))).location_ ,  (*(stops_.find(*next(it)))).location_ );
        }
    return length;
}

}
   