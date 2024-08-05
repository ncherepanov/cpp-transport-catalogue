
#pragma once

#include <iosfwd>
#include <iostream>
#include <set>
#include <string_view>

#include "transport_catalogue.h"

namespace outputspace{
                       
void GetFromCatalogue(std::istream& in, catalogue::TransportCatalogue& catalogue, std::ostream& out);    
    
}

