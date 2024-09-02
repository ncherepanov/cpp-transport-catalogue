
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
    JsonReader(std::istream& in) 
    : in_(json::Load(in)) {}
    
    const json::Node& Request(const std::string& request) const;

    void AddToCatalogue(catalogue::TransportCatalogue& catalogue);
    
private:
    json::Document in_;
    json::Node null = json::Node(nullptr);
};

}