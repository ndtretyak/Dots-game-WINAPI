#pragma once
#include <map>
#include <vector>
#include <set>
#include "Dot.h"

class CGraph {
public:
	CGraph();
	~CGraph();

	bool contains(const CDot & dot);
	std::vector< std::vector<CDot> > getCycles();
	std::vector<CDot> getDots();
	bool addDot(const CDot & dot);
	bool removeDot(const CDot & dot);
	
protected:

private:
	std::vector< std::vector<int> > adjList;
	std::map<CDot, int> dots_to_int;
	std::map<int, CDot> int_to_dots;
	std::vector< std::vector<int> > cycles;
	std::vector< std::set<int> > cyclesSet;

	int cycles_count;

	void updateGraph(const CDot & newDot);
	bool createEdgeToNeighbour(const CDot & dot, int dot_number, int horizontalDelta, int verticalDelta);
	void updateCycles();

	int add_cycle(int cycle_end, int cycle_st, const std::vector<int> & path);
	void dfs(int v, const std::vector< std::vector<int> > & graph, std::vector<int> & colors, std::vector<int> & path);
};
