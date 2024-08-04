
#pragma once

#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"

namespace catalogue{

struct Stop{
    Stop(std::string_view stop, geo::Coordinates location, 
         std::unordered_map<std::string_view, uint32_t> distances)
    :stop_(stop), location_(location), distances_(distances){}
    
    Stop(std::string_view stop)
    :stop_(stop), location_({0., 0.}), distances_({}){}
    
    Stop(const Stop& another){
        stop_ = another.stop_;
        location_ = another.location_;
        distances_ = another.distances_;
    }
    
    std::string_view stop_;
    geo::Coordinates location_;
    std::unordered_map<std::string_view, uint32_t> distances_;
    
    bool operator==(const Stop& stop) const {
        return stop_ == stop.stop_;
    }
    bool operator==(const std::string_view& stop) const {
        return stop_ == stop;
    }
    bool operator<(const Stop& stop) const {
        return stop_ < stop.stop_;
    }
    bool operator<(const std::string_view& stop) const {
        return stop_ < stop;
    }    
};

struct Bus{
    Bus(std::string_view bus, double length, uint32_t distance)
    :bus_(bus), length_(length), distance_(distance){}
        
    Bus(std::string_view bus)
    :bus_(bus), length_(0), distance_(0){}    
    
    std::string_view bus_;
    double length_;
    uint32_t distance_;
    
    bool operator==(const Bus& bus) const {
        return bus_ == bus.bus_;
    }
    bool operator==(const std::string_view& bus) const {
        return bus_ == bus;
    }
    bool operator<(const Bus& bus) const {
        return bus_ < bus.bus_;
    }
    bool operator<(const std::string_view& bus) const {
        return bus_ < bus;
    }    
};

class Hasher {
public:
    size_t operator()(std::string_view name)  const {
        return hasher(name); 
    }
    size_t operator()(const Bus& bus)  const {
        return hasher(bus.bus_); 
    }
    size_t operator()(const Stop& stop)  const {
        return hasher(stop.stop_); 
    }
    size_t operator()(std::pair<std::string_view, std::string_view> pair)  const {
        return hasher(pair.first) + 13 * hasher(pair.second);
    }  
private:
    std::hash<std::string_view> hasher;
};

class TransportCatalogue {
    
public:

    void AddStop(std::string_view stop, const geo::Coordinates& point);
    
    void AddDistances(std::string_view stop, std::unordered_map<std::string_view, uint32_t> distances);
    
    void AddDistance(std::string_view stop_1, std::string_view stop_2, uint32_t distance);
    
    void AddBus(std::string_view bus, std::vector<std::string_view> bus_stops);
    
    const std::vector<std::string_view>* GetBusStops(std::string_view bus);
    
    const std::set<std::string_view>* GetStopBuses(std::string_view stop);
    
    double GetBusLen(std::string_view bus) const;
    
    uint32_t GetBusDist(std::string_view bus) const;    
    
    uint32_t GetDistance(std::string_view stop_1, std::string_view stop_2) const;
    
private:
    std::set<std::string> stops_str_;
    std::set<std::string> buses_str_;    
    std::unordered_set<Stop, Hasher> stops_;
    std::unordered_set<Bus, Hasher> buses_;    
    std::unordered_map<std::string_view, std::set<std::string_view>, Hasher> stops_buses_;
    std::unordered_map<std::string_view, std::vector<std::string_view>, Hasher> buses_stops_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, uint32_t, Hasher> distances_;
    
    uint32_t GetDistanceBus(std::vector<std::string_view> bus_stops) const;
    double GetLength(std::vector<std::string_view> bus_stops) const;
};

}
