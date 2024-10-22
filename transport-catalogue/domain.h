#pragma once

#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "geo.h"
#include "svg.h"
 
namespace catalogue {

struct Stop{
    Stop(std::string_view stop, geo::Coordinates location)
    :stop_(stop), location_(location){}
    
    Stop(std::string_view stop)
    :stop_(stop), location_(0., 0.){}
    
    std::string stop_;
    geo::Coordinates location_;
    
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
    Bus(std::string_view bus, double length, uint32_t distance, bool roundtrip)
    :bus_(bus), length_(length), distance_(distance), roundtrip_(roundtrip){}
        
    Bus(std::string_view bus)
    :bus_(bus), length_(0), distance_(0), roundtrip_(false){}    
    
    std::string bus_;
    double length_;
    uint32_t distance_;
    bool roundtrip_;
    
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

struct Hasher {
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
    std::hash<std::string_view> hasher;
};

struct StopStatistic {
    std::string_view stop;
    geo::Coordinates location = {0., 0.};
    std::set<std::string_view> stop_buses;
};

struct BusRouteStatistic {
    std::string_view bus;
    int stops = 0;
    int unique_stops = 0;
    int distance = 0;
    double curvature = 0.;
};

}

namespace map_render {
    
struct RenderSettings {
    std::pair<double, double> stop_label_offset_ = {0., 0.};
    std::pair<double, double> bus_label_offset_ = {0., 0.};
    double width_ = 0.;
    double height_ = 0.;
    double padding_ = 0.;
    double line_width_ = 0.;
    double stop_radius_ = 0.;
    double underlayer_width_ = 0.;
    std::vector<svg::Color> color_palette_;
    svg::Color underlayer_color_;
    int bus_label_font_size_ = 1;
    int stop_label_font_size_ = 1;
};

}

namespace router {
    
struct RoutingSettings {
    double bus_wait_time_ = 0.;
    double bus_velocity_ = 0.;
};

struct RoutingPoints {
    std::string_view from_;
    std::string_view to_;
};

struct RouteSection {
    std::string_view type_;     // Wait or Bus
    std::string_view name_;
    double time_ = 0.;
    int count_ = 0;
};

struct Route {
    std::vector<RouteSection> route_;
    double total_time_ = 0.;
};  
    
}


