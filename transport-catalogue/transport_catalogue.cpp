
#include "transport_catalogue.h"

namespace catalogue{

void TransportCatalogue::AddStop(std::string_view&& stop, geo::Coordinates&& point){
    if(stops_.count(stop)){
        return;
    }
    stops_[std::move(stop)] = std::move(point);
}

void TransportCatalogue::AddBus(std::string_view&& bus, std::vector<std::string_view>&& bus_stops){
    if(buses_.count(bus)){
        return;
    }    
    for(auto i : bus_stops) 
        stop_buses_[i].insert(bus);
    buses_[std::move(bus)] = std::move(bus_stops);
}

double TransportCatalogue::GetLength(const std::string_view& bus) const{
    double length = 0.;
    if(buses_.count(bus)){
        for(auto it = buses_.at(bus).begin(); it != prev(buses_.at(bus).end()); it = next(it)){
            length += geo::ComputeDistance(stops_.at(*it), stops_.at(*next(it)));
        } 
    }
    return length;
}

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBus(const std::string_view& bus) const{
    if(!buses_.count(bus)){
        return std::nullopt;
    }
    return buses_.at(bus);    
}

std::optional<std::set<std::string_view>> TransportCatalogue::GetStopBuses(const std::string_view& stop) const{
    if(!stop_buses_.count(stop)){
        if(!stops_.count(stop)){
            return std::nullopt;
        }
        return std::set<std::string_view>{};
    }
    return stop_buses_.at(stop);
}

}
   