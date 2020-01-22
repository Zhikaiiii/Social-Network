#include "Update.h"
#include "Graph.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <string>
#include "Data.h"
using namespace std;

float fitness(myGraph Graph, bool * Strategy, float uff, float ufn, float unn, float alpha, int index)
{
	float fitness = 0;
	int count = 0;
	if (Strategy[index])
	{
		while (Graph.GraphMatrix[index][count]>=0)
		{
			if (Strategy[Graph.GraphMatrix[index][count]])
				fitness += uff;
			else
				fitness += ufn;
			count++;
		//	cout << fitness << endl;
		//	cout << Graph.GraphMatrix[index][count] << endl;
		}
	}
	else
	{
		while (Graph.GraphMatrix[index][count]>=0)
		{
			if (Strategy[Graph.GraphMatrix[index][count]])
				fitness += ufn;
			else
				fitness += unn;
			count++;
		//	cout << fitness << endl;
		//	cout << Graph.GraphMatrix[index][count] << endl;
		}
	}
	fitness = 1 - alpha + alpha * fitness;
	//cout << fitness << endl;
	return fitness;
}

int Friend_Num(myGraph Graph, int index)
{
	int i = 0;
	if (Graph.getType() == RandReg)
	{
		return Graph.getDegree();
	}
	else
	{
		while (Graph.GraphMatrix[index][i] >= 0)
		{
			i++;
		}
		return i;
	}
}

float * BD_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph)
{
	default_random_engine random(time(NULL));
	uniform_int_distribution<int> dis1(0, N - 1);
	uniform_real_distribution<double> dis2(0.0, 1.0);
	bool * Strategy = new bool[N];
	float * Fitness = new float[N];//每个个体的fitness
	float * Chosen_Probability = new float[N];//选中概率
	float * Result = new float[iterate_time];//最终结果
	//srand(unsigned(time(0)));
	Result[0] = p_ini;
	int init_Num = N * p_ini;
	//策略初始化
	mysql_query(&myCont, "START TRANSACTION");
	int length = int(sqrt(N));
	int gap = 1500 / length;
	for (int i = 0; i < N; i++)
	{
		Strategy[i] = false;
		int x, y;//均匀设置x，y 坐标
		y = -750 + gap * (i / length);
		if (y % 2 == 0)
			x = -750 + gap * (i%length);
		else
			x = -750 + gap * (i%length+1);
;		string Insert = "Insert Into  nodesdata(id, label, color, x, y, size) Values('";
		Insert = Insert + to_string(i) + "','" + to_string(i) + "', '#4f19c7' ," + to_string(x) +
		","	+ to_string(y) + "," + to_string(Graph.getIndexDegree(i)) + ");";
		mysql_query(&myCont, Insert.c_str());
	}
	int i = 0;
	//随机选取sf策略用户
	int Random_user;
	while (i < init_Num)
	{
		
		Random_user= (rand() % N ) ;
		if (Strategy[Random_user] == false)
		{
			Strategy[Random_user] = true;
			//cout << Random_user << endl;
			string Update = "UPDATE  nodesdata SET color = '#c71969' where Id =";
			Update= Update + to_string(Random_user) + ";";
			mysql_query(&myCont, Update.c_str());
			i++;
		}
	}
	//计算fitness
	for (int j = 0; j < N; j++)
	{
		Fitness[j] = fitness(Graph, Strategy, uff, ufn, unn, alpha, j);
		//cout << j << " " << Fitness[j] << endl;
	}
	//迭代过程
	int Count = 1;
	int Flag = 1;
	while (Count <= iterate_time)
	{
		//每次迭代包含N次更新
		for (int p = 0; p < N; p++)
		{

			if (Flag)
			{
				Chosen_Probability[0] = Fitness[0];
				for (int k = 1; k < N; k++)
				{
					Chosen_Probability[k] = Chosen_Probability[k - 1] + Fitness[k];
				}
				for (int k = 0; k < N; k++)
				{
					Chosen_Probability[k] = Chosen_Probability[k] / Chosen_Probability[N - 1];
					//cout << Chosen_Probability[k] << endl;

				}
			}
			//依概率随机选取一名用户

			int Chosen_index;
			//double Random_prob = (rand() % (10000)) / double(10000);
			//cout << Random_prob << endl;
			double Random_prob = dis2(random);
			for (int k = 0; k < N; k++)
			{
				if (Chosen_Probability[k] >= Random_prob)
				{
					Chosen_index = k;
					//cout << Chosen_index << endl;
					break;
				}
			}

			int friend_num = Friend_Num(Graph, Chosen_index);
			if (friend_num == 0)//如果没有邻居
			{
				Flag = 0;
				continue;
			}
			int Chosen_neighbor = Graph.GraphMatrix[Chosen_index][rand() % friend_num];//选中一个邻居
			//cout << Chosen_neighbor;
			if (Strategy[Chosen_index] != Strategy[Chosen_neighbor])
			{
				Strategy[Chosen_neighbor] = Strategy[Chosen_index];
				string Update;
				if(Strategy[Chosen_index])
					Update = "UPDATE  nodesdata SET color = '#c71969' where Id = '";
				else
					Update = "UPDATE  nodesdata SET color = '#4f19c7' where Id = '";
				Update = Update + to_string(Chosen_neighbor) + "';";
				mysql_query(&myCont, Update.c_str());
				//更新健康度
				Fitness[Chosen_neighbor] = fitness(Graph, Strategy, uff, ufn, unn, alpha, Chosen_neighbor);
				int i = 0;
				int Neighbor_friend;
				while (Graph.GraphMatrix[Chosen_neighbor][i]>=0)
				{
					Neighbor_friend = Graph.GraphMatrix[Chosen_neighbor][i];
					Fitness[Neighbor_friend] = fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor_friend);
					i++;
				}
				Flag = 1;
			}
			else
				Flag = 0;
			//cout <<"Round"<< p<< Flag << endl;
		}
		int Sum = 0;
		for (int i = 0; i < N; i++)
		{
			if (Strategy[i])
				Sum++;
		}
		Result[Count] = double(Sum) / double(N);
		//cout << Result[Count];
		Count++;
	}
	mysql_query(&myCont, "COMMIT");
	delete[] Strategy;
	delete[] Fitness;
	delete[] Chosen_Probability;
	return Result;
}

