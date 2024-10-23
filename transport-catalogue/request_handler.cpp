#include "request_handler.h"

namespace request_handler {
    
using namespace std::literals;

    json::Node RequestHandler::GetRoute(const int id, router::RoutingPoints points) {               
        json::Node result;
        static Router router(catalogue_, GetRoutingSettings());
        auto route = router.GetRoute(points);
        if (!route) {
            result = json::Builder().StartDict().Key("request_id").Value(id).
            Key("error_message").Value("not found"s).EndDict().Build();
        }
        else {
            json::Array steps;
            for (auto step : route->route_) {
                json::Node step_dict;
                if (step.type_ == "Wait"sv) {
                    step_dict = json::Builder().StartDict().Key("type").Value("Wait"s).
                    Key("stop_name").Value(std::string(step.name_)).
                    Key("time").Value(step.time_).EndDict().Build();
                }
                else if (step.type_ == "Bus"sv) {
                    step_dict = json::Builder().StartDict().Key("type").Value("Bus"s).
                    Key("bus").Value(std::string(step.name_)).Key("span_count").Value(step.count_).
                    Key("time").Value(step.time_).EndDict().Build();
                }                
                steps.emplace_back(step_dict);
            }
            result = json::Builder().StartDict().Key("request_id").Value(id).
            Key("total_time").Value(route->total_time_).
            Key("items").Value(steps).EndDict().Build();
        }        
        return result;
    }    
    
    json::Node RequestHandler::GetJsonMap(const int id) {
        std::ostringstream str_out;
	    Render render(catalogue_, reader_);
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
    
	router::RoutingSettings RequestHandler::GetRoutingSettings() {
		static const double km_hour_to_metr_min = 1000. / 60;
		const json::Dict& map = reader_.Request("routing_settings"s).AsDict();
		router::RoutingSettings settings;
		settings.bus_wait_time_ = map.at("bus_wait_time"s).AsDouble();
		settings.bus_velocity_ = map.at("bus_velocity"s).AsDouble() * km_hour_to_metr_min;
		return settings;
	}

RequestHandler::RequestHandler(const Catalogue& catalogue, const Reader& reader)
    : catalogue_(catalogue), reader_(reader) {
    
    const json::Array& arr = reader.Request("stat_requests"s).AsArray();
        
    for (auto& maps : arr) {
        const auto& map = maps.AsDict();
        const int id = map.at("id"s).AsInt();
        json::Node node;
        if (map.at("type"s).AsString() == "Route"s) {                              
            router::RoutingPoints points;
            points.from_ = map.at("from"s).AsString();
            points.to_ = map.at("to"s).AsString();
            node = GetRoute(id, points);
        }       
        else if (map.at("type"s).AsString() == "Map"s) {
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