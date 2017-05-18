#include "Dot.h"

bool CDot::operator<(const CDot other) const
{
	return ((this->x < other.x) || (this->x == other.x && this->y < other.y));
}

CDot::CDot(int _x, int _y) : x(_x), y(_y) { ; }

CDot::CDot() : x(0), y(0) { ; }