float * DB_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph)
{
	default_random_engine random(time(NULL));
	uniform_int_distribution<int> dis1(0, N - 1);
	uniform_real_distribution<double> dis2(0.0, 1.0);
	bool * Strategy = new bool[N];
	float * Result = new float[iterate_time];//最终结果
	//srand(unsigned(time(0)));
	Result[0] = p_ini;
	//策略初始化
	int length = int(sqrt(N));
	int gap = 1500 / length;
	for (int i = 0; i < N; i++)
	{
		Strategy[i] = false;
		int x, y;//均匀设置x，y 坐标
		y = -750 + gap * (i / length);
		if (y % 2 == 0)
			x = -750 + gap * (i%length);
		else
			x = -750 + gap * (i%length + 1);
		string Insert = "Insert Into  nodesdata(id, label, color, x, y, size) Values('";
		Insert = Insert + to_string(i) + "','" + to_string(i) + "', '#4f19c7' ," + to_string(x) +
			"," + to_string(y) + "," + to_string(Graph.getIndexDegree(i)) + ");";
		mysql_query(&myCont, Insert.c_str());
	}
	int i = 0;
	//随机选取sf策略用户
	int init_Num = N * p_ini;//sf用户个数
	int Random_user;
	while (i < init_Num)
	{
		Random_user = (rand() % N);
		if (Strategy[Random_user] == false)
		{
			Strategy[Random_user] = true;
			//cout << Random_user << endl;
			string Update = "UPDATE  nodesdata SET color = '#c71969' where Id =";
			Update = Update + to_string(Random_user) + ";";
			mysql_query(&myCont, Update.c_str());
			i++;
		}
	}
	//迭代过程
	int Count = 1;
	while (Count <= iterate_time)
	{
		for (int p = 0; p < N; p++)
		{
			//int Select_user = rand() % N;
			int Select_user = dis1(random);
			float Fit_f = 0;
			float Fit_n = 0;
			float Judge;
			int friend_Num = Friend_Num(Graph, Select_user);
			int Neighbor;
			if (Strategy[Select_user])//为sf
			{
				for (int i = 0; i < friend_Num; i++)
				{
					Neighbor = Graph.GraphMatrix[Select_user][i];
					if (Strategy[Neighbor])
						Fit_f += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
					else
						Fit_n += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
				}
				Judge = Fit_n / (Fit_f + Fit_n);
				//double Random_prob = (rand() % (10000)) / double(10000);
				double Random_prob = dis2(random);
				if (Random_prob <= Judge)
				{
					Strategy[Select_user] = false;
					string Update;
					Update = "UPDATE  nodesdata SET color = '#4f19c7' where Id = '";
					Update = Update + to_string(Select_user) + "';";
					mysql_query(&myCont, Update.c_str());
				}
			}
			else
			{
				for (int i = 0; i < friend_Num; i++)
				{
					Neighbor = Graph.GraphMatrix[Select_user][i];
					if (Strategy[Neighbor])
						Fit_f += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
					else
						Fit_n += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
				}
				Judge = Fit_f / (Fit_f + Fit_n);
				//double Random_prob = (rand() % (10000)) / double(10000);
				double Random_prob = dis2(random);
				if (Random_prob <= Judge)
				{
					Strategy[Select_user] = true;
					string Update;
					Update = "UPDATE  nodesdata SET color = '#c71969' where Id = '";
					Update = Update + to_string(Select_user) + "';";
					mysql_query(&myCont, Update.c_str());
				}
			}
		}
		int Sum = 0;
		for (int i = 0; i < N; i++)
		{
			if (Strategy[i])
				Sum++;
		}
		Result[Count] = double(Sum) / double(N);
		//cout << Result[Count];
		Count++;
	}
	delete[] Strategy;
	return Result;
}

