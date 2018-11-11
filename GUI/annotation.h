#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <string>

namespace Annotation
{
    enum class AnnotationType
    {
        LINE,
        DOT,
        CIRCLE,
        LABEL,
    };

    struct ColorType
    {
        // color
        int red;
        int green;
        int blue;
    };

    class IAnnotation
    {
    public:
        IAnnotation(AnnotationType t_type);
        virtual ~IAnnotation();

        void setWidth(int t_width);
        void setColor(int t_r, int t_g, int t_b);
        void setColor(ColorType t_color);
        int width();
        ColorType color();
        void color(int& t_r, int& t_g, int& t_b);

    private:
        const AnnotationType type_;
        int width_;

        // color
        ColorType color_;
    };

    struct Line : public IAnnotation
    {
        Line(double t_startX, double t_startY, double t_endX, double t_endY);
        // plot coordinates
        const double startX_;
        const double startY_;
        const double endX_;
        const double endY_;
    };

    struct Dot : public IAnnotation
    {
        Dot(double t_x, double t_y);
        const double x_;
        const double y_;
    };

    struct Circle: public IAnnotation
    {
        Circle(double t_x, double t_y, int t_radius);
        const double x_;
        const double y_;
        const int radius_;
    };

    struct Label: public IAnnotation
    {
        Label(std::string t_text, double t_x, double t_y);
        const std::string text_;
        const double x_;
        const double y_;
    };
}
#endif
