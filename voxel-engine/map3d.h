#pragma once
#include "stdafx.h"

class Point {
public:
	Point(void) : x(0), y(0), z(0) {};
	Point(int x, int y, int z) : x(x), y(y), z(z) {};
	Point(int i, int size) : x(i / (size*size)), y((i / size) % size), z(i % size) {}

	int x, y, z;

	inline int oneDIndex(const int size)
	{
		return(x*size*size + y*size + z);
	}

	inline double distance(Point p)
	{
		double deltaX = this->x - p.x;
		double deltaY = this->y - p.y;
		double deltaZ = this->z - p.z;
		double dist = (deltaX*deltaX) + (deltaY*deltaY) + (deltaZ*deltaZ);
		return(dist);
	}
	inline Point normal()
	{
		int magnitude = (int)sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		magnitude = magnitude > 0 ? magnitude : 1;
		return Point(this->x / magnitude, this->y / magnitude, this->z / magnitude);
	}
	inline Point operator+(const Point &p) const
	{
		Point point = Point(this->x + p.x, this->y + p.y, this->z + p.z);
		return(point);
	}
	inline Point operator-(const Point &p) const
	{
		Point point = Point(this->x - p.x, this->y - p.y, this->z - p.z);
		return(point);
	}
	inline Point operator*(const Point &p) const
	{
		Point point = Point(this->x * p.x, this->y * p.y, this->z * p.z);
		return(point);
	}
	inline Point operator*(const int &p) const
	{
		Point point = Point(this->x * p, this->y * p, this->z * p);
		return(point);
	}
	inline Point operator/(const Point &p) const
	{
		Point point = Point(this->x / p.x, this->y / p.y, this->z / p.z);
		return(point);
	}
	inline bool operator==(const Point &p) const
	{
		return(this->x == p.x && this->y == p.y && this->z == p.z);
	}
};

struct hashFunc {
	size_t operator()(const Point &k) const {
		size_t h1 = std::hash<int>()(k.x);
		size_t h2 = std::hash<int>()(k.y);
		size_t h3 = std::hash<int>()(k.z);
		return (h1 ^ (h2 << 1)) ^ h3;
	}
};

struct equalsFunc {
	bool operator()(const Point& lhs, const Point& rhs) const {
		return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
	}
};