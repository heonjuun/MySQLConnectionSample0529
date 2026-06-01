#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include <string>
#pragma comment(lib, "ws2_32")
//#pragma comment(lib, "NetCommon")

#include "../NetCommon/protocol.h"


void RecvAndPrint(SOCKET Sock)
{
	REsponsePacket Res{};
	recv(Sock, (char*)&Res, sizeof(Res), 0);

	switch (Res.Header.type)
	{
	case PacketType::RES_SUCCESS: std::cout << "success" << Res.message << std::endl; break;
	case PacketType::RES_FAIL: std::cout << "fail" << Res.message << std::endl; break;
	case PacketType::RES_DUPLICATE: std::cout << "duplicate" << Res.message << std::endl; break;
	}
}

int main()
{
	std::cout << "client " << std::endl;

	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.0.194");
	ServerSockAddr.sin_port = htons(35000);

	//connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	std::cout << "client connect" << std::endl;

	if (connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr)) == SOCKET_ERROR)
	{
		std::cout << "connencted error"<<std::endl;
		WSACleanup();
		return -1;
	}
	std::cout << "server connect Success!"<<std::endl;
	while (true)
	{
		std::cout << "[1]로그인 [2]회원가입  [3] 로그아웃 [0] 종료 "<<std::endl;
		int choice;
		std::cin >> choice;
		if (choice == 0) break;
		if (choice == 1)
		{
			std::string id, pw;
			std::cout << "아이디 : "; std::cin >> id;
			std::cout << "비밀번호 : "; std::cin >> pw;
			LoginPacket loginpacket;
			loginpacket.header.type = PacketType::REQ_LOGIN;
			loginpacket.header.body_size = sizeof(LoginPacket) - sizeof(PacketHeader);
			strncpy_s(loginpacket.user_id, id.c_str(), 45);
			strncpy_s(loginpacket.user_pw, pw.c_str(), 64);
			send(ServerSocket, (char*)&loginpacket,sizeof(loginpacket), 0);
			RecvAndPrint(ServerSocket);
		}
		if (choice == 2)
		{
			std::string id, pw, name;
			std::cout << "아이디 : "; std::cin >> id;
			std::cout << "비밀번호 : "; std::cin >> pw;
			std::cout << "이름 : "; std::cin >> name;

			RegisterPacket regpkt{};
			regpkt.Header.type = PacketType::REQ_REGISTER;
			regpkt.Header.body_size = sizeof(RegisterPacket) - sizeof(PacketHeader);
			strncpy_s(regpkt.user_id,id.c_str(), 45);
			strncpy_s(regpkt.user_pw, pw.c_str(), 64);
			strncpy_s(regpkt.name, name.c_str(), 50);

			send(ServerSocket, (char*)&regpkt, sizeof(regpkt), 0);
			RecvAndPrint(ServerSocket);
		}
		if (choice == 3)
		{

		}
		
	}
	closesocket(ServerSocket);
	WSACleanup();
	system("pause");
	return 0;
}


//        if (choice == 3) {
//            AuthPacket pkt{};
//            pkt.header.type = PacketType::REQ_LOGOUT;
//            pkt.header.body_size = 0;
//            send(ServerSocket, (char*)&pkt, sizeof(pkt), 0);
//            recvAndPrint(ServerSocket);
//            continue;
//        }// 
//    }



