#pragma once
#include<iostream>

#include<string>
#include"jdbc/mysql_connection.h"
#include"jdbc/cppconn/driver.h"
#include"jdbc/cppconn/exception.h"
#include"jdbc/cppconn/resultset.h"
#include"jdbc/cppconn/statement.h"
#include"jdbc/cppconn/prepared_statement.h"

#ifndef _DEBUG
#pragma comment(lib, "mysqlcppconn")
#else
#pragma comment(lib, "debug/mysqlcppconn")
#endif

class DBManager
{
public:
	DBManager();
	~DBManager();

	bool Connect(const std::string &host, const std::string &userid, const std::string 
		&userpw, const std::string &database);

	bool RegisterUser(const std::string username, const std::string& userid, const std::string& hashpassword);

	bool LoginUser(const std::string& userid, const std::string& hashpassword);

	bool UserExists(const std::string &userid);
private:
	sql::Driver* driver;
	sql::Connection* conn;







};

