#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include "mswsock.h"
#include "iostream"

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#pragma pack(push,1)
class CursorInfo
{
public:
	CursorInfo() :m_posX(0), m_posY(0), m_data(0)
	{
	}
	unsigned short m_posX;
	unsigned short m_posY;
	unsigned char m_data;
};

class Packet
{
public:
	enum
	{
		e_pixel = 1,		// Client to server. Draw a pixel.
		e_line,				// Client to server. Draw a line.
		e_box,				// Client to server. Draw a box.
		e_circle,			// Client to server. Draw a circle.
		e_clientAnnounce,	// Client to server. Client announces that it exists. Server responds with Server Info packet containing window size.
		e_clientCursor,		// Client to server. Current cursor position sent to the server. Server responds with Server Cursors
		e_serverInfo,		// Server to client. Contains server window resolution.
		e_serverCursors		// Server to client. Contains an array of every client cursor value.
	};
	int type;
};

class PacketPixel : public Packet
{
public:
	int x;
	int y;
	float r;
	float g;
	float b;
};

class PacketBox : public Packet
{
public:
	int x;
	int y;
	int w;
	int h;
	float r;
	float g;
	float b;
};

class PacketLine : public Packet
{
public:
	int x1;
	int y1;
	int x2;
	int y2;
	float r;
	float g;
	float b;
};

class PacketCircle : public Packet
{
public:
	int x;
	int y;
	int radius;
	float r;
	float g;
	float b;
};

class PacketClientCursor : public Packet
{
public:
	CursorInfo cursor;
};

class PacketClientAnnounce : public Packet
{
public:

};

class PacketServerInfo : public Packet
{
public:
	unsigned short width;
	unsigned short height;
};

class PacketServerCursors : public Packet
{
public:
	unsigned short count;
	CursorInfo cursor[1];
};

#pragma pack(pop)


int main()
{
	//init
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup failed." << endl;
		return 1;
	}

	//creating the sending address
	sockaddr_in send_address;
	send_address.sin_family = AF_INET;
	send_address.sin_port = htons(1300);
	send_address.sin_addr.s_addr = inet_addr("10.40.60.248");

	//creating recieving address
	sockaddr_in rec_address;

	//creating the socket
	SOCKET send_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (send_socket == SOCKET_ERROR)
	{
		cout << "Error Opening socket: Error " << WSAGetLastError();
		return 1;
	}

	//creating rec socket
	SOCKET rec_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (rec_socket == SOCKET_ERROR)
	{
		cout << "Error Opening socket: Error " << WSAGetLastError();
		return 1;
	}

	//setting the socket to be broadcastable
	BOOL bOptVal = TRUE;
	setsockopt(send_socket,
		SOL_SOCKET,
		SO_BROADCAST,
		(const char *)&bOptVal,
		sizeof(BOOL));

	//making it not sucks
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	WSAIoctl(send_socket,
		SIO_UDP_CONNRESET,
		&bNewBehavior,
		sizeof(bNewBehavior),
		NULL,
		0,
		&dwBytesReturned,
		NULL,
		NULL);
	
	//creating the announcement
	PacketClientAnnounce pca;
	pca.type = Packet::e_clientAnnounce;
	sendto(send_socket, (char *)&pca, sizeof(PacketClientAnnounce), 0, (SOCKADDR*)&send_address, sizeof(send_address));

	//recieving
	char buffer[1000];
	int lenght = sizeof(rec_address);
	int result = recvfrom(send_socket, buffer, 1000, 0, (SOCKADDR*)&rec_address, &lenght);

	PacketServerInfo* pci;
	pci = (PacketServerInfo*)buffer;
	if (pci->type == Packet::e_serverInfo)
	{
		cout << pci->width << " " << pci->height << " " << std::endl;
	}

	//making something --- DO IT BELOW THIS POINT
	PacketBox pb;
	pb.type = Packet::e_box;
	pb.x = 5;
	pb.y = 5;
	pb.w = 5;
	pb.h = 5;
	pb.r = 1;
	pb.g = 1;
	pb.b = 1;

	sendto(send_socket, (char *)&pb, sizeof(PacketBox), 0, (SOCKADDR*)&send_address, sizeof(send_address));

	return 0;

}