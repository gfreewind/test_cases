#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <cstring>
#include <string>

#include "socket.hpp"

using namespace std;
 
bool Socket::open(int domain, int type, int protocol, unsigned int flags) 
{
	if (flags & SOCKET_NONBLOCK_BIT) {
		sock_ = socket(domain, type | SOCK_NONBLOCK, protocol);
	} else {
		sock_ = socket(domain, type, protocol);
	}

	if (-1 == sock_) {
		cerr << "socket failed: " << strerror(errno) << endl;
		return false;
	}

	if (flags & SOCKET_REUSEADDR_BIT) {		
		int sock_opt = 1;
		if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opt, sizeof(sock_opt)) < 0) {
			cerr << "Fail to set SO_REUSEADDR" << endl;
			return false;
		}
	}

	if (flags & SOCKET_REUSEPORT_BIT) {		
		int sock_opt = 1;
		if (setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, (void*)&sock_opt, sizeof(sock_opt)) < 0) {
			cerr << "Fail to set SO_REUSEPORT" << endl;
			return false;
		}
	}

	return true;
}

bool Socket::bind(unsigned int addr,unsigned short port)
{
	struct sockaddr_in saddr;
	
	memset(&saddr, 0, sizeof(addr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(addr);
	if (-1 == ::bind(sock_, (const struct sockaddr *)&saddr, sizeof(saddr))) {
		cerr << "Fail to bind UDP socket" << endl;
		return false;
	}

	return true;
}


