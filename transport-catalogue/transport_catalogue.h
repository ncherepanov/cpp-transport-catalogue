
#pragma once

#include <iostream>
#include <optional>
#include <set>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "geo.h"

namespace catalogue{

class TransportCatalogue {
    
public:

    void AddStop(std::string_view&& stop, geo::Coordinates&& point);
    
    void AddBus(std::string_view&& bus, std::vector<std::string_view>&& bus_stops);
    
    double GetLength(const std::string_view& bus) const;
    
    std::optional<std::vector<std::string_view>> GetBus(const std::string_view& bus) const;
    
    std::optional<std::set<std::string_view>> GetStopBuses(const std::string_view& stop) const;
    
private:
    std::unordered_map<std::string_view, geo::Coordinates> stops_;
    std::unordered_map<std::string_view, std::vector<std::string_view>> buses_;
    std::unordered_map<std::string_view, std::set<std::string_view>> stop_buses_; 
};

}