#pragma once
#ifndef _UPDATE_H
#define _UPDATE_H
#include "Graph.h"
 
float fitness(myGraph Graph, bool * Strategy, float uff, float ufn, float unn, float alpha, int index);

int  Friend_Num(myGraph Graph, int index);

float * BD_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph);
float * DB_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph);
float * IM_Update(float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph);
float * Update(int UpdateType, float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini, myGraph Graph);
void PrintInfo(int UpdateType,int GraphType, float uff, float ufn, float unn, float alpha, int iterate_time, int N, float p_ini);
#endif