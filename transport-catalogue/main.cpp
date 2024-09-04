
#include <iostream>
#include <string>

#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace std;

int main() {

	catalogue::TransportCatalogue catalogue;
	
	json_reader::JsonReader reader(cin, catalogue);
	//reader.AddToCatalogue(catalogue);
	
	request_handler::RequestHandler handler(catalogue, reader);
	handler.OutputArray(cout);	
}
