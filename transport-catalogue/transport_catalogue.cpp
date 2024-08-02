
#include "transport_catalogue.h"

using namespace  std;

namespace catalogue{

void TransportCatalogue::AddStop(std::string_view stop, const geo::Coordinates& location){
    if (stops_.find(stop) != stops_.end()){
        return;
    }
    auto temp_stop = stops_str_.emplace(stop).first;
    stops_buses_[*temp_stop] = {};
    stops_.emplace(Stop(*temp_stop, location, {})); 
}

void TransportCatalogue::AddDistances(std::string_view stop, std::unordered_map<std::string_view, uint32_t> distances){
    auto it_stop = stops_.find(stop);
    if (it_stop == stops_.end()){
        return;
    }
    std::unordered_map<std::string_view, uint32_t> temp_dist;
    for (auto [st, dist]: distances) {
        auto it = stops_str_.find(string(st));
        temp_dist[*it] = dist;
    }
    Stop temp_stop(*it_stop);
    stops_.erase(it_stop);
    temp_stop.distances_ = temp_dist; 
    stops_.emplace(temp_stop); 
}

void TransportCatalogue::AddBus(std::string_view bus, std::vector<std::string_view> bus_stops){
    if (buses_.find(bus) != buses_.end()){
        return;
    }
    auto temp_bus = buses_str_.emplace(bus);
    std::vector<std::string_view> temp_stops;
    for (auto stop : bus_stops){
        auto it_stop = stops_buses_.find(stop);
        it_stop->second.insert(*temp_bus.first);                    
        temp_stops.push_back(it_stop->first);
    }
    buses_.emplace(*temp_bus.first, TransportCatalogue::GetLength(bus_stops), GetDistance(bus_stops));
    buses_stops_[*temp_bus.first] =  temp_stops;
}

const std::vector<std::string_view>* TransportCatalogue::GetBusStops(std::string_view bus){
    if(auto iter = buses_stops_.find(bus); iter != buses_stops_.end()){
        return &(*iter).second;
    }
    return nullptr;    
}

const std::set<std::string_view>* TransportCatalogue::GetStopBuses(std::string_view stop){
    if(auto iter = stops_buses_.find(stop); iter != stops_buses_.end()){
            return &iter->second;
    }
    return nullptr;
}

double TransportCatalogue::GetLength(std::vector<std::string_view> bus_stops) const{
    double length = 0.;
        for(auto it = bus_stops.begin(); it != prev(bus_stops.end()); it = next(it)){
            length += geo::ComputeDistance( (*(stops_.find(*it))).location_ ,  (*(stops_.find(*next(it)))).location_ );
        }
    return length;
}

uint32_t TransportCatalogue::GetDistance(std::vector<std::string_view> bus_stops) const{
    uint32_t distance = 0.;
    for (auto it = bus_stops.begin(); it != prev(bus_stops.end()); it = next(it)){
        auto it_stop_1 = stops_.find(*it);
        if (auto it_stop_2 = it_stop_1->distances_.find(*next(it)); it_stop_2 !=  it_stop_1->distances_.end()){
            distance += it_stop_2->second;
        }
        else {
            auto it_stop_3 = stops_.find(*next(it));
            if (auto it_stop_4 = it_stop_3->distances_.find(*it); it_stop_4 !=  it_stop_3->distances_.end()){
                distance += it_stop_4->second;
            }
        }
    }
    return distance;
}

}
   