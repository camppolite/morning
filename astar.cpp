//#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <queue>
#include "astar.h"

#define TILE_COST 1

class AStarNode;
typedef std::shared_ptr<AStarNode> AStarNodePtr;

MAZE get_maze(unsigned int scene_id) {
	switch (scene_id)
	{
		case 女儿村:
			return nvercun_maze;
		case 普陀山:
			return putuoshan_maze;
		case 江南野外:
			return jiangnanyewai_maze;
		case 大唐境外:
			return datangjingwai_maze;
		case 朱紫国:
			return zhuziguo_maze;
		case 傲来国:
			return aolaiguo_maze;
		case 宝象国:
			return baoxiangguo_maze;
		case 建邺城:
			return jianyecheng_maze;
		case 五庄观:
			return wuzhuangguan_maze;
		case 长寿村:
			return changshoucun_maze;
		case 西梁女国:
			return xiliangnvguo_maze;
		case 长安酒店:
			return changanjiudian_maze;
		//default:
		//	break;
	}
	return MAZE();
}

POINT get_map_max_loc(unsigned int scene_id) {
	switch (scene_id)
	{
		case 女儿村:
			return { 126, 142 };
		case 普陀山:
			return { 94, 70 };
		case 江南野外:
			return { 158, 118 };
		case 大唐境外:
			return { 637, 117 };
		case 朱紫国:
			return { 190, 118 };
		case 傲来国:
			return { 222, 149 };
		case 宝象国:
			return { 158, 118 };
		case 建邺城:
			return { 286, 143 };
		case 五庄观:
			return { 98, 73 };
		case 长寿村:
			return { 158, 208 };
		case 西梁女国:
			return { 162, 122 };
		case 长安酒店:
			return { 66, 49 };
		case 长安城:
			return { 548, 279 };
		case 东海湾:
			return { 119, 119 };
		case 化生寺:
			return { 127, 95 };
		case 长安国子监:
			return { 79, 59 };
		case 地府:
			return { 131, 98 };
		case 狮驼岭:
			return { 131, 98 };
		case 建邺杂货店:
			return { 119, 119 }; //todo
		case 花果山:
			return { 119, 119 }; //todo
		case 长寿郊外:
			return { 119, 119 }; //todo
		case 傲来客栈二楼:
			return { 119, 119 }; //todo
		default:
			break;
	}
	return { -1, -1 };
}

class AStarNode
{
public:
	int x, y;
	int g, score;
	AStarNodePtr parent;

	AStarNode(int x, int y, int cost, AStarNodePtr p, int score = 0)
		: x(x), y(y), g(cost), score(score), parent(p)
	{}
	static AStarNodePtr makePtr(int x, int y, int cost, AStarNodePtr p, int score = 0)
	{
		return AStarNodePtr(new AStarNode(x, y, cost, p, score));
	}
	int heuristic(const int destx, int desty) const
	{
		int xd = destx - x;
		int yd = desty - y;
		return abs(xd) + abs(yd);
	}
	void updateScore(int endx, int endy)
	{
		auto h = this->heuristic(endx, endy);
		this->score = g + h;
	}
	AStarNodePtr getCopy()
	{
		return AStarNode::makePtr(x, y, g, parent, score);
	}
	std::vector<AStarNodePtr> getChildren(MAZE mz, int mz_x_len, int mz_y_len)
	{
		std::vector<AStarNodePtr> ret;
		auto copy = getCopy();
		//if (x > 0)
		//	ret.push_back(AStarNode::makePtr(x - 1, y, g + TILE_COST, copy));
		//if (y > 0)
		//	ret.push_back(AStarNode::makePtr(x, y - 1, g + TILE_COST, copy));
		//if (x < mz_x_len - 1)
		//	ret.push_back(AStarNode::makePtr(x + 1, y, g + TILE_COST, copy));
		//if (y < mz_y_len - 1)
		//	ret.push_back(AStarNode::makePtr(x, y + 1, g + TILE_COST, copy));

		vector<POINT> v = { {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
		for (auto a = v.begin(); a != v.end(); a++)
		{
			int node_x = x + (*a).x;
			int node_y = y + (*a).y;

			// Make sure within range
			if (node_x > mz_x_len || node_x < 0 || node_y > mz_y_len || node_y < 0)
				continue;
			// Make sure walkable terrain
			auto v_y = mz.find(node_x);
			if (v_y != mz.end()) {
				for (auto& a_y : (*v_y).second)
				{
					if (a_y.min <= node_y && node_y <= a_y.max) {
						ret.push_back(AStarNode::makePtr(node_x, node_y, g + TILE_COST, copy));
						break;
					}
				}
			}
		}
		return ret;
	}
};

bool operator<(const AStarNodePtr& a, const AStarNodePtr& b)
{
	return a->score > b->score;
}
bool operator==(const AStarNodePtr& a, const AStarNodePtr& b)
{
	return a->x == b->x && a->y == b->y;
}

POINT doAStarSearch(int startx, int starty, int endx, int endy, MAZE mz, int mz_x_len, int mz_y_len, int far_x, int far_y)
{
	std::priority_queue<AStarNodePtr> frontier;
	std::vector<AStarNodePtr> allNodes;

	auto node = AStarNode::makePtr(startx, starty, 0, nullptr);
	node->updateScore(endx, endy);
	allNodes.push_back(node);
	POINT pt{ -1, -1 };
	DWORD startTime = GetTickCount();
	while (true)
	{
		if (node->x == endx && node->y == endy)
		{
			vector<POINT> path;

			while (node.get() != nullptr)
			{
				path.push_back({ node->x, node->y });
				//printf("%d, %d\n", node->x, node->y);
				node = node->parent;
			}
			for (auto it = path.rbegin(); it != path.rend(); ++it) {
				// 寻路距离目的坐标的差距，捉鬼是12,10 宝图是x,y
				if (abs(endx - (*it).x) <= far_x && abs(endy - (*it).y) <= far_y) {
					pt = { (*it).x, (*it).y };
					break;
				}
			}
			break;
		}

		if (GetTickCount() - startTime > 4000) break;  // 超时退出

		auto children = node->getChildren(mz, mz_x_len, mz_y_len);
		for (auto c = children.begin(); c != children.end(); c++)
		{
			auto found = std::find(allNodes.rbegin(), allNodes.rend(), *c);
			if (found != allNodes.rend())
			{
				if (*found > *c)
				{
					(*found)->g = (*c)->g;
					(*found)->parent = (*c)->parent;
					(*found)->updateScore(endx, endy);
				}
			}
			else
			{
				(*c)->updateScore(endx, endy);
				frontier.push(*c);
				allNodes.push_back(*c);
			}
		}

		if (frontier.size() == 0)
			break;

		node = frontier.top();
		frontier.pop();
	}
	if (pt.x == -1 || pt.y == -1) {
		// 匹配不上坐标
		const int side = 4;
		int x_v = 1;
		int y_v = 1;
		if (endx < startx) x_v = -1;
		if (endy < starty) y_v = -1;
		pt.x = endx - side * x_v;
		pt.y = endy - side * y_v;
	}
	return pt;
}

POINT astar(int startx, int starty, int endx, int endy, int m, int far_x, int far_y) {
	MAZE mz = get_maze(m);
	POINT mz_pt = get_map_max_loc(m);
	if (mz_pt.x != -1)
		return doAStarSearch(startx, starty, endx, endy, mz, mz_pt.x, mz_pt.y, far_x, far_y);
	return { -1, -1 };
}

char* astar_test() {
	static char ret[10] = "100,100";
	return ret;
}