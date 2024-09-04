
#pragma once 

#include <iostream>
#include <map>
#include <string_view>
#include <vector>

#include "json.h"
#include "transport_catalogue.h"

namespace json_reader {

class JsonReader {
public:
    JsonReader(std::istream& in, catalogue::TransportCatalogue& catalogue);
    
    const json::Node& Request(const std::string& request) const;
    void FuncAddStop(const json::Array& arr);
    void FuncAddDist(const json::Array& arr);
    void FuncAddBus(const json::Array& arr);
  
    void AddToCatalogue();
private:
    json::Document in_;
    json::Node null = json::Node(nullptr);
    catalogue::TransportCatalogue& catalogue_;
};

}