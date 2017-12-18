#pragma once
#include "StdAfx.h"
#include <string>
#include "mysql.h"

class RFMYSQL {
public:
	RFMYSQL();
	~RFMYSQL();

	int Open(const char* host, const char* usr, const char* pwd, const char* db, int port);
	int Reopen();
	int Close();

	int Exec(const char* sql);

	bool IsOpen();

public:
	bool connected;
	MYSQL* _db;

	std::string m_host;
	std::string m_usr;
	std::string m_pwd;
	std::string m_db;
	int			m_port;
};

class RFMYSQLStmt {
public:
	RFMYSQLStmt();
	RFMYSQLStmt(RFMYSQL& db, const char* sql);
	~RFMYSQLStmt();

	int Prepare(RFMYSQL& db, const char *sql);
	int Step();
	int Finalize();

	int GetInt(int index);
	std::string GetString(int index);

	MYSQL_RES* _res;
	MYSQL_ROW _row;
};
