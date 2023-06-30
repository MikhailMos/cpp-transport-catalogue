#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}
 
void OstreamColorPrinter::operator()(std::monostate) {
    os << "none"sv;
}
void OstreamColorPrinter::operator()(std::string str) {
    os << str;
}
void OstreamColorPrinter::operator()(svg::Rgb rgb) {
    os << "rgb("sv << std::to_string(rgb.red) << ","sv << std::to_string(rgb.green) << ","sv << std::to_string(rgb.blue) << ")"sv;
}
void OstreamColorPrinter::operator()(svg::Rgba rgba) {
    os << "rgba("sv << std::to_string(rgba.red) << ","sv << std::to_string(rgba.green) << ","sv << std::to_string(rgba.blue) << ","sv << rgba.opacity << ")"sv;
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
    out << "r=\""sv << radius_ << "\""sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Polyline ------------------
Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool is_first = true;
    
    for (const Point& p : points_) {
        if (!is_first) { out << " "sv; }
        out << p.x << ","sv << p.y;
        if (is_first) { is_first = false; }
    }
    
    out << "\""sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Text ------------------
Text& Text::SetPosition(Point pos) {
    pos_ = std::move(pos);
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = std::move(offset);
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
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

void Text::RenderObject(const RenderContext& context) const {
    //font: [font - style || font - variant || font - weight] font - size[/ line - height] font - family | inherit
    
    auto& out = context.out;
    
    out << "<text"sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
    out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
    out << "font-size=\""sv << font_size_ << "\""sv;
    if (!font_family_.empty()) {
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\""sv << font_weight_ << "\""sv;
    }
    out << ">"sv;
    out << data_;
    out << "</text>"sv;
}

// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

    for (auto& ptr_obj : objects_) {
        RenderContext ctx(out, 2, 2);
        ptr_obj->Render(ctx);
    }
    
    out << "</svg>"sv;
}

// ---------- operator<< ------------------

std::ostream& operator<<(std::ostream& os, const StrokeLineCap& str_linecap) {

    switch (str_linecap) {
    case StrokeLineCap::BUTT:
        os << "butt";
        break;
    case StrokeLineCap::ROUND:
        os << "round";
        break;
    case StrokeLineCap::SQUARE:
        os << "square";
        break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& str_linejoin) {
    switch (str_linejoin) {
    case StrokeLineJoin::ARCS:
        os << "arcs";
        break;
    case StrokeLineJoin::BEVEL:
        os << "bevel";
        break;
    case StrokeLineJoin::MITER:
        os << "miter";
        break;
    case StrokeLineJoin::MITER_CLIP:
        os << "miter-clip";
        break;
    case StrokeLineJoin::ROUND:
        os << "round";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Color& color) {
    std::visit(svg::OstreamColorPrinter{ os }, color);
    return os;
}


}  // namespace svg