float * IM_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph)
{
	default_random_engine random(time(NULL));
	uniform_int_distribution<int> dis1(0, N-1);
	uniform_real_distribution<double> dis2(0.0, 1.0);
	bool * Strategy = new bool[N];
	float * Result = new float[iterate_time];//最终结果
	Result[0] = p_ini;
	//srand(unsigned(time(0)));
	//策略初始化
	mysql_query(&myCont, "START TRANSACTION");
	int length = int(sqrt(N));
	int gap = 1500 / length;
	for (int i = 0; i < N; i++)
	{
		Strategy[i] = false;
		int x, y;//均匀设置x，y 坐标
		y = -750 + gap * (i / length);
		if (y % 2 == 0)
			x = -750 + gap * (i%length);
		else
			x = -750 + gap * (i%length + 1);
		string Insert = "Insert Into  nodesdata(id, label, color, x, y, size) Values('";
		Insert = Insert + to_string(i) + "','" + to_string(i) + "', '#4f19c7' ," + to_string(x) +
			"," + to_string(y) + "," + to_string(Graph.getIndexDegree(i)) + ");";
		mysql_query(&myCont, Insert.c_str());
	}
	int i = 0;
	//随机选取sf策略用户
	int init_Num = N * p_ini;//sf用户个数
	int Random_user;
	while (i < init_Num)
	{
		
		Random_user =  dis1(random);
		if (Strategy[Random_user] == false)
		{
			Strategy[Random_user] = true;
			//cout << Random_user << endl;
			string Update = "UPDATE  nodesdata SET color = '#c71969' where Id =";
			Update = Update + to_string(Random_user) + ";";
			mysql_query(&myCont, Update.c_str());
			i++;
		}
	}
	//迭代过程
	int Count = 1;
	while (Count <= iterate_time)
	{
		for (int p = 0; p < N; p++)
		{
			//int Select_user = rand() % N;
			int Select_user = dis1(random);
			//cout << Select_user << endl;
			float Fit_f = 0;
			float Fit_n = 0;
			float Fit_self = fitness(Graph, Strategy, uff, ufn, unn, alpha, Select_user);
			float Judge;
			int friend_Num = Friend_Num(Graph, Select_user);
			int Neighbor;
			if (Strategy[Select_user])//为sf
			{
				for (int j = 0; j < friend_Num; j++)
				{
					Neighbor = Graph.GraphMatrix[Select_user][j];
					if (Strategy[Neighbor])
						Fit_f += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
					else
						Fit_n += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
				//	cout << Fit_f << Fit_n << endl;
				}
				Judge = Fit_n / (Fit_f + Fit_n + Fit_self);
				//double Random_prob = (rand() % (10000)) / double(10000);
				double Random_prob = dis2(random);
				if (Random_prob <= Judge)
				{
					Strategy[Select_user] = false;
					string Update;
					Update = "UPDATE  nodesdata SET color = '#4f19c7' where Id = '";
					Update = Update + to_string(Select_user) + "';";
					mysql_query(&myCont, Update.c_str());
				}
				//cout << Fit_n << " " << Fit_f << " " << Fit_self << " " << Judge << " " << Random_prob << endl;
			}
			else
			{
				for (int j = 0; j < friend_Num; j++)
				{
					Neighbor = Graph.GraphMatrix[Select_user][j];
					if (Strategy[Neighbor])
						Fit_f += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
					else
						Fit_n += fitness(Graph, Strategy, uff, ufn, unn, alpha, Neighbor);
				//	cout << Fit_f << Fit_n << endl;
				}
				Judge = Fit_f / (Fit_f + Fit_n + Fit_self);
				//double Random_prob = (rand() % (10000)) / double(10000);
				double Random_prob = dis2(random);
				if (Random_prob <= Judge)
				{
					Strategy[Select_user] = true;
					string Update;
					Update = "UPDATE  nodesdata SET color = '#c71969' where id = '";
					Update = Update + to_string(Select_user) + "';";
					mysql_query(&myCont, Update.c_str());
				}
			}
		}
		int Sum = 0;
		for (int i = 0; i < N; i++)
		{
			if (Strategy[i])
				Sum++;
		}
		Result[Count] = double(Sum) / double(N);
		//cout << Result[Count];
		Count++;
	}
	delete[] Strategy;
	mysql_query(&myCont, "COMMIT");
	return Result;
}

