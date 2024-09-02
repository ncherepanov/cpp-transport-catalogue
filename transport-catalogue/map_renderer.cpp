#include "map_renderer.h"
 
using namespace std::literals;
 
namespace map_render {
    
svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return { (coords.lng - min_lng_) * zoom_coeff_ + padding_,
             (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
}
 
bool SphereProjector::IsZero(double value) {
    return std::abs(value) < EPSILON;
}
    
MapRenderer::MapRenderer(Catalogue& catalogue, Reader& reader) 
: catalogue_(catalogue), reader_(reader) {

    const json::Dict& map = reader.Request("render_settings"s).AsMap();

    if (map.at("stop_label_offset"s).IsArray()) {
        auto stop_lab_offset = map.at("stop_label_offset"s).AsArray();
        settings_.stop_label_offset_ = {stop_lab_offset[0].AsDouble(), stop_lab_offset[1].AsDouble()};
    }
    if (map.at("bus_label_offset"s).IsArray()) {
        auto bus_lab_offset = map.at("bus_label_offset"s).AsArray();
        settings_.bus_label_offset_ = {bus_lab_offset[0].AsDouble(), bus_lab_offset[1].AsDouble()};
    }
    settings_.width_ = map.at("width"s).AsDouble();
    settings_.height_ = map.at("height"s).AsDouble();
    settings_.padding_ = map.at("padding"s).AsDouble();
    settings_.line_width_ = map.at("line_width"s).AsDouble();
    settings_.stop_radius_ = map.at("stop_radius"s).AsDouble();
    settings_.stop_label_font_size_ = map.at("stop_label_font_size"s).AsInt();
    settings_.bus_label_font_size_ = map.at("bus_label_font_size"s).AsInt();
    if (map.at("underlayer_color"s).IsString()) {
        settings_.underlayer_color_ = svg::Color(map.at("underlayer_color"s).AsString());
    } 
    else if (map.at("underlayer_color"s).IsArray()) {
        auto arr_color = map.at("underlayer_color"s).AsArray();
        int red = arr_color[0].AsInt();
        int green = arr_color[1].AsInt();
        int blue = arr_color[2].AsInt();
        if (arr_color.size() == 3){
            settings_.underlayer_color_ = svg::Color(svg::Rgb(red, green, blue));
        } 
        else {
            settings_.underlayer_color_ = svg::Color(svg::Rgba(red, green, blue, arr_color[3].AsDouble()));
        }
    }
    settings_.underlayer_width_ = map.at("underlayer_width"s).AsDouble();
    if (map.at("color_palette"s).IsArray()) {
        auto arr_color = map.at("color_palette"s).AsArray();
        for (json::Node color : arr_color) {
            if (color.IsString()) {
                settings_.color_palette_.emplace_back(color.AsString());
            } 
            else if (color.IsArray()) {
                auto arr_color = color.AsArray();
                int red = arr_color[0].AsInt();
                int green = arr_color[1].AsInt();
                int blue = arr_color[2].AsInt();
                if (arr_color.size() == 3) {
                    settings_.color_palette_.emplace_back(svg::Rgb(red, green, blue));
                } 
                else {
                    settings_.color_palette_.emplace_back(svg::Rgba(red, green, blue, arr_color[3].AsDouble()));
                }
            }
        }
    }
}
  
uint32_t MapRenderer::GetPaletteSize() const{
    return settings_.color_palette_.size();
}
    
svg::Color MapRenderer::GetColor(uint32_t line_number) const {
    return settings_.color_palette_[line_number];
}
 
void MapRenderer::AddSettingsToPolyline(svg::Polyline& polyline, uint32_t line_num) const {
    polyline.SetStrokeColor(GetColor(line_num)).SetFillColor("none"s).SetStrokeWidth(settings_.line_width_).
             SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapRenderer::AddLineCapJoinText(svg::Text& text) const {
    text.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void AddFontWeight(svg::Text& text, std::string str = "bold"s) {
    text.SetFontWeight(str);
}

void MapRenderer::AddSettingText(svg::Text& text, bool is_underlayer, bool is_bus, svg::Point pos, 
                                                    std::string data, svg::Color item_color) const {
                                              
    svg::Color color = is_underlayer ? settings_.underlayer_color_ : item_color;
    svg::Point offset = is_bus ? svg::Point({settings_.bus_label_offset_.first, settings_.bus_label_offset_.second})
                               : svg::Point({settings_.stop_label_offset_.first, settings_.stop_label_offset_.second});
    int font_size = is_bus ? settings_.bus_label_font_size_ : settings_.stop_label_font_size_;
    
    text.SetFillColor(color).SetFontFamily("Verdana"s).SetFontSize(font_size).SetPosition(pos).SetOffset(offset).SetData(data);
    
    if (is_underlayer) {
        text.SetStrokeWidth(settings_.underlayer_width_).SetStrokeColor(color);
    }
}

void MapRenderer::GetMap(std::ostream& out) const 
{
    svg::Document document;
    
    std::unordered_set<std::string_view, catalogue::Hasher> stops_routed;   //все остановки входящие в маршруты
    std::map<std::string_view, uint32_t> bus_color;                        //маршруты и их цвета 
    std::map<std::string_view, std::vector<std::string_view>> bus_stops_temp;  // маршрут - остановки для прохода с правильной сортировкой маршрута

    std::map<std::string_view, svg::Point> all_stop;                  //для вывода слоёв кругов и наименований остановок
    std::vector<std::pair<std::string_view, std::string_view>> all_bus; //для вывода наименований маршрутов и 

    for (const auto& [bus, stops] : catalogue_.GetBusesStops()) {  //складируем все остановки во всех маршрутах 
        if (!stops.empty()) {
            stops_routed.insert(stops.begin(), stops.end());
            bus_stops_temp[bus] = stops;                           //заполняем маршрут - остановки для прохода с правильной сортировкой маршрута
        }
    }
    
    std::vector<geo::Coordinates> set_point;
    for (const auto& stop : catalogue_.GetStops()) {               // складируем координаты тех остановок, которые есть в маршрутах
        if (stops_routed.count(stop.stop_)) {        
            set_point.emplace_back(stop.location_);
        }
    }
    // создаём объект SphereProjector для пересчёта координат в х у
    SphereProjector projector(set_point.begin(), set_point.end(), settings_.width_, settings_.height_, settings_.padding_);
    
    uint32_t num_color = 0; 
    for (const auto& [bus, stops] : bus_stops_temp) { 
        all_bus.emplace_back(std::make_pair(bus, stops.front()));
        if (!catalogue_.GetBuses().find(bus)->roundtrip_ && stops.front() != stops[stops.size()/2]) {
            all_bus.emplace_back(std::make_pair(bus, stops[stops.size()/2]));
        }
                    
        std::vector<std::pair<std::string_view, svg::Point>> stops_pos; // вектор содержащий остановки и их х у для текущего автобуса
        svg::Polyline points; //инициализируем для складывания координат х у и свойств для текущего автобуса
        for (size_t i = 0; i < stops.size(); ++i) {      // проходимся по остановкам и складируем их координаты х у
            svg::Point point = projector(catalogue_.GetStops().find(stops[i])->location_); // пересчитываем в х у
            stops_pos.emplace_back(std::make_pair(stops[i], point)); // заполняем контейнер остановка координаты
            points.AddPoint(point);     // заполняем полилайн
            all_stop[stops[i]] = point;
        }
        bus_color[bus] = num_color;    
        AddSettingsToPolyline(points, num_color);   // дописываем в полилайн прочие настройки
        num_color = num_color < GetPaletteSize() - 1 ? num_color + 1 : 0; //инкремент номера цвета, цвета закончились - начинаем с нуля
        document.Add(points); // добавляем сформированный полилайн
    }
    
    for (auto [bus, stop] : all_bus) {
        svg::Text text_bus_under;
        AddLineCapJoinText(text_bus_under);
        svg::Point pos = all_stop.at(stop);
        std::string data = std::string(bus); 
        AddSettingText(text_bus_under, true, true, pos, data, svg::NoneColor);
        AddFontWeight(text_bus_under);
        document.Add(text_bus_under);
        
        svg::Text text_bus;
        AddSettingText(text_bus, false, true, pos, data, GetColor(bus_color.at(bus)));
        AddFontWeight(text_bus);
        document.Add(text_bus);        
    }

    for (auto stop : all_stop) {
        document.Add(svg::Circle().SetCenter(stop.second).SetRadius(settings_.stop_radius_).SetFillColor("white"s));
    } 
    
    for (auto stop : all_stop) {
        svg::Text text_stop_under;
        AddLineCapJoinText(text_stop_under);
        svg::Point pos = stop.second;
        std::string data = std::string(stop.first); 
        AddSettingText(text_stop_under, true, false, pos, data, svg::NoneColor);
        document.Add(text_stop_under);
        
        svg::Text text_stop;
        AddSettingText(text_stop, false, false, pos, data, "black"s);
        document.Add(text_stop);          
    }
    
    document.Render(out);
}
    
}
