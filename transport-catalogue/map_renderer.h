#pragma once

#include <iostream>
#include <optional>
#include <algorithm>
#include <cstdlib>
 
#include "domain.h"
#include "geo.h"
#include "svg.h"
#include "json_reader.h"
#include "transport_catalogue.h"
 
using Catalogue = catalogue::TransportCatalogue;
using Reader = json_reader::JsonReader;
 
namespace map_render {
    
inline const double EPSILON = 1e-6;
 
class SphereProjector {
public:
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding);
    svg::Point operator()(geo::Coordinates coords) const;
 
private:
    double padding_ = 0.;
    double min_lng_ = 0.;
    double max_lat_ = 0.;
    double zoom_coeff_ = 0.;
    bool IsZero(double value);
};

class MapRenderer {
public:
    MapRenderer(const Catalogue& catalogue, Reader& reader);
    void GetMap(std::ostream& out) const;
private:
    const Catalogue& catalogue_;
    Reader& reader_;
    RenderSettings settings_;
};

class MapCreator {
public:    
    MapCreator(const Catalogue& catalogue, const RenderSettings& settings);
    
    void AddSettingsToPolyline(svg::Polyline& polyline, uint32_t line_num) const;
    void AddLineCapJoinText(svg::Text& text) const; 
    void AddFontWeight(svg::Text& text, std::string str);
    void AddSettingText(svg::Text& text, bool is_underlayer, bool is_bus, svg::Point pos, 
                        std::string data, svg::Color item_color) const;

    void ConstructorPolyline();
    void ConstructorTxtBus();
    void ConstructorCircle();
    void ConstructorTxtStop();    
    const svg::Document* GetDoc() ;                  

private:    
    const Catalogue& catalogue_;
    const RenderSettings& settings_;
    svg::Document doc_;
    std::map<std::string_view, std::vector<std::string_view>> buses_stops_; //маршруты отсортированные по наименованию
    std::map<std::string_view, uint32_t> bus_color_;                        //маршруты и их цвета
    std::map<std::string_view, svg::Point> all_stop_;                       //для вывода слоёв кругов и наименований остановок
    std::vector<std::pair<std::string_view, std::string_view>> all_bus_;    //для вывода наименований маршрутов
};
 
// points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
template <typename PointInputIt>
SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                                 double max_width, double max_height, double padding)
: padding_(padding) {
    // Если точки поверхности сферы не заданы, вычислять нечего
    if (points_begin == points_end) {
        return;
    }
    // Находим точки с минимальной и максимальной долготой
    const auto [left_it, right_it] = std::minmax_element(points_begin, points_end,
                                     [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lng_ = left_it->lng;
    const double max_lng = right_it->lng;
    // Находим точки с минимальной и максимальной широтой
    const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end,
                                     [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;
    // Вычисляем коэффициент масштабирования вдоль координаты x
    std::optional<double> width_zoom;
    if (!IsZero(max_lng - min_lng_)) {
        width_zoom = (max_width - 2 * padding) / (max_lng - min_lng_);
    }
    // Вычисляем коэффициент масштабирования вдоль координаты y
    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }
    if (width_zoom && height_zoom) {
        // Коэффициенты масштабирования по ширине и высоте ненулевые,
        // берём минимальный из них
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    } else if (width_zoom) {
        // Коэффициент масштабирования по ширине ненулевой, используем его
        zoom_coeff_ = *width_zoom;
    } else if (height_zoom) {
        // Коэффициент масштабирования по высоте ненулевой, используем его
        zoom_coeff_ = *height_zoom;
    }
}
    
}//end namespace map_render