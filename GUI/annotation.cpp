#include "annotation.h"

namespace Annotation
{
    IAnnotation::IAnnotation(AnnotationType t_type):
        type_(t_type),
        width_(1),
        color_{255, 255, 255}
    {

    }

    IAnnotation::~IAnnotation()
    {

    }

    void IAnnotation::setWidth(int t_width)
    {
        width_ = t_width;
    }

    void IAnnotation::setColor(int t_r, int t_g, int t_b)
    {
        color_.red = t_r;
        color_.green = t_g;
        color_.blue = t_b;
    }

    void IAnnotation::setColor(ColorType t_color)
    {
        color_ = t_color;
    }

    int IAnnotation::width()
    {
        return width_;
    }

    ColorType Annotation::IAnnotation::color()
    {
        return color_;
    }

    void IAnnotation::color(int &t_r, int &t_g, int &t_b)
    {
        t_r = color_.red;
        t_g = color_.green;
        t_b = color_.blue;
    }

    Line::Line(double t_startX, double t_startY, double t_endX, double t_endY):
        IAnnotation (AnnotationType::LINE),
        startX_(t_startX),
        startY_(t_startY),
        endX_(t_endX),
        endY_(t_endY)
    {

    }

    Dot::Dot(double t_x, double t_y):
        IAnnotation (AnnotationType::DOT),
        x_(t_x),
        y_(t_y)
    {

    }

    Circle::Circle(double t_x, double t_y, int t_radius):
        IAnnotation (AnnotationType::CIRCLE),
        x_(t_x),
        y_(t_y),
        radius_(t_radius)
    {

    }

    Label::Label(std::string t_text, double t_x, double t_y):
        IAnnotation (AnnotationType::LABEL),
        text_(t_text),
        x_(t_x),
        y_(t_y)
    {

    }


}
