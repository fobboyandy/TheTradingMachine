#include "basicplot.h"

namespace BasicPlot
{
    IBasicPlot::IBasicPlot(BasicPlotType t_type):
        type_(t_type),
        width_(1),
        color_{255, 255, 255}
    {

    }

    IBasicPlot::~IBasicPlot()
    {

    }

    void IBasicPlot::setWidth(int t_width)
    {
        width_ = t_width;
    }

    void IBasicPlot::setColor(int t_r, int t_g, int t_b)
    {
        color_.red = t_r;
        color_.green = t_g;
        color_.blue = t_b;
    }

    void IBasicPlot::setColor(ColorType t_color)
    {
        color_ = t_color;
    }

    int IBasicPlot::width()
    {
        return width_;
    }

    ColorType BasicPlot::IBasicPlot::color()
    {
        return color_;
    }

    void IBasicPlot::color(int &t_r, int &t_g, int &t_b)
    {
        t_r = color_.red;
        t_g = color_.green;
        t_b = color_.blue;
    }

    Line::Line(double t_startX, double t_startY, double t_endX, double t_endY):
        IBasicPlot (BasicPlotType::LINE),
        startX_(t_startX),
        startY_(t_startY),
        endX_(t_endX),
        endY_(t_endY)
    {

    }

    Dot::Dot(double t_x, double t_y):
        IBasicPlot (BasicPlotType::DOT),
        x_(t_x),
        y_(t_y)
    {

    }

    Circle::Circle(double t_x, double t_y, int t_radius):
        IBasicPlot (BasicPlotType::CIRCLE),
        x_(t_x),
        y_(t_y),
        radius_(t_radius)
    {

    }

    Label::Label(std::string t_text, double t_x, double t_y):
        IBasicPlot (BasicPlotType::LABEL),
        text_(t_text),
        x_(t_x),
        y_(t_y)
    {

    }


}
