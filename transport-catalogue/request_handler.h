#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>

#include "domain.h"
#include "json.h"
#include "json_builder.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace request_handler {
    
using Catalogue = catalogue::TransportCatalogue;
using Reader = json_reader::JsonReader;
    
class RequestHandler {
public:
    //формируем данные в формате Array в ответ на запрос
    RequestHandler(Catalogue& catalogue, Reader& reader); 
    // выводим полученные json при необходимости
    void OutputArray(std::ostream& out) const;

private:
    Catalogue& catalogue_;
    Reader& reader_;
    json::Array arr_;
    
    json::Node GetJsonMap(const int id);
    json::Node GetStop(const int id, std::string_view name);
    json::Node GetBus(const int id, std::string_view name);
};

}