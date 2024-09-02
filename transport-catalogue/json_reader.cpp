
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json_reader.h"
#include "request_handler.h"

namespace json_reader {

using namespace std::literals;

const json::Node& JsonReader::Request(const std::string& request) const {
    if (!in_.GetRoot().AsMap().count(request)) { 
        return null;
    }
    return in_.GetRoot().AsMap().at(request);
}

    void FuncAddStop(catalogue::TransportCatalogue& catalogue, const json::Dict& map, const std::string& name) {
        if (map.at("type"s).AsString() == "Stop"s) {
            catalogue.AddStop(name, {map.at("latitude"s).AsDouble(), map.at("longitude"s).AsDouble()});
        }   
    }
    
    void FuncAddDist(catalogue::TransportCatalogue& catalogue, const json::Dict& map, const std::string& name) {
        if (map.at("type"s).AsString() == "Stop"s) {
            for (auto [stop, distance] : map.at("road_distances"s).AsMap()){
                catalogue.AddDistance(name, stop, distance.AsInt());
            }
        }
    }
    
    void FuncAddBus(catalogue::TransportCatalogue& catalogue, const json::Dict& map, const std::string& name) {
        if (map.at("type"s).AsString() == "Bus"s) {
            std::vector<std::string_view> stops;
            for (const auto& stop : map.at("stops"s).AsArray()) {
                stops.emplace_back(stop.AsString());
            }
            bool roundtrip = map.at("is_roundtrip"s).AsBool();
            if (roundtrip == false) {
                stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
            }
            catalogue.AddBus(name, stops, roundtrip);
        }
    }

void AddItem(catalogue::TransportCatalogue& catalogue, const json::Array& arr, void (*Func)(catalogue::TransportCatalogue&, const json::Dict&, const std::string&)) {
    for (auto& items : arr) {
        const auto& map = items.AsMap();
        const std::string& name = map.at("name"s).AsString();
        Func(catalogue, map, name);
    } 
}

void JsonReader::AddToCatalogue(catalogue::TransportCatalogue& catalogue) {
    
    const json::Array& arr = Request("base_requests"s).AsArray();
    
    AddItem(catalogue, arr, &FuncAddStop);
    AddItem(catalogue, arr, &FuncAddDist);
    AddItem(catalogue, arr, &FuncAddBus);
}

}
