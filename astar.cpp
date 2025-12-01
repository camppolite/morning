//#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <queue>
#include "astar.h"

#define TILE_COST 1

class AStarNode;
typedef std::shared_ptr<AStarNode> AStarNodePtr;

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

		vector<point> v = { {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
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

point doAStarSearch(int startx, int starty, int endx, int endy, MAZE mz, int mz_x_len, int mz_y_len, int far_x, int far_y)
{
	std::priority_queue<AStarNodePtr> frontier;
	std::vector<AStarNodePtr> allNodes;

	auto node = AStarNode::makePtr(startx, starty, 0, nullptr);
	node->updateScore(endx, endy);
	allNodes.push_back(node);
	point pt{ -1, -1 };
	DWORD startTime = GetTickCount();
	while (true)
	{
		if (node->x == endx && node->y == endy)
		{
			vector<point> path;

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

point astar(int startx, int starty, int endx, int endy, int m, int far_x, int far_y) {
	MAZE mz;
	int mz_x_len = -1;
	int mz_y_len = -1;
	switch (m)
	{
		case NVERCUN:
		{
			mz = nvercun_maze;
			mz_x_len = 126;
			mz_y_len = 142;
			break;
		}
		case PUTUOSHAN:
		{
			mz = putuoshan_maze;
			mz_x_len = 94;
			mz_y_len = 70;
			break;
		}
		case JIANGNANYEWAI:
		{
			mz = jiangnanyewai_maze;
			mz_x_len = 158;
			mz_y_len = 118;
			break;
		}
		case DATANGJINGWAI:
		{
			mz = datangjingwai_maze;
			mz_x_len = 637;
			mz_y_len = 117;
			break;
		}
		case ZHUZIGUO:
		{
			mz = zhuziguo_maze;
			mz_x_len = 190;
			mz_y_len = 118;
			break;
		}
		case AOLAIGUO:
		{
			mz = aolaiguo_maze;
			mz_x_len = 222;
			mz_y_len = 149;
			break;
		}
		case BAOXIANGGUO:
		{
			mz = baoxiangguo_maze;
			mz_x_len = 158;
			mz_y_len = 118;
			break;
		}
		case JIANYECHENG:
		{
			mz = jianyecheng_maze;
			mz_x_len = 286;
			mz_y_len = 142;
			break;
		}
		case WUZHUANGGUAN:
		{
			mz = wuzhuangguan_maze;
			mz_x_len = 98;
			mz_y_len = 73;
			break;
		}
		case CHANGSHOUCUN:
		{
			mz = changshoucun_maze;
			mz_x_len = 158;
			mz_y_len = 208;
			break;
		}
		case XILIANGNVGUO:
		{
			mz = xiliangnvguo_maze;
			mz_x_len = 162;
			mz_y_len = 122;
			break;
		}
		case 长安酒店:
		{
			mz = changanjiudian_maze;
			mz_x_len = 66;
			mz_y_len = 42;
			break;
		}
		//default:
		//	break;
	}
	if (mz_x_len != -1)
		return doAStarSearch(startx, starty, endx, endy, mz, mz_x_len, mz_y_len, far_x, far_y);

	return { -1, -1 };
}

char* astar_test() {
	static char ret[10] = "100,100";
	return ret;
}