#pragma once

#include <tuple>
#include <utility>
#include <vector>
#include "pixel.h"

class AABB
{
public:
	AABB() {}
	AABB(const std::vector<Pixel2> & cloud) 
	{
		for (auto& px : cloud)
		{
			update_x(px.x);
			update_y(px.y);
		}
	}
	void init_x(StatsInt x) { xmin = xmax = x; }
	void init_y(StatsInt y) { ymin = ymax = y; }
	void update_x(StatsInt x)
	{
		xmin = std::min(xmin, x);
		xmax = std::max(xmax, x);
	}
	void update_y(StatsInt y)
	{
		ymin = std::min(ymin, y);
		ymax = std::max(ymax, y);
	}
	StatsInt get_height() const { return ymax - ymin + 1; }
	StatsInt get_width() const { return xmax - xmin + 1; }
	StatsInt get_area() { return get_width() * get_height(); }
	inline StatsInt get_xmin() { return xmin; }
	inline StatsInt get_xmax() { return xmax; }
	inline StatsInt get_ymin() { return ymin; }
	inline StatsInt get_ymax() { return ymax; }
	static std::tuple<StatsInt, StatsInt, StatsInt, StatsInt> from_pixelcloud (const std::vector<Pixel2>& P)
	{
		AABB bb;
		for (auto& p : P)
		{
			bb.update_x(p.x);
			bb.update_y(p.y);
		}
		return {bb.get_xmin(), bb.get_ymin(), bb.get_xmax(), bb.get_ymax()};
	}
protected:
	StatsInt xmin = INT32_MAX, xmax = INT32_MIN, ymin = INT32_MAX, ymax = INT32_MIN;
};