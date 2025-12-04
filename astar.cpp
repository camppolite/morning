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
	MAZE mz;
	switch (scene_id)
	{
	case 女儿村:
	{
		mz = nvercun_maze;
		break;
	}
	case 普陀山:
	{
		mz = putuoshan_maze;
		break;
	}
	case 江南野外:
	{
		mz = jiangnanyewai_maze;
		break;
	}
	case 大唐境外:
	{
		mz = datangjingwai_maze;
		break;
	}
	case 朱紫国:
	{
		mz = zhuziguo_maze;
		break;
	}
	case 傲来国:
	{
		mz = aolaiguo_maze;
		break;
	}
	case 宝象国:
	{
		mz = baoxiangguo_maze;
		break;
	}
	case 建邺城:
	{
		mz = jianyecheng_maze;
		break;
	}
	case 五庄观:
	{
		mz = wuzhuangguan_maze;
		break;
	}
	case 长寿村:
	{
		mz = changshoucun_maze;
		break;
	}
	case 西凉女国:
	{
		mz = xiliangnvguo_maze;
		break;
	}
	case 长安酒店:
	{
		mz = changanjiudian_maze;
		break;
	}
	//default:
	//	break;
	}
	return mz;
}

POINT get_map_max_loc(unsigned int scene_id) {
	POINT pos = { -1, -1 };
	switch (scene_id)
	{
	case 女儿村:
	{
		pos = { 126, 142 };
		break;
	}
	case 普陀山:
	{
		pos = { 94, 70 };
		break;
	}
	case 江南野外:
	{
		pos = { 158, 118 };
		break;
	}
	case 大唐境外:
	{
		pos = { 637, 117 };
		break;
	}
	case 朱紫国:
	{
		pos = { 190, 118 };
		break;
	}
	case 傲来国:
	{
		pos = { 222, 149 };
		break;
	}
	case 宝象国:
	{
		pos = { 158, 118 };
		break;
	}
	case 建邺城:
	{
		pos = { 286, 143 };
		break;
	}
	case 五庄观:
	{
		pos = { 98, 73 };
		break;
	}
	case 长寿村:
	{
		pos = { 158, 208 };
		break;
	}
	case 西凉女国:
	{
		pos = { 162, 122 };
		break;
	}
	case 长安酒店:
	{
		pos = { 66, 49 };
		break;
	}
	case 长安城:
	{
		pos = { 548, 279 };
		break;
	}
	default:
		break;
	}
	return pos;
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