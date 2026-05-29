#include<iostream>

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

// 경로 $(SolutionDir)mysql-connector-c++\mysql-connector-c++-8.0.33-winx64\include
// 경로 $(SolutionDir)mysql-connector-c++\mysql-connector-c++-8.0.33-winx64\lib64\vs14

int main()
{
	//회원의 아이디 및 비밀번호
	std::string UserID = "muhamad";
	std::string UserPW = "al";

	try
	{
		sql::Driver *MyDriver; //workbench
		sql::Connection *MyConnection; //접속정보
		//sql::Statement* MyStatement;//쿼리 창
		sql::ResultSet* MyResultSet; //결과 정보
		sql::PreparedStatement* MyPreparedStatement; //쿼리를만들때 injection방어

		MyDriver = get_driver_instance();
		MyConnection = MyDriver->connect("tcp://127.0.0.1", "ninja", "root12349");//내 sql 정보

		MyConnection->setSchema("membership");
		//MyStatement = MyConnection->createStatement();

		//sql::SQLString Query="select * from user where `user_id`= ' '";
		//Query = Query + UserID+ "' and `user_pw` = sha2(' ";
		//Query = Query + UserPW+ " ' , 512) and is_delete = 'N';";
		//MyResultSet = MyStatement->executeQuery(Query);

		sql::SQLString Query = "select * from user where `user_id` = ? and `user_pw` = sha2( ?, 512) and is_delete = 'Y';";

		MyPreparedStatement = MyConnection->prepareStatement(Query);
		MyPreparedStatement->setString(1, UserID);
		MyPreparedStatement->setString(2, UserPW);
		MyResultSet=MyPreparedStatement->executeQuery();

		std::cout << Query << std::endl;

		if (MyResultSet->getRow() != 0)
		{
			std::cout << "id or pw wrong";
		}
		else
		{
			//key 설정
			for (; MyResultSet->next();)
			{
				std::cout << MyResultSet->getInt("idx") << std::endl;
				std::cout << MyResultSet->getString("user_id") << std::endl;
				std::cout << MyResultSet->getString("user_pw") << std::endl;
				std::cout << MyResultSet->getString("name") << std::endl;
				std::cout << MyResultSet->getString("is_delete") << std::endl;
				std::cout << MyResultSet->getString("created_at") << std::endl;
			}

		}
	}
	catch(sql::SQLException Exception)
	{
		std::cout << Exception.what() << std::endl;
		std::cout << Exception.getSQLState() << std::endl;
	}
	return 0;
}