
#include "stat_reader.h"

using namespace std;

namespace outputspace{

void PrintBus(catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out){
    string_view bus = request.substr(4);
    const vector<string_view>* stops = catalogue.GetBusStops(bus);  
    if (!stops){
        out << request << ": not found"sv << endl;
        return;
    }
    set<string_view> set_stops((*stops).begin(), (*stops).end());
    out << request << ": "sv << (*stops).size() << " stops on route, "sv 
        << set_stops.size() << " unique stops, "sv << catalogue.GetLength(bus) 
        << " route length"sv << endl;                                     
}
    
void PrintStop(catalogue::TransportCatalogue& catalogue, 
                                 string_view request, ostream& out){
    string_view stop = request.substr(5);
    const set<string_view>* stop_buses = catalogue.GetStopBuses(stop);
    if(!stop_buses){
        out << request << ": not found"sv << endl;
        return;
    }
    else if(!(*stop_buses).size()){
        out << request << ": no buses"sv << endl;
        return;
    }
    out << request << ": "sv << "buses"sv;
    for(auto stop_bus : *stop_buses){
        out << ' ' << stop_bus;
    }
    out << endl;                                     
}

void ParseAndPrintStat(catalogue::TransportCatalogue& catalogue, 
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

void GetFromCatalogue(std::istream& in, catalogue::TransportCatalogue& catalogue){
    int stat_request_count;
    in >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(in, line);
        ParseAndPrintStat(catalogue, line, cout);
    }    
}

}
