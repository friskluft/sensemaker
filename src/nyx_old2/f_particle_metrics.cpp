#define _USE_MATH_DEFINES	// For M_PI, etc.
#include <cmath>
#include <memory>
#include <unordered_map>
#include <unordered_set> 
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <array>
#include "rotation.h"
#include "sensemaker.h"


#ifndef __unix
#define NOMINMAX	// Prevent converting std::min(), max(), ... into macros
#include<windows.h>
#endif

ParticleMetrics::ParticleMetrics (std::vector<Pixel2>& _convex_hull) :
		convex_hull(_convex_hull)
{
}

void ParticleMetrics::calc_ferret(
	// output:
	double & minFeretDiameter,
	double & minFeretAngle,
	double & maxFeretDiameter,
	double & maxFeretAngle,
	std::vector<double> & all_D)
{
	// Rotated convex hull
	std::vector<Pixel2> CH_rot;
	CH_rot.reserve (convex_hull.size());

	// Rotate and calculate the diameter
	all_D.clear();
	for (float theta = 0.f; theta < 180.f; theta += rot_angle_increment)
	{
		Rotation::rotate_around_center (convex_hull, theta, CH_rot);
		auto [minX, minY, maxX, maxY] = AABB::from_pixelcloud (CH_rot);

		//
		std::vector<float> DA;	// Diameters at this angle

		// Iterate y-grid
		float stepY = (maxY - minY) / float(NY);
		for (int iy = 1; iy <= NY; iy++)
		{
			float chord_y = minY + iy * stepY;

			// Find convex hull segments intersecting 'y'
			std::vector<std::pair<float, float>> X;	// intersection points
			for (int iH = 1; iH < CH_rot.size(); iH++)
			{
				// The convex hull points are guaranteed to be consecutive
				auto& a = CH_rot[iH - 1],
					& b = CH_rot[iH];

				// Chord's Y is between segment AB's Ys ?
				if ((a.y >= chord_y && b.y <= chord_y) || (b.y >= chord_y && a.y <= chord_y))
				{
					auto chord_x = b.y != a.y ? 
						(b.x - a.x) * (chord_y - a.y) / (b.y - a.y) + a.x  
						: (b.y + a.y) / 2;	
					auto tup = std::make_pair(chord_x, chord_y);
					X.push_back(tup);
				}
			}

			// Save the length of this chord. There must be 2 items in 'chordEnds' because we don't allow uniformative chords of zero length
			if (X.size() >= 2)
			{
				// for N segments
				auto compareFunc = [](const std::pair<float, float>& p1, const std::pair<float, float>& p2) { return p1.first < p2.first; };
				auto idx_minX = std::distance(X.begin(), std::min_element(X.begin(), X.end(), compareFunc));	
				auto idx_maxX = std::distance(X.begin(), std::max_element(X.begin(), X.end(), compareFunc));	
				// left X and right X segments
				auto &e1 = X[idx_minX], &e2 = X[idx_maxX];
				auto x1 = e1.first, y1 = e1.second, x2 = e2.first, y2 = e2.second;
				// save this chord
				auto dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);	// Squared distance
				DA.push_back(dist);
			}
		}

		if (DA.size() > 0)
		{
			// Find the shortest and longest chords (diameters)
			double minD2 = *std::min_element (DA.begin(), DA.end()),
				maxD2 = *std::max_element (DA.begin(), DA.end()), 
				min_ = sqrt(minD2), 
				max_ = sqrt(maxD2);

			// Save them
			all_D.push_back(min_);
			all_D.push_back(max_);
		}
	}

	// Check if we have a degenerate case
	if (all_D.size() > 0)
	{
		// Min and max
		auto itr_min_d = std::min_element(all_D.begin(), all_D.end());
		auto itr_max_d = std::max_element(all_D.begin(), all_D.end());
		minFeretDiameter = *itr_min_d;
		maxFeretDiameter = *itr_max_d;

		// Angles
		auto idxMin = std::distance(all_D.begin(), itr_min_d);
		minFeretAngle = (double)idxMin / 2;
		auto idxMax = std::distance(all_D.begin(), itr_max_d);
		maxFeretAngle = (double)idxMax / 2;
	}
	else
	{
		minFeretDiameter =
		maxFeretDiameter =
		minFeretAngle =
		maxFeretAngle = 0.0;
	}

}

