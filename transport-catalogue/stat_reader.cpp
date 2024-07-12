
#include "stat_reader.h"

using namespace std;

namespace outputspace{

void PrintBus(const catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out){
    string_view bus = request.substr(4);
    optional<vector<string_view>> stops = catalogue.GetBus(bus);  
    if (!stops){
        out << request << ": not found"sv << endl;
        return;
    }
    set<string_view> set_stops((*stops).begin(), (*stops).end());
    out << request << ": "sv << (*stops).size() << " stops on route, "sv 
        << set_stops.size() << " unique stops, "sv << catalogue.GetLength(bus) 
        << " route length"sv << endl;                                     
}
    
void PrintStop(const catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out){
    string_view stop = request.substr(5);
    optional<set<string_view>> stop_buses = catalogue.GetStopBuses(stop);
    if(!stop_buses){
        out << request << ": not found"sv << endl;
        return;
    }
    else if(!(*stop_buses).size()){
        out << request << ": no buses"sv << endl;
        return;
    }
    out << request << ": "sv << "buses"sv;
    for(auto i : *stop_buses){
        out << ' ' << i;
    }
    out << endl;                                     
}

void ParseAndPrintStat(const catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out) {
    if(request.find("Bus"sv) == 0){
        PrintBus(catalogue, request, out);
    }
    else if(request.find("Stop"sv) == 0){
        PrintStop(catalogue, request, out);
    } 
    else 
        out << "Wrong request"sv << endl;
}

}