float * Update(int UpdateType, float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph)
{
	float * Result = new float[iterate_time];//最终结果
	switch (UpdateType)
	{
	case 1:
		Result = BD_Update(uff, ufn, unn, alpha, iterate_time, N, p_ini, Graph);
		break;
	case 2:
		Result = DB_Update(uff, ufn, unn, alpha, iterate_time, N, p_ini, Graph);
		break;
	case 3:
		Result = IM_Update(uff, ufn, unn, alpha, iterate_time, N, p_ini, Graph);
		break;
	default:
		break;
	}
	return Result;
}

void PrintInfo(int UpdateType, int GraphType, float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini)
{
	cout << "UpdateType: ";
	switch (UpdateType)
	{
	case 1:
		cout << "BD";
		break;
	case 2:
		cout << "DB";
		break;
	case 3:
		cout << "IM";
		break;
	default:
		break;
	}
	cout << "    GraphType:";
	switch (GraphType)
	{
	case 1:
		cout << "ER Graph";
		break;
	case 2:
		cout << "Uniform-Degree Graph";
		break;
	case 3:
		cout << "Scale-Free NetWork";
		break;
	default:
		break;
	}
	cout << "  Payoff Matrix:";
	cout << "    uff:" << uff << "     ufn:" << ufn << "    unn:" << unn << endl;
	cout << "Selection Intensity:" << alpha;
	cout << "   Interate Time:" << iterate_time << "   Num:" << N;
	cout << "    p Initial:" << p_ini<< endl;

}