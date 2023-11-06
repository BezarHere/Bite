#define WIN32_LEAN_AND_MEAN

#include "_bite_net.h"
#include "_bite_reporting.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include "_bite_str.h"
using namespace bite;

#if 0
enum class WSACallResult
{
  Successful = 0,
  
  
  FaultyDataPtr = WSAEFAULT,
  BlockingProcInProgress = WSAEINPROGRESS,
  MaxSocketsReached = WSAEPROCLIM,
  SystemNotReady = WSASYSNOTREADY,
  VersionNotSupported = WSAVERNOTSUPPORTED,
  NotInitialised = WSANOTINITIALISED,
  NetSubsystemFailure = WSAENETDOWN,
};

class WSAServerGuard
{
public:
  static constexpr WORD WSAVersion = MAKEWORD(2, 2);
  FORCEINLINE WSAServerGuard()
  {
    // no holding instances, startup
    if (!s_guard_refc)
    {
      WSACallResult res = (WSACallResult)WSAStartup(WSAVersion, &s_latest_wsadata);

      if (res != WSACallResult::Successful)
        raise(format("Failed to start WSA with code: {}", res));
    }

    s_guard_refc++;
  }

  FORCEINLINE ~WSAServerGuard()
  {
    s_guard_refc--;

    // no more holding instances, cleanup
    if (!s_guard_refc)
    {
      WSACleanup();
    }
  }

  FORCEINLINE static bool is_running()
  {
    return s_guard_refc;
  }

  FORCEINLINE static const WSADATA &get_wsa_data()
  {
    return s_latest_wsadata;
  }

private:
  static WSADATA s_latest_wsadata;
  static uint32_t s_guard_refc;
};
uint32_t WSAServerGuard::s_guard_refc = 0;

//void start_wsa()
//{
//  constexpr WORD WSAVersion = MAKEWORD(2, 2);
//	WSADATA data;
//	int res = WSAStartup(WSAVersion, &data);
//
//	if (res)
//		raise(format("Failed to start WSA with code: {}", res));
//}

void stop_wsa()
{
  constexpr WORD WSAVersion = MAKEWORD(2, 2);
	WSADATA data;
	int res = WSAStartup(WSAVersion, &data);

	if (res)
		raise(format("Failed to start WSA with code: {}", res));
}

void server_create_ms_samble()
{
  constexpr size_t ReciveBufferLen = 512;
  constexpr auto DefaultPort = "27015";

  WSADATA wsaData;
  int iResult;

  SOCKET ListenSocket = INVALID_SOCKET;
  SOCKET ClientSocket = INVALID_SOCKET;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int iSendResult;
  char recvbuf[ ReciveBufferLen ];
  int recvbuflen = ReciveBufferLen;

  // Initialize Winsock
  WSAServerGuard guard;

  memset((&hints), 0, (sizeof(hints)));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, DefaultPort, &hints, &result);
  if (iResult != 0) {
    raise(format("getaddrinfo failed with error: {}", iResult));
  }

  // Create a SOCKET for the server to listen for client connections.
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    freeaddrinfo(result);
    raise(format("socket failed with error: {}", WSAGetLastError()));
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(result);
    closesocket(ListenSocket);
    raise(format("bind failed with error: {}", WSAGetLastError()));
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    closesocket(ListenSocket);
    raise(format("listen failed with error: {}", WSAGetLastError()));
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    closesocket(ListenSocket);
    raise(format("accept failed with error: {}", WSAGetLastError()));
  }

  // No longer need server socket
  closesocket(ListenSocket);

  // Receive until the peer shuts down the connection
  do {

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      printf("Bytes received: \d\n", iResult);

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, recvbuf, iResult, 0);
      if (iSendResult == SOCKET_ERROR) {
        closesocket(ClientSocket);
        raise(format("send failed with error: {}", WSAGetLastError()));
      }
      printf("Bytes sent: %d\n", iSendResult);
    }
    else if (iResult == 0)
      printf("Connection closing...\n");
    else {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
    }

  } while (iResult > 0);

  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    closesocket(ClientSocket);
    printf("shutdown failed with error: %d\n", WSAGetLastError());
  }

  // cleanup
  closesocket(ClientSocket);
}

void test()
{
	//start_winsock();

	struct sockaddr_in server_info = { 0 };
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(1337);

	intptr_t sock = socket(AF_INET, SOCK_STREAM, 0);

	//? error
	if (sock < 0)
	{
		raise("can't create server socket");
	}

	int bind_ret = bind(sock, (struct sockaddr *)&server_info, sizeof(server_info));
	
	//? error
	if (bind_ret < 0)
	{
		raise(format("Can't bind server socket to serverinfo localhost::{}", server_info.sin_addr.S_un.S_addr));
	}

	int listen_ret = listen(sock, 0);

	//? error
	if (listen_ret < 0)
	{
		raise("Couldn't listen to socket");
	}

	struct sockaddr client_info = { 0 };
	int client_info_size = 0;

	while (true)
	{
		char data_buf[ 1024 ]{};

		std::cout << "waiting for connections" << '\n';
		intptr_t clientfd = accept(sock, &client_info, &client_info_size);

		if (clientfd < 0)
		{
			raise("Failed to accept connection!");
		}
		std::cout << "connected to " << clientfd << '\n';

		int r = recv(clientfd, data_buf, 1024, 0);

		//? error
		if (r < 0)
		{
			raise(format("Failed to recive data from {}", clientfd));
		}
		else
		{
			std::cout << "Data recived: " << data_buf << '\n';
		}
		
		int close_ret = closesocket(clientfd);

		//? error
		if (close_ret < 0)
		{
			raise(format("failed to close client {}", clientfd));
		}

	}

	int master_sock_close_ret = closesocket(sock);

	//? error
	if (master_sock_close_ret < 0)
	{
		raise(format("failed to close server {}", master_sock_close_ret));
	}

}

namespace bite
{
	namespace net
	{
		Socket::Socket()
		{
			test();
		}

	}
}
#endif