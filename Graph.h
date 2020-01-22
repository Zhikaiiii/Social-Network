#pragma once
#ifndef _GRAPH_H
#define _GRAPH_H
enum GraphType {ER,RandReg,SFNet};//���ͼ������� ER, d-regular undirected graph��scale-free
class Vertex {
public:
	int Num;//ÿ�����ı��
	int Degree;//ÿ�����Ķ���
	Vertex() :Num(0), Degree(0) {}
};
class myGraph
{
public:
	myGraph(GraphType type, int N);//���캯��
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
	int ** GraphMatrix;//�ڽӾ���
	void ResetGraph();
private:
	int VertexNum;//������Ŀ
	int EdgeNum;//����
	int Degree;//ƽ������
	GraphType Type;//����
	Vertex* Ver;
};

int* append(int n, int k);

void remove(int* A, int low, int high, int length);

int* RandPerm(int N);

#endif