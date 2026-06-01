#include "DBManager.h"
#include <sstream>
#include <iomanip>

DBManager::DBManager():driver(nullptr),conn(nullptr)
{
}

DBManager::~DBManager()
{
	delete conn;
}



// 바이너리 → hex 문자열 변환
std::string toHexString(const std::string& input) {
	std::ostringstream oss;
	for (unsigned char c : input) {
		oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
	}
	return oss.str();
}
bool DBManager::Connect(const std::string& host, const std::string& userid, const std::string& userpw, const std::string& database)
{

	try 
	{
		driver = get_driver_instance();
		conn = driver->connect("tcp://127.0.0.1", "ninja", "root12349");
		conn->setSchema("membership");
		std::cout << "db connect!" << std::endl;
		return true;
	}
	catch(sql::SQLException e)
	{
		std::cout << "error: "<< e.what() << std::endl;
		return false;
	}
}


bool DBManager::RegisterUser(const std::string username, const std::string& userid, const std::string& hashpassword)
{
	std::string safePw = toHexString(hashpassword);
	if (UserExists(userid)) return false;

	try
	{
		sql::PreparedStatement* pstmt = conn->prepareStatement("INSERT INTO user (user_id, user_pw , name) VALUES(?, ?, ? )");
		pstmt->setString(1, userid);
		pstmt->setString(2, safePw);
		pstmt->setString(3, username);
		pstmt->executeUpdate();
		return true;
	}
	catch (sql::SQLException e)
	{
		std::cout << "register error: " << e.what() << std::endl;
		return false;
	}

	
}

bool DBManager::LoginUser(const std::string& userid, const std::string& hashpassword)
{
	std::string safePw = toHexString(hashpassword);
	try
	{
		sql::PreparedStatement* pstmt = conn->prepareStatement("SELECT idx from user WHERE user_id=? AND user_pw ? AND is_delete='N' ");
		pstmt->setString(1, userid);
		pstmt->setString(2, safePw);

		sql::ResultSet* res = pstmt->executeQuery();
		bool found = res->next();
		delete res; 
		delete pstmt;
		return found;
	}
	catch(sql::SQLException e)
	{
		std::cout << "login error : " << e.what() << std::endl;
		return false;
	}
}

bool DBManager::UserExists(const std::string& userid)
{
	try
	{
		sql::PreparedStatement* pstmt = conn->prepareStatement("SELECT idx  FROM user WHERE user_id= ?");
		pstmt->setString(1, userid);

		sql::ResultSet* res = pstmt->executeQuery();
		bool exists = res->next();

		delete res;
		delete pstmt;
		return exists;
	}
	catch (sql::SQLException e)
	{
		std::cout << "logout error:" << e.what() << std::endl;
		return false;
	}
}








