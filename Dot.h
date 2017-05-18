#pragma once

struct CDot {
	int x;
	int y;
	CDot(int _x, int _y);
	CDot();
	bool operator < (const CDot other) const;
};