
#pragma once

#include <iosfwd>
#include <iostream>
#include <optional>
#include <set>
#include <string_view>

#include "transport_catalogue.h"

namespace outputspace{

void ParseAndPrintStat(const catalogue::TransportCatalogue& catalogue, 
                       std::string_view request, std::ostream& out);
                       
}                       