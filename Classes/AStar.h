﻿/****************************************************************************
 Copyright (c) 2015 Zhangpanyi

 Created by Zhangpanyi on 2015

 zhangpanyi@live.com
 ****************************************************************************/
#pragma once

#include <deque>
#include <vector>
#include <functional>
#include "BlockAllocator.h"

#define NOTEXIST		0
#define IN_OPENLIST		1
#define IN_CLOSELIST	2

/**
 * 格子位置
 */
struct Grid
{
	int row;
	int col;

	Grid() : row(0), col(0) {}
	Grid(int nRow, int nCol) : row(nRow), col(nCol) {}

	bool operator== (const Grid &incoming) const
	{
		return row == incoming.row && col == incoming.col;
	}
};

/**
 * 查询函数
 */
typedef std::function<bool(const Grid&)> QueryCallBack;

/**
 * A*算法参数定义
 */
struct AStarDef
{
	bool			allow_corner;
	int				row;
	int				col;
	Grid			start;
	Grid			end;
	QueryCallBack	reach;

	AStarDef() : row(0), col(0), reach(nullptr), allow_corner(false) {}
};

class AStar
{
public:
	/**
	 * 格子结点
	 * 记录g值、h值、和父节点信息
	 * 使用小对象分配器分配内存
	 */
	struct Node
	{
		int		g;
		int		h;
		Node*	parent;
		Grid	pos;

		int f() const
		{
			return g + h;
		}

		Node(const Grid &p) : g(0), h(0), pos(p), parent(nullptr) {}

		void* operator new(std::size_t size)
		{
			void *ptr = BlockAllocator::GetInstance()->Allocate(size);
			return ptr;
		}

		void operator delete(void* p) throw()
		{
			if (p) BlockAllocator::GetInstance()->Free(p, sizeof(Node));
		}
	};

	/**
	 * 保存节点位于开启列表或者关闭列表的信息
	 * 以及与之对应的节点指针信息
	 */
	struct NodeState
	{
		char	state;
		Node*	ptr;
		NodeState() : ptr(nullptr), state(NOTEXIST) {};
	};

public:
	AStar();
	~AStar();

public:
	/**
	 * 执行A*搜索
	 * @ 参数 def A*算法参数定义
	 * @ 返回搜索路径
	 */
	std::deque<Grid> Search(const AStarDef &def);

private:
	/**
	 * 清理
	 */
	void Clear();

	/**
	 * 初始化
	 * @ 参数 def A*算法参数定义
	 */
	void Init(const AStarDef &def);

	/**
	 * 检测 A*算法参数定义是否有效
	 * @ 参数 def A*算法参数定义
	 */
	bool ValidAStarDef(const AStarDef &def);

private:
	/**
	 * 格子是否存在于开启列表
	 * @ 参数 Grid 格子位置
	 * @ 存在返回格子结点的指针，不存在返回nullptr
	 */
	Node* IsExistInOpenList(const Grid &grid);

	/**
	 * 格子是否存在于关闭列表
	 * @ 参数 Grid 格子位置
	 * @ 存在返回true，不存在返回false
	 */
	bool IsExistInCloseList(const Grid &grid);

	/**
	 * 查询格子是否可通行
	 * @ 参数 target 目标格
	 * @ 成功返回true，失败返回false
	 */
	bool IsCanReach(const Grid &target);

	/**
	 * 查询格子是否可到达
	 * @ 参数 current 当前格位置, target 目标格位置, allowCorner 是否允许斜走
	 * @ 成功返回true，失败返回false
	 */
	bool IsCanReached(const Grid &current, const Grid &target, bool allowCorner);

	/**
	 * 搜索可通行的格子
	 * @ 参数 surround 存放搜索结果的数组, current 当前格, allowCorner 是否允许斜走
	 */
	void SearchCanReached(std::vector<Grid> &surround, const Grid &current, bool allowCorner);

	/**
	 * 计算G值
	 * @ 参数 parent 父节点, current 当前格
	 */
	int CalculG(Node *parent, const Grid &current);

	/**
	 * 计算H值
	 * @ 参数 current 当前格, end 终点格
	 */
	int CalculH(const Grid &current, const Grid &end);

	/**
	 * 获取节点在开启列表中的索引值
	 * @ 失败返回-1
	 */
	int GetIndex(Node *node);

	/**
	 * 开启列表上滤(二叉堆上滤)
	 * @ 参数 hole 上滤位置
	 */
	void PercolateUp(int hole);

private:
	/**
	 * 当节点存在于开启列表中的处理函数
	 */
	void FoundNode(Node *current_grid, Node *new_grid);

	/**
	 * 当节点不存在于开启列表中的处理函数
	 */
	void NotFoundNode(Node *current_grid, Node *new_grid, const Grid &end);

private:
	int					num_row_;			// 地图行数
	int					num_col_;			// 地图列数
	int					num_map_size_;		// 节点地图大小
	NodeState*			map_index_;			// 地图索引
	QueryCallBack		query_func_;		// 查询函数
	std::vector<Node*>	open_list_;			// 开启列表
};