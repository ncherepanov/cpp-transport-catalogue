#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <optional>
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
#include "transport_router.h"

namespace request_handler {
    
using Catalogue = catalogue::TransportCatalogue;
using Reader = json_reader::JsonReader;
using Render = map_render::MapRenderer;
using Router = router::TransportRouter;
    
class RequestHandler {
public:
    //формируем данные в формате Array в ответ на запрос
    RequestHandler(const Catalogue& catalogue, const Reader& reader); 
    // выводим полученные json при необходимости
    void OutputArray(std::ostream& out) const;

private:
    const Catalogue& catalogue_;
    const Reader& reader_;
    json::Array arr_;
    
    json::Node GetRoute(const int id, router::RoutingPoints points);
    json::Node GetJsonMap(const int id);
    json::Node GetStop(const int id, std::string_view name);
    json::Node GetBus(const int id, std::string_view name);
};

}