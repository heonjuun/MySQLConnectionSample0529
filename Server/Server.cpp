#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include"DBManager.h"
#include<map>
#include<mutex>
#include "../NetCommon/Protocol.h"

#pragma comment(lib, "ws2_32")
//#pragma comment(lib, "NetCommon")

std::map<std::string, SOCKET>Sessions;
std::mutex SessionLock;

void SendResponse(SOCKET sock, PacketType type, const char* msg)
{
	REsponsePacket res{};
	res.Header.type = type;
	res.Header.body_size = sizeof(REsponsePacket) - sizeof(PacketHeader);
	strncpy_s(res.message, msg, 127);
	send(sock, (char*)&res, sizeof(res), 0);
}
void HandleClient(SOCKET ClientSock, DBManager& DB)
{
	std::string LoggedInUser = "";
	while (true)
	{
		AuthPacket authpacket{};
		int RecvLength = recv(ClientSock, (char*)&authpacket, sizeof(authpacket), 0);
		if (RecvLength <= 0)
		{
			if (!LoggedInUser.empty())
			{
				std::lock_guard<std::mutex> lock(SessionLock);
				Sessions.erase(LoggedInUser);
				std::cout << "[server]" << LoggedInUser << "disconnect server" << std::endl;
			}
			break;
		}
		switch (authpacket.Header.type)
		{
		case PacketType::REQ_REGISTER:{
			RegisterPacket reg{};
			recv(ClientSock, (char*)&reg, sizeof(reg), 0);
			if (DB.RegisterUser(reg.user_id, reg.user_pw, reg.name))
			{
				SendResponse(ClientSock,PacketType::RES_SUCCESS,"register_Success");
			}
			else
			{
				SendResponse(ClientSock, PacketType::RES_DUPLICATE, "id DUPLICATE");
				break;
			}
			break;
		}
		case PacketType::REQ_LOGIN:{
			LoginPacket login{};
			recv(ClientSock, (char*)&login, sizeof(login), 0);
		
			std::lock_guard<std::mutex> lock(SessionLock);

			if (Sessions.count(login.user_id))
			{
				SendResponse(ClientSock,PacketType::RES_FAIL,"already loggined");
				break;
			}
			if (DB.LoginUser(login.user_id, login.user_pw))
			{
				LoggedInUser = login.user_id;
				Sessions[LoggedInUser] = ClientSock;
				SendResponse(ClientSock, PacketType::RES_SUCCESS, "Login_SUCCESS");
				
			}
			else {
				// is_delete='Y'이거나 아이디/비번 불일치
				SendResponse(ClientSock, PacketType::RES_FAIL, "Login_FAIL");
			}
			break;
		}
		case PacketType::REQ_LOGOUT:{
			if (!LoggedInUser.empty()) 
			{
				std::lock_guard<std::mutex> lock(SessionLock);
		        Sessions.erase(LoggedInUser);
		        std::cout << "[server] logout: " << LoggedInUser << "\n";
				LoggedInUser = "";
		        SendResponse(ClientSock, PacketType::RES_SUCCESS, "logout success");
		    }
		    break;
		}
		    
		}
	}
	closesocket(ClientSock);
}

int main() {
    std::cout << "server start\n";

    // DB 연결
    DBManager db;
    if (!db.Connect("tcp://127.0.0.1:3306", "root", "root12349", "membership")) {
        return -1;
    }

    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN ListenSockAddr;
    memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
    ListenSockAddr.sin_family = AF_INET;
    ListenSockAddr.sin_addr.s_addr = INADDR_ANY;
    ListenSockAddr.sin_port = htons(35000);

    ::bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
    listen(ListenSocket, SOMAXCONN);
    std::cout << "포트 35000 대기 중...\n";

    while (true) {
        SOCKET clientSock = accept(ListenSocket, nullptr, nullptr);
        std::cout << "[서버] 클라이언트 접속\n";
        // 클라이언트마다 스레드 생성
        std::thread(HandleClient, clientSock, std::ref(db)).detach();
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}