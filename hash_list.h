#pragma once
#include "stdafx.h"
#include <unordered_map>

///���˼·
/*
1�����ݽṹ��unordered_map + ˫������unordered_map��valueΪ˫�������node;
˫������β�����롢ɾ��ָ��nodeʱ�临�Ӷȶ�Ϊo(1)��
2��������ʱ�临�Ӷ����£�������ߡ����ߡ���ѯ��Ϊo(1);
��ҳʱ����˫������,���ǵ���Ҿ����鿴ǰ��ҳ���ݣ�ʵ�ʺ��ٳ���o(n)���Ӷ�;
��ʹ��Ҳ鿴�������ݣ�˫������Ҳ���Դ�β����ǰ������
*/

///������ʵ�ֶ�����.h�ļ�����ڲ鿴��

//�����Ϣ�ڵ�
struct PlayerNode
{
	int playerId;         //key
	int value;            //simple data
	PlayerNode* pPre;     //ǰ�ڵ�
	PlayerNode* pNext;    //��ڵ�

	PlayerNode(int id, int v) : playerId(id), value(v), pPre(nullptr), pNext(nullptr) {}
};

//˫������  
struct PlayerList
{
	PlayerNode* pHead;
	PlayerNode* pTail;

	PlayerList() : pHead(nullptr), pTail(nullptr) {}

	//β������
	void AddTail(PlayerNode* pNode)
	{
		if (nullptr == pNode)
		{
			return;
		}

		//�����
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

	//ɾ��node
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
		else           //pNodeΪhead
		{
			pHead = pNode->pNext;
		}

		if (nullptr != pNode->pNext)
		{
			pNode->pNext->pPre = pNode->pPre;
		}
		else          //pNodeΪtail
		{
			pTail = pNode->pPre;
		}
	}
};

//�����Ϣ
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