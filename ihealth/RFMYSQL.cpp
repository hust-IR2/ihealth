#include "StdAfx.h"
#include "RFMYSQL.h"

RFMYSQL::RFMYSQL() :
	_db(NULL),
		connected(false)
{
}

RFMYSQL::~RFMYSQL()
{
	Close();
}

int RFMYSQL::Open(const char* host, const char* usr, const char* pwd, const char* db, int port)
{
	Close();

	if (!(_db=mysql_init(0))) {
		return -1;
	}

	m_host = host;
	m_usr = usr;
	m_pwd = pwd;
	m_db = db;
	m_port = port;

	mysql_options(_db, MYSQL_READ_DEFAULT_GROUP, "connect");
	if (!mysql_real_connect(_db,host, usr, pwd, db, port,NULL,0)) {
		mysql_close(_db);
		_db = NULL;
		return -1;
	}
	
	connected = true;
	return 1;
}

int RFMYSQL::Reopen()
{
	Close();

	if (!(_db=mysql_init(0))) {
		return -1;
	}

	mysql_options(_db, MYSQL_READ_DEFAULT_GROUP, "connect");
	if (!mysql_real_connect(_db,m_host.c_str(), m_usr.c_str(), m_pwd.c_str(), m_db.c_str(), m_port,NULL,0)) {
		mysql_close(_db);
		_db = NULL;
		return -1;
	}

	connected = true;
	return 1;
}

int RFMYSQL::Close()
{
	if (NULL != _db && connected) {
		mysql_close(_db);
		_db = NULL;
	}
	connected = 0;

	return 1;
}

int RFMYSQL::Exec(const char* sql)
{
	if (mysql_real_query(_db,sql,(unsigned int) strlen(sql))) {
		return -1;
	}

	return 1;
}

bool RFMYSQL::IsOpen()
{
	if (_db) {
		return true;
	} else {
		return false;
	}
}

RFMYSQLStmt::RFMYSQLStmt() :
	_res(NULL),
		_row(NULL)
{

}

RFMYSQLStmt::RFMYSQLStmt(RFMYSQL& db, const char* sql) 
{
	Prepare(db, sql);
}

RFMYSQLStmt::~RFMYSQLStmt()
{
	Finalize();
}

int RFMYSQLStmt::Prepare(RFMYSQL& db, const char *sql)
{
	if (!db._db || !sql) {
		return -1;
	}

	if (mysql_real_query(db._db, sql, (unsigned int) strlen(sql))) {
		return -1;
	}

	_res = mysql_store_result(db._db);
	if (!_res) {
		return -1;
	}
	
	return 1;
}

int RFMYSQLStmt::Step()
{
	if (!_res) {
		return -1;
	}

	_row = mysql_fetch_row(_res);
	if (!_row) {
		return -1;
	}

	return 1;
}

int RFMYSQLStmt::Finalize()
{
	if (!_res) {
		return -1;
	}

	mysql_free_result(_res);
	_res = NULL;
	return 1;
}

int RFMYSQLStmt::GetInt(int index)
{
	if (!_res) {
		return -1;
	}

	return atoi(_row[index]);
}

std::string RFMYSQLStmt::GetString(int index)
{
	if (!_res) {
		return "";
	}

	return _row[index];
}