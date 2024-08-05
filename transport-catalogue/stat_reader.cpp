
#include "stat_reader.h"

using namespace std;

namespace outputspace{

void PrintStop(catalogue::TransportCatalogue& catalogue, string_view request, ostream& out) {
    string_view stop = request.substr(5);
    auto stop_stat = catalogue.GetStopStatistics(stop);
    if(stop_stat.stop.empty()){
        out << request << ": not found"sv << endl;
        return;
    }
    else if(stop_stat.stop_buses.empty()){
        out << request << ": no buses"sv << endl;
        return;
    }
    out << "Stop "sv << stop_stat.stop << ": "sv << "buses"sv;
    for(auto stop_bus : stop_stat.stop_buses){
        out << ' ' << stop_bus;
    }
    out << endl;                                     
}

void PrintBus(catalogue::TransportCatalogue& catalogue, string_view request, ostream& out) {
    string_view bus = request.substr(4);
    auto route_stat = catalogue.GetRouteStatistics(bus);  
    if (route_stat.bus.empty()){
        out << request << ": not found"sv << endl;
        return;
    }
    out << "Bus "sv << route_stat.bus << ": "sv << route_stat.stops << " stops on route, "sv 
        << route_stat.unique_stops << " unique stops, "sv << route_stat.distance
        << " route length, "sv << route_stat.curvature << " curvature"sv << endl;                                     
}
    
void ParseAndPrintStat(catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out) {
    if(request.find("Stop"sv) == 0){
        PrintStop(catalogue, request, out); 
    }
    else if(request.find("Bus"sv) == 0){
        PrintBus(catalogue, request, out);
    }
    else 
        out << "Wrong request"sv << endl;
}

void GetFromCatalogue(std::istream& in, catalogue::TransportCatalogue& catalogue, std::ostream& out) {
    int stat_request_count;
    in >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(in, line);
        ParseAndPrintStat(catalogue, line, out);
    }    
}

}