// D - diameters at angles 0..180 degrees
void ParticleMetrics::calc_martin (std::vector<double> & D)
{
	// Calculate unrotated convex hull's area in 'area'
	auto [minX, minY, maxX, maxY] = AABB::from_pixelcloud(convex_hull);

	float stepY = (maxY - minY) / float(NY);
	float halfArea = 0;
	for (int iy = 1; iy <= NY; iy++)
	{
		float chord_y = minY + iy * stepY;

		// Find convex hull segments intersecting 'y'
		std::vector<std::pair<float, float>> X;	// intersection points
		for (int iH = 1; iH < convex_hull.size(); iH++)
		{
			// The convex hull points are guaranteed to be consecutive
			auto& a = convex_hull[iH - 1],
				& b = convex_hull[iH];

			// Chord's Y is between segment AB's Ys ?
			if ((a.y >= chord_y && b.y <= chord_y) || (b.y >= chord_y && a.y <= chord_y))
			{
				auto chord_x = b.y != a.y ?
					(b.x - a.x) * (chord_y - a.y) / (b.y - a.y) + a.x
					: (b.y + a.y) / 2;
				auto tup = std::make_pair(chord_x, chord_y);
				X.push_back(tup);
			}
		}

		// Save the length of this chord. There must be 2 items in 'chordEnds'
		if (X.size() >= 2)
		{
			// for N segments
			auto compareFunc = [](const std::pair<float, float>& p1, const std::pair<float, float>& p2) { return p1.first < p2.first; };
			auto idx_minX = std::distance(X.begin(), std::min_element(X.begin(), X.end(), compareFunc));	//get_min_x_vertex(chordEnds)
			auto idx_maxX = std::distance(X.begin(), std::max_element(X.begin(), X.end(), compareFunc));	//get_max_x_vertex(IP);
			// left X and right X segments
			auto& e1 = X[idx_minX], & e2 = X[idx_maxX];
			auto x1 = e1.first, y1 = e1.second, x2 = e2.first, y2 = e2.second;
			// save this chord
			auto dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);	// Squared distance
			halfArea += sqrt(dist);
		}
	}
	halfArea /= 2;

	// Rotated convex hull
	std::vector<Pixel2> CH_rot;	
	CH_rot.reserve (convex_hull.size());

	// Rotate and calculate the diameter
	D.clear();	
	for (float theta = 0.f; theta < 180.f; theta += rot_angle_increment)
	{
		Rotation::rotate_around_center (convex_hull, theta, CH_rot);
		auto [minX, minY, maxX, maxY] = AABB::from_pixelcloud(CH_rot);

		float runSumArea = 0.f;
		std::vector<float> distFromHalfarea;
		std::vector<float> chordLengths;

		// Iterate y-grid
		float stepY = (maxY - minY) / float(NY);
		for (int iy = 1; iy <= NY; iy++)
		{
			float chord_y = minY + iy * stepY;
			
			// Find convex hull segments intersecting 'y'
			std::vector<std::pair<float, float>> X;
			for (int iH=1; iH<CH_rot.size(); iH++)
			{
				// The convex hull points are guaranteed to be consecutive
				auto& a = CH_rot [iH - 1],
					& b = CH_rot [iH];
				
				// Look for chords whose Y is between segment AB's Ys. Expecting 1 such a chord, or 2 total number of ends
				if ((a.y >= chord_y && b.y <= chord_y) || (b.y >= chord_y && a.y <= chord_y))	
				{
					auto chord_x = b.y != a.y ?
						(b.x - a.x) * (chord_y - a.y) / (b.y - a.y) + a.x
						: (b.y + a.y) / 2;
					auto tup = std::make_pair (chord_x, chord_y);
					X.push_back (tup);
				}
			}

			// Save the length of this chord. There must be 2 items in 'chordEnds'
			if (X.size() >= 2)
			{
				// for N segments
				auto compareFunc = [](const std::pair<float, float>& p1, const std::pair<float, float>& p2) { return p1.first < p2.first; };
				auto idx_minX = std::distance(X.begin(), std::min_element(X.begin(), X.end(), compareFunc));	//get_min_x_vertex(chordEnds)
				auto idx_maxX = std::distance(X.begin(), std::max_element(X.begin(), X.end(), compareFunc));	//get_max_x_vertex(IP);
				// left X and right X segments
				auto& e1 = X[idx_minX], & e2 = X[idx_maxX];
				auto x1 = e1.first, y1 = e1.second, x2 = e2.first, y2 = e2.second;

				// save this chord
				auto dist2 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);	// Squared distance
				auto chlen = sqrt(dist2);
				runSumArea += sqrt(chlen);
				distFromHalfarea.push_back (std::abs(runSumArea - halfArea));
				chordLengths.push_back(chlen);
			}
		} 

		if (distFromHalfarea.size() > 0)
		{
			// Find the chord index cutting the rotated convex hull's chunk closest to its half-area
			auto itMin = min_element(distFromHalfarea.begin(), distFromHalfarea.end());
			auto idx = std::distance(distFromHalfarea.begin(), itMin);
			auto martinD = chordLengths[idx];

			// Save them
			D.push_back(sqrt(martinD));
		}
	}
}

