
#pragma once

#include <iostream>
#include <iterator>
#include <set>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "geo.h"

namespace catalogue{

struct Stop{
    Stop(std::string_view stop, geo::Coordinates location)
    :stop_(stop), location_(location){}
    Stop(std::string_view stop)
    :stop_(stop){}
    
    std::string_view stop_;
    geo::Coordinates location_ = {0., 0.};
    
    bool operator==(const Stop& stop) const {
        return stop_ == stop.stop_;
    }
    bool operator==(const std::string_view& stop) const {
        return stop_ == stop;
    }
};

struct Bus{
    Bus(std::string_view bus)
    :bus_(bus){}
    
    std::string_view bus_;
    
    bool operator==(const Bus& bus) const {
        return bus_ == bus.bus_;
    }
    bool operator==(const std::string_view& bus) const {
        return bus_ == bus;
    }
};

class Hasher {
public:
    size_t operator()(const Bus& bus)  const {
        return hasher(bus.bus_); 
    }
    size_t operator()(const Stop& stop)  const {
        return hasher(stop.stop_); 
    }    
private:
    std::hash<std::string_view> hasher;
};

class TransportCatalogue {
    
public:

    void AddStop(const std::string_view& stop, const geo::Coordinates& point);
    
    void AddBus(const std::string_view& bus, const std::vector<std::string_view>& bus_stops);
    
    double GetLength(const std::string_view& bus) const;
    
    const std::vector<std::string_view>* GetBusStops(const std::string_view& bus);
    
    const std::set<std::string_view>* GetStopBuses(const std::string_view& stop);
    
    std::pair<std::unordered_map<Stop, std::set<std::string_view>, Hasher>, 
              std::unordered_map<Bus, std::vector<std::string_view>, Hasher>> GetAll() const{
        return {nstops_, nbuses_};
    }
    
private:
    std::unordered_map<Stop, std::set<std::string_view>, Hasher> nstops_;
    std::unordered_map<Bus, std::vector<std::string_view>, Hasher> nbuses_;
};

}
