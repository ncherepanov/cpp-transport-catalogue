#include "request_handler.h"

namespace request_handler {
    
using namespace std::literals;

    json::Node RequestHandler::GetJsonMap(const int id) {
        std::ostringstream str_out;
	    map_render::MapRenderer render(catalogue_, reader_);
	    render.GetMap(str_out);
	    std::string str_svg = str_out.str();
	    json::Node result = json::Builder().StartDict().Key("request_id").Value(id).
        Key("map").Value(str_svg).EndDict().Build();
        return result;
    }

    json::Node RequestHandler::GetStop(const int id, std::string_view name) {
        json::Node result;
        auto stop_stat = catalogue_.GetStopStatistic(name);
        if (stop_stat.stop.empty()) {
            result = json::Builder().StartDict().Key("request_id").Value(id).
            Key("error_message").Value("not found").EndDict().Build();
        }
        else {
            json::Array buses;
            for (auto bus : stop_stat.stop_buses) {
                buses.emplace_back(json::Node(std::string(bus)));
            }
            result = json::Builder().StartDict().Key("request_id").Value(id).
            Key("buses").Value(buses).EndDict().Build();
        }
        return result;
    }

    json::Node RequestHandler::GetBus(const int id, std::string_view name) {
        json::Node result;
        auto bus_stat = catalogue_.GetRouteStatistic(name);
        if (bus_stat.bus.empty()) {
            result = json::Builder().StartDict().Key("request_id").Value(id).
            Key("error_message").Value("not found").EndDict().Build();
        }
        else {
            result = json::Builder().StartDict().Key("request_id").Value(id).
                     Key("stop_count").Value(bus_stat.stops).           
                     Key("unique_stop_count").Value(bus_stat.unique_stops).
                     Key("route_length").Value(bus_stat.distance).
                     Key("curvature").Value(bus_stat.curvature).
                     EndDict().Build(); 
        }
        return result;       
    }

RequestHandler::RequestHandler(Catalogue& catalogue, Reader& reader)
    : catalogue_(catalogue), reader_(reader) {
    
    const json::Array& arr = reader.Request("stat_requests"s).AsArray();
        
    for (auto& maps : arr) {
        
        const auto& map = maps.AsDict();
        const int id = map.at("id"s).AsInt();
        json::Node node;
        if (map.at("type"s).AsString() == "Map"s) {
            node = GetJsonMap(id);
        }
        else if (map.at("type"s).AsString() == "Stop"s) {
            const std::string_view name = map.at("name"s).AsString();
            node = GetStop(id, name);
        }
        else if (map.at("type"s).AsString() == "Bus"s) {
            const std::string_view name = map.at("name"s).AsString();
            node = GetBus(id, name);
        }
        arr_.emplace_back(std::move(node));
    }
}
    
void RequestHandler::OutputArray(std::ostream& out) const {
    json::Print(json::Document{arr_}, out);
}

}