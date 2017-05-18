#include "Graph.h"
#include <algorithm>

CGraph::CGraph() : cycles_count(0) {
	cycles = std::vector<std::vector<int> > (10);
}

CGraph::~CGraph() {
}

std::vector<std::vector<CDot>> CGraph::getCycles() {
	std::vector< std::vector<CDot> > result;
	for (int i = 0; i < cycles_count; i++) {
		result.push_back(std::vector<CDot>());
		for (auto dot_number : cycles[i]) {
			result[result.size() - 1].push_back(int_to_dots[dot_number]);
		}
	}
	return result;
}

std::vector<CDot> CGraph::getDots() {
	std::vector<CDot> result;
	for (auto i : dots_to_int) {
		result.push_back(i.first);
	}
	return result;
}

bool CGraph::addDot(const CDot & dot) {
	if (contains(dot)) {
		return false;
	}
	dots_to_int[dot] = adjList.size();
	int_to_dots[adjList.size()] = dot;
	adjList.push_back(std::vector<int>());
	updateGraph(dot);
	return true;
}

bool CGraph::removeDot(const CDot & dot) {
	if (dots_to_int.count(dot) == 0) {
		return false;
	}
	int dot_number = dots_to_int[dot];
	for (auto i : adjList) {
		auto it = std::find(i.begin(), i.end(), dot_number);
		if (it != i.end()) {
			i.erase(it);
		}
	}
	dots_to_int.erase(dot);
	int_to_dots.erase(dot_number);
	return true;
}

bool CGraph::contains(const CDot & dot) {
	return dots_to_int.count(dot) > 0;
}


bool operator == (const std::set<int> & set1, const std::set<int> & set2) {
	if (set1.size() != set2.size())
		return false;
	for (auto i : set1) {
		if (set2.find(i) == set2.end())
			return false;
	}
	return true;
}

int CGraph::add_cycle(int cycle_end, int cycle_st, const std::vector<int> & path) {
	if (cycles_count >= cycles.size()) {
		cycles.resize(cycles_count * 2);
	}
	cycles[cycles_count].clear();
	cycles[cycles_count].push_back(cycle_st);
	for (int v = cycle_end; v != cycle_st; v = path[v]) {
		cycles[cycles_count].push_back(v);
	}
	cycles[cycles_count].push_back(cycle_st);
	std::reverse(cycles[cycles_count].begin(), cycles[cycles_count].end());
	std::set<int> newCycle = std::set<int>(cycles[cycles_count].begin(), cycles[cycles_count].end());
	if (std::find(cyclesSet.begin(), cyclesSet.end(), newCycle) != cyclesSet.end()) {
		return -1;
	}
	cyclesSet.push_back(std::set<int>(cycles[cycles_count].begin(), cycles[cycles_count].end()));
	return cycles[cycles_count].size();
}

void CGraph::dfs(int v, const std::vector< std::vector<int> > & graph, std::vector<int> & colors, std::vector<int> & path) {
	colors[v] = 1;
	for (int i = 0; i < graph[v].size(); i++) {
		int to = graph[v][i];
		if (colors[to] == 0) {
			path[to] = v;
			dfs(to, graph, colors, path);
		} else if (colors[to] == 1 && add_cycle(v, to, path) > 4) {
			cycles_count++;
		}
	}	
	colors[v] = 0;
}


void CGraph::updateCycles() {
	int size = adjList.size();
	std::vector<int> colors(size, 0);
	std::vector<int> path(size, 0);
	for (int i = 0; i < size; i++) {
		if (colors[i] == 0) {
			dfs(i, adjList, colors, path);
		}
	}
}

bool CGraph::createEdgeToNeighbour(const CDot & dot, int dot_number, int horizontalDelta, int verticalDelta) {
	auto neighbour = dots_to_int.find(CDot(dot.x + horizontalDelta, dot.y + verticalDelta));
	if (neighbour != dots_to_int.end()) {
		adjList[neighbour->second].push_back(dot_number);
		adjList[dot_number].push_back(neighbour->second);
		return true;
	}
	return false;
}

void CGraph::updateGraph(const CDot & newDot) {
	int verticalDeltas[] = { -1, 0, 1 };
	int horizontalDeltas[] = { -1, 0, 1 };
	for (auto i : horizontalDeltas) {
		for (auto j : verticalDeltas) {
			if (!((i == 0) && (j == 0))) {
				createEdgeToNeighbour(newDot, adjList.size() - 1, i, j);
			}
		}
	}
	updateCycles();
}
