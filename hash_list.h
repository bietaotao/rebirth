#pragma once
#include "stdafx.h"
#include <unordered_map>

///设计思路
/*
1、数据结构：unordered_map + 双向链表，unordered_map的value为双向链表的node;
双向链表尾部插入、删除指定node时间复杂度都为o(1)。
2、各操作时间复杂度如下：玩家上线、下线、查询均为o(1);
分页时遍历双向链表,考虑到玩家经常查看前几页数据，实际很少出现o(n)复杂度;
即使玩家查看最后的数据，双向链表也可以从尾部往前遍历。
*/

///申明和实现都放在.h文件里，便于查看。

//玩家信息节点
struct PlayerNode
{
	int playerId;         //key
	int value;            //simple data
	PlayerNode* pPre;     //前节点
	PlayerNode* pNext;    //后节点

	PlayerNode(int id, int v) : playerId(id), value(v), pPre(nullptr), pNext(nullptr) {}
};

//双向链表  
struct PlayerList
{
	PlayerNode* pHead;
	PlayerNode* pTail;

	PlayerList() : pHead(nullptr), pTail(nullptr) {}

	//尾部插入
	void AddTail(PlayerNode* pNode)
	{
		if (nullptr == pNode)
		{
			return;
		}

		//链表空
		if (nullptr == pHead && nullptr == pTail)
		{
			pHead = pNode;
			pTail = pNode;
		}
		else if (nullptr == pHead || nullptr == pTail)     //error
		{
			return;
		}
		else
		{
			pTail->pNext = pNode;
			pNode->pPre = pTail;
			pTail = pNode;
		}
	}

	//删除node
	void RemoveNode(PlayerNode* pNode)
	{
		if (nullptr == pNode)
		{
			return;
		}

		if (nullptr != pNode->pPre)
		{
			pNode->pPre->pNext = pNode->pNext;
		}
		else           //pNode为head
		{
			pHead = pNode->pNext;
		}

		if (nullptr != pNode->pNext)
		{
			pNode->pNext->pPre = pNode->pPre;
		}
		else          //pNode为tail
		{
			pTail = pNode->pPre;
		}
	}
};

//玩家信息
class PlayerInfo
{
public:
	PlayerInfo() {}

	void Online(int playerId, int value)
	{
		if (infoUMap.end() == infoUMap.find(playerId))
		{
			PlayerNode* pNode = new PlayerNode(playerId, value);
			infoUMap.insert(std::make_pair(playerId, pNode));
			infoList.AddTail(pNode);
		}
	}

	void Offline(int playerId)
	{
		if (infoUMap.end() != infoUMap.find(playerId))
		{
			PlayerNode* pNode = infoUMap.find(playerId)->second;
			infoUMap.erase(playerId);
			infoList.RemoveNode(pNode);

			delete pNode;
		}
	}

	int Query(int playerId) const
	{
		if (infoUMap.end() != infoUMap.find(playerId))
		{
			return infoUMap.find(playerId)->second->value;
		}
		else
		{
			return 0;
		}
	}

	void GetPageInfo(int indexStart, int count, std::vector<PlayerNode*>& rst) const
	{
		rst.clear();
		int index = 0;
		for (PlayerNode* temp = infoList.pHead; temp != nullptr; temp = temp->pNext)
		{
			if (index >= indexStart && index < indexStart + count)
			{
				rst.push_back(temp);
			}
			
			if (index >= indexStart + count)
			{
				return;
			}

			++index;
		}
	}

//for test
public: 
	void Dump() const
	{
		printf("PlayerInfo:");
		for (PlayerNode* temp = infoList.pHead; temp != nullptr; temp = temp->pNext)
		{
			printf("%d  ", temp->playerId);
		}
		printf("\n");
	}

private:
	std::unordered_map<int, PlayerNode*> infoUMap;       //key:playerId
	PlayerList infoList;
};

//for test
static void DumpRst(int indexStart, int count, const std::vector<PlayerNode*> rst)
{
	printf("PageInfo[%d,%d]:", indexStart, count);
	for each (const PlayerNode* it in rst)
	{
		printf("%d ", it->playerId);
	}
	printf("\n");
}

static void Test()
{
	PlayerInfo info;
	for (int i = 0; i < 10; ++i)
	{
		info.Online(i, 0);
		info.Dump();
	}
	printf("\n");

	for (int i = 0; i < 10; ++i)
	{
		info.Offline(i);
		info.Dump();
	}
	printf("\n");

	//test GetPageInfo
	for (int i = 0; i < 100; ++i)
	{
		info.Online(i, 0);
	}
	std::vector<PlayerNode*> rst;
	info.GetPageInfo(0, 10, rst);
	DumpRst(0, 10, rst);

	info.GetPageInfo(20, 10, rst);
	DumpRst(20, 10, rst);

	printf("\nOffline:");
	for (int i = 5; i < 10; ++i)
	{
		info.Offline(i);
		printf("%d ", i);
	}
	printf("\n");
	info.GetPageInfo(0, 10, rst);
	DumpRst(0, 10, rst);

	info.GetPageInfo(50, 10, rst);
	DumpRst(50, 10, rst);
}