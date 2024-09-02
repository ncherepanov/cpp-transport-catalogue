#include "svg.h"

namespace svg {

using namespace std::literals;

// ---------- Color ------------------

void ColorPrinter::operator()(std::monostate) const {
    out << "none"sv;
}
void ColorPrinter::operator()(std::string color) const {
    out << color;
}
void ColorPrinter::operator()(Rgb rgb) const {
    out << "rgb("sv << TO_STR(rgb.red) << ',' << TO_STR(rgb.green) << ',' << TO_STR(rgb.blue) << ")"sv;
}
void ColorPrinter::operator()(Rgba rgba) const {
    out << "rgba("sv << TO_STR(rgba.red) << ',' << TO_STR(rgba.green) << ',' << TO_STR(rgba.blue) << ',' << rgba.opacity << ")"sv;
}


std::ostream& operator<<(std::ostream& out, const Color& color){
    std::visit(ColorPrinter{out}, color);
    return out;
}

// ---------- PathProps ------------------
//template <typename Owner>

// ---------- Render ------------------

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    // Делегируем вывод тега своим подклассам
    RenderObject(context);
    
    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
// <circle cx="50" cy="50" r="50" />
// ----------  Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    lines_.emplace_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    //std::ostream& out = context.out;
    auto& out = context.out;
    out << "<polyline points=\""sv;
    for (auto* it = &lines_[0]; it < &lines_[0] + lines_.size(); ++it){
        out << it->x << ","sv << it->y;
        if(it!= &lines_[0] + lines_.size() - 1)
            out << " "sv;
    }
    out << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

//<polyline points="0,100 50,25 50,75 100,0" />
//<polyline points="100,100 150,25 150,75 200,0" fill="none" stroke="black" />
// ----------  Text ------------------------

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }
 
std::string Text::Decode(std::string data) {
    const std::unordered_map <char, std::string> tab = 
        {{'"',  "&quot;"s}, {'\'', "&apos;"s}, {'`', "&apos;"s}, {'>', "&gt;"s}, {'<', "&lt"s}, {'&', "&amp;"s}};
    for (const auto& [ch, str] : tab){
        if (auto it = data.find(ch); it != std::string::npos)
            data.replace(it, 1, str);
    } 
    return data;
}
// &quot &apos &apos &apos &gt &lt &amp
    
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text "sv;
    RenderAttrs(context.out);
    out << "x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
    out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
    out << "font-size=\""sv << size_ << "\""sv;
    if (!font_family_.empty()) 
        out << " font-family=\""sv << font_family_  << "\""sv;  
    if (!font_weight_.empty()) 
        out << " font-weight=\""sv << font_weight_  << "\""sv;
    out << ">"sv << Decode(data_) << "</text>"sv;
    //out << ">"sv << data_ << "</text>"sv;
}
//<text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
// ---------- Document----------------------

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream &out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl; 
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv<< std::endl;
    RenderContext context(out, 8, 8); // out, indent_step, indent
    for (auto& object : objects_) {
        object->Render(context);
    }
    out << "</svg>"sv;
}

}
