#pragma once
#ifndef _DATA_H
#define _DATA_H
#include <Windows.h>
#include <mysql.h>
#pragma comment(lib,"D:\\mysql-8.0.17-winx64\\mysql-8.0.17-winx64\\lib\\libmysql.lib") 
extern const char user[];
extern const char pswd[];
extern const char host[];
extern const char database[]; 
//const char table[] = "test";
extern unsigned int port;
extern MYSQL myCont;

#endif