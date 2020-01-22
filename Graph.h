#pragma once
#ifndef _GRAPH_H
#define _GRAPH_H
enum GraphType {ER,RandReg,SFNet};//随机图像的总类 ER, d-regular undirected graph，scale-free
class Vertex {
public:
	int Num;//每个结点的编号
	int Degree;//每个结点的度数
	Vertex() :Num(0), Degree(0) {}
};
class myGraph
{
public:
	myGraph(GraphType type, int N);//构造函数
	void Generate_ER(float p);
	void Generate_SFnet(int n,int mlinks);
	void Generate_RandReg(int Deg);
	void GraphChange();
	void RandomGraphOrder();
	int getVertexNum() { return VertexNum; }
	int getEdgeNum() { return EdgeNum; }
	int getDegree() { return Degree; }
	int getIndexDegree(int index) { return Ver[index].Degree; }
	GraphType getType() { return Type; }
	int ** GraphMatrix;//邻接矩阵
	void ResetGraph();
private:
	int VertexNum;//顶点数目
	int EdgeNum;//边数
	int Degree;//平均度数
	GraphType Type;//类型
	Vertex* Ver;
};

int* append(int n, int k);

void remove(int* A, int low, int high, int length);

int* RandPerm(int N);

#endif