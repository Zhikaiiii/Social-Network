#include "Graph.h"
#include "Data.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include<random>
#include <string>
#define maxIter	10

using namespace std;
myGraph::myGraph(GraphType type, int N)
{
	GraphMatrix = new int*[N];
	VertexNum = N;
	EdgeNum = 0;
	Type = type;
	Ver = new Vertex[N];
	for (int i = 0; i < N; i++)
	{
		GraphMatrix[i] = new int[N];
		Ver[i].Num = i;
		Ver[i].Degree = 0;
		for (int j = 0; j < N; j++)
			GraphMatrix[i][j] = 0;
	}
}
void myGraph::ResetGraph()
{
	EdgeNum = 0;
	for (int i = 0; i < VertexNum; i++)
	{
		GraphMatrix[i] = new int[VertexNum];
		for (int j = 0; j < VertexNum; j++)
			GraphMatrix[i][j] = 0;
	}
}
void myGraph::Generate_ER(float p)
{
	if (Type != ER)
		return;
	else
	{
		Degree = 0;
		srand(time(NULL));
		for (int i = 0; i < VertexNum; i++)
		{
			for (int j = i + 1; j < VertexNum; j++)
			{
				int Rand1 = rand() % 10000;
				double Random_prob = Rand1 / double(10000);
				if (Random_prob < p)
				{
					GraphMatrix[i][j] = 1;
					GraphMatrix[j][i] = 1;
					Ver[i].Degree++;
					Ver[j].Degree++;
					EdgeNum++;
				}
			}
		}
			//return myGraph;
	}
}
void myGraph::Generate_SFnet(int n,int mlinks)
{
	default_random_engine random(time(NULL));
	//uniform_int_distribution<int> dis1(0, N - 1);
	uniform_real_distribution<double> dis2(0.0, 1.0);
	if (Type != SFNet)
		return;
	else
	{
		srand(time(NULL));
		//产生seed 表示主要结点
		int pos = n-1;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < i; j++)
			{
				GraphMatrix[i][j] = 1;
				GraphMatrix[j][i] = 1;
				Ver[i].Degree++;
				Ver[j].Degree++;
				EdgeNum++;
			}
			GraphMatrix[i][i] = 0;
		}
		while (pos < VertexNum-1)
		{
			pos++;
			int nlinks = 0;//表示已经有的边
			int RandomNode;
			int Deg = 0;//随机结点的度

			//每个结点产生mlinks条边
			while (nlinks < mlinks)
			{
				RandomNode = rand() % (pos);
				Deg = 0;
				for (int i = 0; i < pos; i++)
				{
					Deg += GraphMatrix[i][RandomNode];
				}
				//double Random_prob = (rand() % (10000)) / double(10000);
				double Random_prob = dis2(random);
				if (Random_prob < Deg / (EdgeNum*1.0) && GraphMatrix[pos][RandomNode] == 0 && GraphMatrix[RandomNode][pos] == 0)
				{
					GraphMatrix[pos][RandomNode] = 1;
					GraphMatrix[RandomNode][pos] = 1;
					Ver[pos].Degree++;
					Ver[RandomNode].Degree++;
					EdgeNum++;
					nlinks++;
				}
			}

		}
	}
}
void myGraph::Generate_RandReg(int Deg)
{
	
	if (Type != RandReg)
		return;
	else if ((VertexNum*Deg) % 2 == 1)
	{
		cout<< "createRandRegGraph input err : n * d must be even!"<<endl;
		return;
	}
	else
	{
		Degree = Deg;
		//srand(time(NULL));
		int * Array;
		Array=append(VertexNum, Deg);//0-(n-1)复制n次
		int Itertime = 1;
		int count = 0;
		int Pos1, Pos2;//随机选中的两个位置
		int Node1, Node2;//两个位置对应的结点
		int Total = VertexNum * Deg;
		int Length = Total;
		while (2 * EdgeNum < Total && Itertime < maxIter)
		{
			Pos1 = rand() % (Length);
			Pos2 = rand() % (Length);
			Node1 = Array[Pos1];
			Node2 = Array[Pos2];
			if (Node1 == Node2 || GraphMatrix[Node1][Node2] == 1)
			{
				if (2 * EdgeNum == Total)//生成失败
				{
					Itertime++;
					Array = append(VertexNum, Deg);
					Length = Total;
					EdgeNum = 0;
					ResetGraph();
				}
			}
			else
			{
				GraphMatrix[Node1][Node2] = 1;
				GraphMatrix[Node2][Node1] = 1;
				Ver[Node1].Degree++;
				Ver[Node2].Degree++;
				int Max = Pos1 > Pos2 ? Pos1 : Pos2;
				int Min = Pos1 <= Pos2 ? Pos1 : Pos2;
				remove(Array, Max, Max + 1, Length);//删除Array的该元素并减小长度
				Length--;
				remove(Array, Min, Min + 1, Length);
				Length--;
				EdgeNum++;

			}
		}
	}
}
void myGraph::GraphChange()
{
	int length = VertexNum;
	//cout << length << endl;
	
	for (int i = 0; i < length; i++)
	{
		int count = 0;
		for (int j = 0; j < length; j++)
		{
			if (GraphMatrix[i][j])
			{
				GraphMatrix[i][count] = j;
				//cout << GraphMatrix[i][count] << " ";
				count++;
			}
		}
		while (count < length)
		{
			GraphMatrix[i][count] = -1;
			//cout << GraphMatrix[i][count] << " ";
			count++;
		}
		//cout << endl;
	}
}
void myGraph::RandomGraphOrder()//主要用于SFNet
{
	int ** Temp = new int*[VertexNum];
	for (int i = 0; i < VertexNum; i++)
	{
		Temp[i] = new int[VertexNum];
		for (int j = 0; j < VertexNum; j++)
			Temp[i][j] = GraphMatrix[i][j];
	}
	srand(time(NULL));
	int *RandArray;
	RandArray = RandPerm(VertexNum);
	for (int i = 0; i < VertexNum; i++)
	{
		int k = 0;
		while (Temp[i][k] >= 0)
		{
			GraphMatrix[RandArray[i]][k] = RandArray[Temp[i][k]];
			k++;
		}
		Ver[RandArray[i]].Degree = k;
	}
	mysql_query(&myCont, "START TRANSACTION");
	string Insert = "Insert Into  edgesdata(sourceid, attributes, targetid, size) Values('";
	for (int i = 0; i < VertexNum; i++)
	{
		int k = 0;
		while (GraphMatrix[i][k] >= 0)
		{
			if (i < GraphMatrix[i][k])
			{
				string Temp = Insert + to_string(i) + "', 'whatever' ,'" + to_string(GraphMatrix[i][k]) + "',1);";
				mysql_query(&myCont, Temp.c_str());
			}
			k++;
		}
	}
	for (int i = 0; i < VertexNum; i++)
	{
		delete[] Temp[i];
	}
	delete[] Temp;
	mysql_query(&myCont, "COMMIT");
}
int * RandPerm(int N)
{
	{
		srand(unsigned(time(0)));
		int * Array = new int[N];
		for (int i = 0; i < N; i++)
		{
			Array[i] = i;
		}
		int temp;
		for (int i = N-1; i >=0 ; i--)
		{
			int RandomIndex = rand() % (i+1);
			temp = Array[i];
			Array[i] = Array[RandomIndex];
			Array[RandomIndex] = temp;
		}
		return Array;
	}
}
int * append(int n, int k)
{
	int *A = new int[n*k];
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A[i*n + j] = j;
		}
	}
	return A;
}
void remove(int* A, int low, int high, int length)
{
	if (low == high)return;
	while (high < length)
		A[low++] = A[high++];
}


