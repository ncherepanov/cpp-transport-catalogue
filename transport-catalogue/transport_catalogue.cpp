
#include "transport_catalogue.h"

using namespace  std;

namespace catalogue{

void TransportCatalogue::AddStop(const std::string_view& stop, const geo::Coordinates& location){
    if(nstops_.find(stop) != nstops_.end()){
        return;
    }
    nstops_[Stop(stop, location)] = {};
}

void TransportCatalogue::AddBus(const std::string_view& bus, const std::vector<std::string_view>& bus_stops){
    if(nbuses_.find(bus) != nbuses_.end()){
        return;
    }    
    for(auto iter = bus_stops.begin(); iter != bus_stops.end(); iter = next(iter)){ 
        nstops_.at(*iter).insert(bus);
    }
    nbuses_[bus] = bus_stops;
}

double TransportCatalogue::GetLength(const std::string_view& bus) const{
    double length = 0.;
    if(auto iter = nbuses_.find(bus); iter != nbuses_.end()){
        for(auto it = (*iter).second.begin(); it != prev((*iter).second.end()); it = next(it)){
            length += geo::ComputeDistance( (*(nstops_.find(*it))).first.location_ ,  (*(nstops_.find(*next(it)))).first.location_ );
        } 
    }
    return length;
}

const std::vector<std::string_view>* TransportCatalogue::GetBusStops(const std::string_view& bus){
    if(auto iter = nbuses_.find(bus); iter != nbuses_.end()){
        return &(*iter).second;
    }
    return nullptr;    
}

const std::set<std::string_view>* TransportCatalogue::GetStopBuses(const std::string_view& stop){
    if(auto iter = nstops_.find(stop); iter != nstops_.end()){
            return &iter->second;
    }
    return nullptr;
}

}
   