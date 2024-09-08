
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json_reader.h"
#include "request_handler.h"

namespace json_reader {

using namespace std::literals;

JsonReader::JsonReader(std::istream& in, catalogue::TransportCatalogue& catalogue) 
    : in_(json::Load(in)), catalogue_(catalogue) {
    AddToCatalogue();
}

const json::Node& JsonReader::Request(const std::string& request) const {
    if (!in_.GetRoot().AsDict().count(request)) { 
        return null;
    }
    return in_.GetRoot().AsDict().at(request);
}

void JsonReader::FuncAddStop(const json::Array& arr) {
    for (auto& items : arr) {
        const auto& map = items.AsDict();
        const std::string& name = map.at("name"s).AsString();
        if (map.at("type"s).AsString() == "Stop"s) {
            catalogue_.AddStop(name, {map.at("latitude"s).AsDouble(), map.at("longitude"s).AsDouble()});
        }       
    }     
}

void JsonReader::FuncAddDist(const json::Array& arr) {
    for (auto& items : arr) {
        const auto& map = items.AsDict();
        const std::string& name = map.at("name"s).AsString();
        if (map.at("type"s).AsString() == "Stop"s) {
            for (auto [stop, distance] : map.at("road_distances"s).AsDict()){
                catalogue_.AddDistance(name, stop, distance.AsInt());
            }
        }
    } 
}
    
void JsonReader::FuncAddBus(const json::Array& arr) {
    for (auto& items : arr) {
        const auto& map = items.AsDict();
        const std::string& name = map.at("name"s).AsString();
        if (map.at("type"s).AsString() == "Bus"s) {
            std::vector<std::string_view> stops;
            for (const auto& stop : map.at("stops"s).AsArray()) {
                stops.emplace_back(stop.AsString());
            }
            bool roundtrip = map.at("is_roundtrip"s).AsBool();
            if (roundtrip == false) {
                stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
            }
            catalogue_.AddBus(name, stops, roundtrip);
        }
    }         
}

void JsonReader::AddToCatalogue() {
    const json::Array& arr = Request("base_requests"s).AsArray();
    FuncAddStop(arr);
    FuncAddDist(arr);
    FuncAddBus(arr);
}

}
