#pragma once
#include<cstdint>

enum class PacketType : uint8_t
{
	REQ_REGISTER=0x01,
	REQ_LOGIN=0x02,
	REQ_LOGOUT=0x03,

	RES_SUCCESS=0x10,
	RES_FAIL=0x11,
	RES_DUPLICATE=0x12,
};
#pragma pack(push,1)
struct PacketHeader
{
	PacketType type;
	uint16_t body_size;
};

// 로그인 요청 (user_id, user_pw만 사용)
struct LoginPacket {
	PacketHeader header;
	char user_id[46];
	char user_pw[65];
};


struct AuthPacket
{
	PacketHeader Header;
	char user_id[46];
	char user_pw[65];
};
struct RegisterPacket
{
	PacketHeader Header;
	char name[51];
	char user_id[46];
	char user_pw[65];
	
};

struct REsponsePacket
{
	PacketHeader Header;
	char message[128];
};
#pragma pack(pop)
