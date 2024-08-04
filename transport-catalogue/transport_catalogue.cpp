
#include "transport_catalogue.h"

using namespace  std;

namespace catalogue{

void TransportCatalogue::AddStop(string_view stop, const geo::Coordinates& location){
    if (stops_.find(stop) != stops_.end()){
        return;
    }
    auto temp_stop = stops_str_.emplace(stop).first;
    stops_buses_[*temp_stop] = {};
    stops_.emplace(Stop(*temp_stop, location, {})); 
}

void TransportCatalogue::AddDistances(string_view stop, unordered_map<string_view, uint32_t> distances){
    auto it_stop = stops_.find(stop);
    if (it_stop == stops_.end()){
        return;
    }
    unordered_map<string_view, uint32_t> temp_dist;
    for (auto [st, dist]: distances) {
        auto it = stops_str_.find(string(st));
        temp_dist[*it] = dist;
    }
    Stop temp_stop(*it_stop);
    stops_.erase(it_stop);
    temp_stop.distances_ = temp_dist; 
    stops_.emplace(temp_stop); 
}

void TransportCatalogue::AddDistance(string_view stop_1, string_view stop_2, uint32_t distance){
    string_view temp_stop_1 = *stops_str_.find(string(stop_1));
    string_view temp_stop_2 = *stops_str_.find(string(stop_2));
    distances_[{temp_stop_1, temp_stop_2}] = distance;
    if (distances_.find({temp_stop_2, temp_stop_1}) == distances_.end()) {
        distances_[{temp_stop_2, temp_stop_1}] = distance;
    }
    
}

void TransportCatalogue::AddBus(string_view bus, vector<string_view> bus_stops){
    if (buses_.find(bus) != buses_.end()){
        return;
    }
    auto temp_bus = buses_str_.emplace(bus);
    vector<string_view> temp_stops;
    for (auto stop : bus_stops){
        auto it_stop = stops_buses_.find(stop);
        it_stop->second.insert(*temp_bus.first);                    
        temp_stops.push_back(it_stop->first);
    }
    buses_.emplace(*temp_bus.first, TransportCatalogue::GetLength(bus_stops), GetDistanceBus(bus_stops));
    buses_stops_[*temp_bus.first] =  temp_stops;
}

const vector<string_view>* TransportCatalogue::GetBusStops(string_view bus){
    if(auto iter = buses_stops_.find(bus); iter != buses_stops_.end()){
        return &(*iter).second;
    }
    return nullptr;    
}

const set<string_view>* TransportCatalogue::GetStopBuses(string_view stop){
    if(auto iter = stops_buses_.find(stop); iter != stops_buses_.end()){
            return &iter->second;
    }
    return nullptr;
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

uint32_t TransportCatalogue::GetDistanceBus(vector<string_view> stops) const {
    uint32_t distance = 0.;
    for (auto i = 0; i < stops.size()-1; ++i){
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
   