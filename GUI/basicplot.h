#ifndef BASICPLOT_H
#define BASICPLOT_H

#include <string>

namespace BasicPlot
{
    enum class BasicPlotType
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

    class IBasicPlot
    {
    public:
        IBasicPlot(BasicPlotType t_type);
        virtual ~IBasicPlot();

        void setWidth(int t_width);
        void setColor(int t_r, int t_g, int t_b);
        void setColor(ColorType t_color);
        int width();
        ColorType color();
        void color(int& t_r, int& t_g, int& t_b);

    private:
        const BasicPlotType type_;
        int width_;

        // color
        ColorType color_;
    };

    struct Line : public IBasicPlot
    {
        Line(double t_startX, double t_startY, double t_endX, double t_endY);
        // plot coordinates
        const double startX_;
        const double startY_;
        const double endX_;
        const double endY_;
    };

    struct Dot : public IBasicPlot
    {
        Dot(double t_x, double t_y);
        const double x_;
        const double y_;
    };

    struct Circle: public IBasicPlot
    {
        Circle(double t_x, double t_y, int t_radius);
        const double x_;
        const double y_;
        const int radius_;
    };

    struct Label: public IBasicPlot
    {
        Label(std::string t_text, double t_x, double t_y);
        const std::string text_;
        const double x_;
        const double y_;
    };
}
#endif // BASICPLOT_H
