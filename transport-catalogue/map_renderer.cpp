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

void MapCreator::AddSettingsToPolyline(svg::Polyline& polyline, uint32_t line_num) const {
    polyline.SetStrokeColor(settings_.color_palette_[line_num]).SetFillColor("none"s).SetStrokeWidth(settings_.line_width_).
             SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapCreator::AddLineCapJoinText(svg::Text& text) const {
    text.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapCreator::AddFontWeight(svg::Text& text, std::string str = "bold"s) {
    text.SetFontWeight(str);
}

void MapCreator::AddSettingText(svg::Text& text, bool is_underlayer, bool is_bus, svg::Point pos, 
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

void MapCreator::ConstructorPolyline() {
    for (const auto& [bus, stops] : buses_stops_) {  // проходимся по остановкам каждого маршрута
        svg::Polyline points;    //инициализируем для складывания координат х у и свойств для текущего автобуса
        for (auto stop : stops) {
            points.AddPoint(all_stop_.at(stop));   // заполняем полилайн
        }
        AddSettingsToPolyline(points, bus_color_.at(bus));   // дописываем в полилайн прочие настройки
        doc_.Add(points); // добавляем сформированный полилайн    
    }
}
    
void MapCreator::ConstructorTxtBus() {
    for (auto [bus, stop] : all_bus_) {

        svg::Text text_bus_under;
        AddLineCapJoinText(text_bus_under);
        svg::Point pos = all_stop_.at(stop);
        std::string data = std::string(bus); 
        AddSettingText(text_bus_under, true, true, pos, data, svg::NoneColor);
        AddFontWeight(text_bus_under);
        doc_.Add(text_bus_under);
        
        svg::Text text_bus;
        AddSettingText(text_bus, false, true, pos, data, settings_.color_palette_[bus_color_.at(bus)]);
        AddFontWeight(text_bus);
        doc_.Add(text_bus);        
    }
}

void MapCreator::ConstructorCircle() {
    for (auto stop : all_stop_) {
        doc_.Add(svg::Circle().SetCenter(stop.second).SetRadius(settings_.stop_radius_).SetFillColor("white"s));
    }
}

void MapCreator::ConstructorTxtStop() {
    for (auto stop : all_stop_) {

        svg::Text text_stop_under;
        AddLineCapJoinText(text_stop_under);
        svg::Point pos = stop.second;
        std::string data = std::string(stop.first); 
        AddSettingText(text_stop_under, true, false, pos, data, svg::NoneColor);
        doc_.Add(text_stop_under);
        
        svg::Text text_stop;
        AddSettingText(text_stop, false, false, pos, data, "black"s);
        doc_.Add(text_stop);          
    }
}

MapCreator::MapCreator(const Catalogue& catalogue, const RenderSettings& settings)
: catalogue_(catalogue), settings_(settings), doc_({}) {
    std::unordered_set<std::string_view, catalogue::Hasher> stops_routed;   //все остановки входящие в маршруты
    for (const auto& [bus, stops] : catalogue_.GetBusesStops()) {  
        if (!stops.empty()) {
            stops_routed.insert(stops.begin(), stops.end()); //складируем все остановки во всех маршрутах 
            buses_stops_[bus] = stops;      // маршрут - остановки для прохода с правильной сортировкой маршрута
        }
    }
    std::vector<geo::Coordinates> set_point;
    for (const auto& stop : catalogue_.GetStops()) {               
        if (stops_routed.count(stop.stop_)) {        
            set_point.emplace_back(stop.location_);   // складируем координаты тех остановок, которые есть в маршрутах
        }
    }
    // создаём объект SphereProjector для пересчёта координат в х у
    SphereProjector projector(set_point.begin(), set_point.end(), settings_.width_, settings_.height_, settings_.padding_);
    uint32_t num_color = 0; //начальное значение цветовой палитры
    for (const auto& [bus, stops] : buses_stops_) { 
        all_bus_.emplace_back(std::make_pair(bus, stops.front()));
        if (!catalogue_.GetBuses().find(bus)->roundtrip_ && stops.front() != stops[stops.size()/2]) {
            all_bus_.emplace_back(std::make_pair(bus, stops[stops.size()/2]));
        }
        for (size_t i = 0; i < stops.size(); ++i) {      // проходимся по остановкам и складируем их координаты х у
            all_stop_[stops[i]] = projector(catalogue_.GetStops().find(stops[i])->location_); // пересчитываем в х у
        }
        bus_color_[bus] = num_color;    
        num_color = num_color < settings_.color_palette_.size() - 1 ? num_color + 1 : 0; //инкремент номера цвета, цвета закончились - начинаем с нуля
    }
}

const svg::Document* MapCreator::GetDoc() {
    return &doc_;
}

MapRenderer::MapRenderer(const Catalogue& catalogue, Reader& reader) 
: catalogue_(catalogue), reader_(reader) {

    const json::Dict& map = reader.Request("render_settings"s).AsDict();

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

void MapRenderer::GetMap(std::ostream& out) const {
    
    MapCreator creator(catalogue_, settings_);
    
    creator.ConstructorPolyline();
    creator.ConstructorTxtBus();
    creator.ConstructorCircle();
    creator.ConstructorTxtStop();   
    
    creator.GetDoc()->Render(out);    
}
    
}
