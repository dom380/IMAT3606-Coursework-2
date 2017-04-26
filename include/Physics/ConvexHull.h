#pragma once
#ifndef CONVEXHULL_H
#define CONVEXHULL_H
#include <vector>
using std::vector;
class ConvexHull
{
public:
	vector<float> m_points;
	vector<int> m_triangles;
};
#endif // !CONVEXHULL_H