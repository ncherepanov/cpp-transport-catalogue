#pragma once
#define _USE_MATH_DEFINES
#define TO_STR(x) std::to_string(x)
#include <cstdint>
#include <cmath>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

using namespace std::literals;

namespace svg {

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t red, uint8_t green, uint8_t blue) 
    : red(red), green(green), blue(blue){};
    
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba : public Rgb {
    Rgba() = default;
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) 
    : Rgb(red, green, blue), opacity(opacity) {};
    
    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};

struct ColorPrinter {
    void operator()(std::monostate) const;
    void operator()(std::string color) const;
    void operator()(Rgb rgb) const;
    void operator()(Rgba rgba) const;
    std::ostream& out;
};

std::ostream& operator<<(std::ostream& out, const Color& color);

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<<(std::ostream& out, StrokeLineCap cap) {
    switch (cap) {
        case StrokeLineCap::BUTT:
            out << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            out << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            out << "square"sv;
            break;
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin join) {
    switch (join) {
        case StrokeLineJoin::ARCS:
            out << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel"sv;
            break;
        case StrokeLineJoin::MITER:
            out << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip"sv;
            break;
        case StrokeLineJoin::ROUND:
            out << "round"sv;
            break;
    }
    return out;
}

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
    void RenderAttrs(std::ostream& out) const {
        if (fill_color_) {
            out << "fill=\""sv << *fill_color_ << "\" "sv;
        }
        if (stroke_color_) {
            out << "stroke=\""sv << *stroke_color_ << "\" "sv;
        }
        if (width_) {
            out << "stroke-width=\""sv << *width_ << "\" "sv;
        }        
        if (line_cap_) {
            out << "stroke-linecap=\""sv << *line_cap_ << "\" "sv;
        }        
        if (line_join_) {
            out << "stroke-linejoin=\""sv << *line_join_ << "\" "sv;
        }    
    }    

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};
    
/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};
    
/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
}; 
    
class ObjectContainer {
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    template <typename T>
    void Add(T t) {
        objects_.emplace_back(std::make_unique<T>(std::move(t)));
    }
    
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
    virtual ~ObjectContainer() = default;

protected:
    std::deque<std::unique_ptr<Object>> objects_;
};
    
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
}; 
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};
    
/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);
private:
    void RenderObject(const RenderContext& context) const override;
    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);
    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> lines_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);
    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);
    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);
    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);
    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);
    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    static std::string Decode(std::string data);
    void RenderObject(const RenderContext& context) const override;
    Point pos_;
    Point offset_;
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class Document : public ObjectContainer{
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;
    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;
    // Прочие методы и данные, необходимые для реализации класса Document
};

}  // namespace svg