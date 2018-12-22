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

	struct IAnnotation
	{
		IAnnotation(AnnotationType t_type, int t_index):
			type_(t_type),
			width_(1),
			index_(t_index),
			color_{ 0, 0, 0 }
		{}
		virtual ~IAnnotation() {};

		const AnnotationType type_;
		int width_;
		const int index_;

		// color
		ColorType color_;
	};

	struct Line : public IAnnotation
	{
		// QCP accepts double as parameter but interprets the value as time in x axis.
		// We are using templates and static_cast to handle warnings. Types will be
		// auto deduced by the input parameters.
		template<typename T1, typename T2>
		Line(T1 t_startX, double t_startY, T2 t_endX, double t_endY, int t_index = 0) :
			IAnnotation(AnnotationType::LINE, t_index),
			startX_(static_cast<double>(t_startX)),
			startY_(t_startY),
			endX_(static_cast<double>(t_endX)),
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
		// QCP accepts double as parameter but interprets the value as time in x axis.
		// We are using templates and static_cast to handle warnings. Types will be
		// auto deduced by the input parameters.
		template<typename T1>
		Dot(T1 t_x, double t_y, int t_index = 0) :
			IAnnotation(AnnotationType::DOT, t_index),
			x_(static_cast<double>(t_x)),
			y_(t_y)
		{}
		const double x_;
		const double y_;
	};

	struct Circle : public IAnnotation
	{
		// QCP accepts double as parameter but interprets the value as time in x axis.
		// We are using templates and static_cast to handle warnings. Types will be
		// auto deduced by the input parameters.

		template<typename T>
		Circle(T t_x, double t_y, int t_radius, int t_index = 0) :
			IAnnotation(AnnotationType::CIRCLE, t_index),
			x_(static_cast<double>(t_x)),
			y_(t_y),
			radius_(t_radius)
		{}
		const double x_;
		const double y_;
		const int radius_;
	};

	struct Label : public IAnnotation
	{
		// QCP accepts double as parameter but interprets the value as time in x axis.
		// We are using templates and static_cast to handle warnings. Types will be
		// auto deduced by the input parameters.
		template<typename T>
		Label(std::string t_text, T t_x, double t_y, int t_index = 0) :
			IAnnotation(AnnotationType::LABEL, t_index),
			text_(t_text),
			x_(static_cast<double>(t_x)),
			y_(t_y)
		{}
		const std::string text_;
		const double x_;
		const double y_;
	};
}
