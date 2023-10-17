#include "_bite_net.h"
#include "_bite_reporting.h"
#include <sys/types.h>
#include <socketapi.h>
#include <websocket.h>
#include <winsock.h>

void test()
{
	struct sockaddr_in server_info = { AF_INET, htons(1337) };

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	//? error
	if (sock < 0)
	{
	}

	int bind_ret = bind(sock, (struct sockaddr *)&server_info, sizeof(server_info));
	
	//? error
	if (bind_ret < 0)
	{
	}

	listen(sock, 0);
	struct sockaddr client_info = { 0 };
	int client_info_size = 0;

	while (true)
	{

		SOCKET clientfd = accept(sock, &client_info, &client_info_size);
		
		//? error
		if (clientfd < 0)
		{
		}

		char data_buf[ 1024 ]{};
		int r = recv(clientfd, data_buf, 1024, 0);

		//? error
		if (r < 0)
		{
		}
		else
		{
			/// do sutff with data
		}
		
		int close_ret = closesocket(clientfd);

		//? error
		if (close_ret < 0)
		{
		}

	}

	int master_sock_close_ret = closesocket(sock);

	//? error
	if (master_sock_close_ret < 0)
	{
	}

}

namespace bite
{
	namespace net
	{



	}
}
