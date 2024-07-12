#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;
    
    inputspace::InputReader reader;
    for (int i = 0; i < base_request_count; ++i) {
        string line;
        getline(cin, line);
        reader.ParseLine(line);
    }
    reader.ApplyCommands(catalogue);

    int stat_request_count;
    cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        outputspace::ParseAndPrintStat(catalogue, line, cout);
    }
}

/*
//ввод 1
13
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831

//последовательный ввод 2
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye

//результат
Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
*/