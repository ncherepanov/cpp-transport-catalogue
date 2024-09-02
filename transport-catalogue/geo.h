#pragma once

#include <cmath>

namespace geo{

struct Coordinates {
    Coordinates(double _lat, double _lng)
    : lat(_lat), lng(_lng) {}
    
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
    
    double lat = 0.;
    double lng = 0.;
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    static const int r_earth = 6371000;
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * r_earth;
}

struct Hasher {
    size_t operator()(Coordinates pair)  const {
        return hasher(pair.lat) + 13 * hasher(pair.lng);
    }  
    std::hash<double> hasher;
};

}