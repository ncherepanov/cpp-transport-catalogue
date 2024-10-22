
#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"
#include "domain.h"

namespace catalogue{

class TransportCatalogue {
    
public:

    void AddStop(std::string_view stop, const geo::Coordinates& point);
    
    void AddDistance(std::string_view stop_1, std::string_view stop_2, uint32_t distance);
    
    void AddBus(std::string_view bus, std::vector<std::string_view>& bus_stops, bool roundtrip);
    
    std::vector<std::string_view> GetBusStops(std::string_view bus) const;
    
    std::set<std::string_view> GetStopBuses(std::string_view stop) const;
    
    double GetBusLen(std::string_view bus) const;
    
    uint32_t GetBusDist(std::string_view bus) const;    
    
    uint32_t GetDistance(std::string_view stop_1, std::string_view stop_2) const;
    
    StopStatistic GetStopStatistic(std::string_view stop) const;
    
    BusRouteStatistic GetRouteStatistic(std::string_view) const;
    
    auto& GetStops() const { return stops_; }
    auto& GetBuses() const { return buses_; }
    auto& GetStopsBuses() const { return stops_buses_; }
    auto& GetBusesStops() const { return buses_stops_; }
    auto& GetDistances() const { return distances_; }
    auto& GetSpanBus() const { return span_bus_; }
    auto& GetSpanNum(std::string_view bus) const { return span_num_bus_. at(bus); }
    
private:
    std::unordered_set<Stop, Hasher> stops_;
    std::unordered_set<Bus, Hasher> buses_;    
    std::unordered_map<std::string_view, std::set<std::string_view>, Hasher> stops_buses_;
    std::unordered_map<std::string_view, std::vector<std::string_view>, Hasher> buses_stops_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, uint32_t, Hasher> distances_buf_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, uint32_t, Hasher> distances_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, std::set<std::string_view>, Hasher> span_bus_;
    std::unordered_map<std::string_view, std::map<std::pair<std::string_view, std::string_view>, int>, Hasher> span_num_bus_;
    
    uint32_t GetDistanceBus(std::vector<std::string_view> bus_stops) const;
    double GetLength(std::vector<std::string_view> bus_stops) const;
};

}
