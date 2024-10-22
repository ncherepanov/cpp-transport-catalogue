
#include <iostream>
#include <string>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"


using namespace std;

int main() {
    
    catalogue::TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(cin, catalogue);
	
    request_handler::RequestHandler handler(catalogue, reader);
    handler.OutputArray(cout);
	
}
