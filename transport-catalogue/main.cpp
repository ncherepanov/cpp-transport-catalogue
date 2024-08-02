
#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;
    {
        inputspace::InputReader reader;
        reader.AddToCatalogue(cin, catalogue);
    }
    outputspace::GetFromCatalogue(cin, catalogue);
}
