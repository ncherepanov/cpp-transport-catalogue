#include "request_handler.h"

namespace request_handler {
    
using namespace std::literals;

    json::Dict RequestHandler::GetJsonMap(const json::Node id) {
        json::Dict result;
        result["request_id"] = id;
        std::ostringstream str_out;
	    map_render::MapRenderer render(catalogue_, reader_);
	    render.GetMap(str_out);
	    std::string str_svg = str_out.str();
        result["map"s] = json::Node(str_svg);
        return result;
    }

    json::Dict RequestHandler::GetStop(const json::Node id, std::string_view name) {
        json::Dict result;
        result["request_id"] = id;
        json::Array arr;
        auto stop_stat = catalogue_.GetStopStatistic(name);
        if(stop_stat.stop.empty()) {
            result["error_message"s] = json::Node("not found"s);
        }
        else {        
            for (auto bus : stop_stat.stop_buses) {
                arr.emplace_back(json::Node(std::string(bus)));
            }
            result["buses"s] = json::Node(arr);
        }
        return result;
    }

    json::Dict RequestHandler::GetBus(const json::Node id, std::string_view name) {
        json::Dict result;
        result["request_id"s] = id;
        auto bus_stat = catalogue_.GetRouteStatistic(name);
        if(bus_stat.bus.empty()) {
            result["error_message"s] = json::Node("not found"s);
        }
        else {
            result["stop_count"s] = json::Node(bus_stat.stops);
            result["unique_stop_count"s] = json::Node(bus_stat.unique_stops);
            result["route_length"s] = json::Node(bus_stat.distance);
            result["curvature"s] = json::Node(bus_stat.curvature);
        }
        return result;        
    }

RequestHandler::RequestHandler(Catalogue& catalogue, Reader& reader)
    : catalogue_(catalogue), reader_(reader) {
    
    const json::Array& arr = reader.Request("stat_requests"s).AsArray();
        
    for (auto& maps : arr) {
        
        const auto& map = maps.AsMap();
        const json::Node id = map.at("id"s);
        json::Dict output_map;
        if (map.at("type"s).AsString() == "Map"s) {
            output_map = GetJsonMap(id);
        }
        else if (map.at("type"s).AsString() == "Stop"s) {
            const std::string_view name = map.at("name"s).AsString();
            output_map = GetStop(id, name);
        }
        else if (map.at("type"s).AsString() == "Bus"s) {
            const std::string_view name = map.at("name"s).AsString();
            output_map = GetBus(id, name);
        }
        arr_.emplace_back(std::move(json::Node(output_map)));
    }
}
    
void RequestHandler::OutputArray(std::ostream& out) const {
    json::Print(json::Document{arr_}, out);
}

}