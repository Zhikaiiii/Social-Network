#include <iostream>
#include <stdlib.h>
#include <time.h>
#include<random>
#include <iomanip>
#include "Update.h"
#include"Graph.h"
#include "Data.h"
#include <string>
#include <cstring>

using namespace std;

int main()
{
	//数据库初始化
	mysql_init(&myCont);
	if (mysql_real_connect(&myCont, host, user, pswd, database, port, NULL, 0))
	{
		mysql_select_db(&myCont, database);
		mysql_query(&myCont, "SET NAMES GBK");
		if (!mysql_query(&myCont, "CREATE TABLE IF NOT EXISTS nodesdata(\
		id VARCHAR(10) NOT NULL , label VARCHAR(10) NOT NULL, color VARCHAR(10) NOT NULL,\
		x FLOAT NOT NULL, y FLOAT NOT NULL, size INT NOT NULL, PRIMARY KEY(id )) ENGINE = MYISAM; "))
		{
			cout << "Table nodesdata was created successfully";
			mysql_query(&myCont, "DELETE FROM nodesdata");
		}
		if (!mysql_query(&myCont, "CREATE TABLE IF NOT EXISTS edgesdata(\
		sourceid VARCHAR(10) NOT NULL , attributes VARCHAR(10) NOT NULL, targetid VARCHAR(10) NOT NULL,\
		size INT NOT NULL); "))
		{
			cout << "Table edgesdata was created successfully";
			mysql_query(&myCont, "DELETE FROM edgesdata");
		}
		if (!mysql_query(&myCont, "CREATE TABLE IF NOT EXISTS Result(\
		Num INT NOT NULL, Result FLOAT NOT NULL, PRIMARY KEY(Num )); "))
		{
			cout << "Table Result was created successfully";
			mysql_query(&myCont, "DELETE FROM Result");
		}
	}
	//读取参数表
	MYSQL_RES *res;//声明存储结构
	int Col;
	MYSQL_ROW row;
	while (true)
	{
		mysql_query(&myCont, "Select * FROM indexdatabase");

		res = mysql_store_result(&myCont);
		Col = mysql_num_fields(res);
		row = mysql_fetch_row(res);
		int flag = atoi(row[1]);
		if (flag == 1)
		{
			mysql_query(&myCont, "Update indexdatabase Set flag = 0 ");
			break;
		}
	}
	for (int i = 0; i < Col; i++)
	{
		cout << row[i] << " ";
	}
	int network_construction = atoi(row[2]);
	int network_size = atoi(row[3]);
	int average_degree = atoi(row[4]);
	float uff = atof(row[5]);
	float ufn = atof(row[6]);
	float unn = atof(row[7]);
	int iterate_time = atoi(row[8]);
	float selection_intensity = atof(row[9]);
	int update_rule = atoi(row[10]);
	float p_ini = atof(row[11]);
	//产生网络
	srand(unsigned(time(0)));
	myGraph network(GraphType(network_construction), network_size);//产生网络结构
	switch (network_construction)
	{
	case 0:
	{
		double percentage = double(average_degree) / double(network_size);
		network.Generate_ER(percentage);
		break;
	}
	case 1:
		network.Generate_RandReg(average_degree);
		break;
	case 2:
	{
		int primeNode = average_degree / 2 + 1;
		network.Generate_SFnet(primeNode, average_degree / 2);
		break;
	}
	default:
		break;
	}
	network.GraphChange();
	network.RandomGraphOrder();
	float *Result2=new float[iterate_time];
	for (int i = 0; i < iterate_time; i++)
	{
		Result2[i] = 0;
	}
	for (int j = 0; j < 24; j++)
	{
		float *Result1;	
		Result1 = Update(update_rule, uff, ufn, unn, selection_intensity, iterate_time, network_size, p_ini, network);
		PrintInfo(update_rule, network_construction, uff, ufn, unn, selection_intensity, iterate_time, network_size, p_ini);
		for (int i = 0; i < iterate_time; i++)
		{
			cout << fixed << setprecision(3)<<  Result1[i]<<" ";
			if (i % 5 == 4)
			{
				cout << endl;
			}
			Result2[i] += Result1[i];
		}
		cout << endl;
	}
	for (int i = 0; i < iterate_time; i++)
	{
		cout << Result2[i]/24<<" ";
		string Insert = "Insert Into  Result(Num,Result) Values(";
		Insert = Insert + to_string(i) + ","+to_string(Result2[i]/24)+");";
		mysql_query(&myCont, Insert.c_str());
	}
	return 0;
}