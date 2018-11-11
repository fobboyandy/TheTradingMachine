#pragma once

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
		IAnnotation(AnnotationType t_type):
			type_(t_type),
			width_(1),
			color_{ 255, 255, 255 }
		{}
		virtual ~IAnnotation() {}

		void setWidth(int t_width)
		{
			width_ = t_width;
		}
		void setColor(int t_r, int t_g, int t_b)
		{
			color_.red = t_r;
			color_.green = t_g;
			color_.blue = t_b;
		}
		void setColor(ColorType t_color)
		{
			color_ = t_color;
		}
		int width()
		{
			return width_;
		}
		ColorType color()
		{
			return color_;
		}
		void color(int& t_r, int& t_g, int& t_b)
		{
			t_r = color_.red;
			t_g = color_.green;
			t_b = color_.blue;
		}

	private:
		const AnnotationType type_;
		int width_;

		// color
		ColorType color_;
	};

	struct Line : public IAnnotation
	{
		Line(double t_startX, double t_startY, double t_endX, double t_endY) :
			IAnnotation(AnnotationType::LINE),
			startX_(t_startX),
			startY_(t_startY),
			endX_(t_endX),
			endY_(t_endY)
		{}
		// plot coordinates
		const double startX_;
		const double startY_;
		const double endX_;
		const double endY_;
	};

	struct Dot : public IAnnotation
	{
		Dot(double t_x, double t_y) :
			IAnnotation(AnnotationType::DOT),
			x_(t_x),
			y_(t_y)
		{}
		const double x_;
		const double y_;
	};

	struct Circle : public IAnnotation
	{
		Circle(double t_x, double t_y, int t_radius) :
			IAnnotation(AnnotationType::CIRCLE),
			x_(t_x),
			y_(t_y),
			radius_(t_radius)
		{}
		const double x_;
		const double y_;
		const int radius_;
	};

	struct Label : public IAnnotation
	{
		Label(std::string t_text, double t_x, double t_y) :
			IAnnotation(AnnotationType::LABEL),
			text_(t_text),
			x_(t_x),
			y_(t_y)
		{}
		const std::string text_;
		const double x_;
		const double y_;
	};
}