void ParticleMetrics::calc_nassenstein (std::vector<double>& all_D)
{
	// Rotated convex hull
	std::vector<Pixel2> CH_rot;
	CH_rot.reserve(convex_hull.size());

	// Rotate and calculate the diameter
	all_D.clear();
	for (float theta = 0.f; theta < 180.f; theta += rot_angle_increment)
	{
		Rotation::rotate_around_center (convex_hull, theta, CH_rot);
		auto [minX, minY, maxX, maxY] = AABB::from_pixelcloud(CH_rot);

		//
		std::vector<float> DA;	// Diameters at this angle

		// Iterate y-grid
		float stepY = (maxY - minY) / float(NY);
		for (int iy = 1; iy <= NY; iy++)
		{
			float chord_y = minY + iy * stepY;

			// Find convex hull segments intersecting 'y'
			std::vector<std::pair<float, float>> X;	// intersection points
			for (int iH = 1; iH < CH_rot.size(); iH++)
			{
				// The convex hull points are guaranteed to be consecutive
				auto& a = CH_rot[iH - 1],
					& b = CH_rot[iH];

				// Chord's Y is between segment AB's Ys ?
				if ((a.y >= chord_y && b.y <= chord_y) || (b.y >= chord_y && a.y <= chord_y))
				{
					auto chord_x = b.y != a.y ?
						(b.x - a.x) * (chord_y - a.y) / (b.y - a.y) + a.x
						: (b.y + a.y) / 2;
					auto tup = std::make_pair(chord_x, chord_y);
					X.push_back(tup);
				}
			}

			// Save the length of this chord. There must be 2 items in 'chordEnds' because we don't allow uniformative chords of zero length
			if (X.size() >= 2)
			{
				// for N segments
				auto compareFunc = [](const std::pair<float, float>& p1, const std::pair<float, float>& p2) { return p1.first < p2.first; };
				auto idx_minX = std::distance(X.begin(), std::min_element(X.begin(), X.end(), compareFunc));	
				auto idx_maxX = std::distance(X.begin(), std::max_element(X.begin(), X.end(), compareFunc));	
				// left X and right X segments
				auto& e1 = X[idx_minX], & e2 = X[idx_maxX];
				auto x1 = e1.first, y1 = e1.second, x2 = e2.first, y2 = e2.second;
				// save this chord
				auto dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);	// Squared distance
				DA.push_back(dist);
			}
		}

		if (DA.size() > 0)
		{
			// Find the shortest and longest chords (diameters)
			double minD2 = *std::min_element(DA.begin(), DA.end()),
				maxD2 = *std::max_element(DA.begin(), DA.end()),
				min_ = sqrt(minD2),
				max_ = sqrt(maxD2);

			// Save them
			all_D.push_back(min_);
			all_D.push_back(max_);
		}
	}
}

