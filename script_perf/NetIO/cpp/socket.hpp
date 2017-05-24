#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <unistd.h>
#include <sys/types.h>		/* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>

struct Socket {
	Socket(): sock_(-1) {};
	~Socket() {
		if (-1 != sock_) {
			close(sock_);
		}
	}

	enum {
		SOCKET_NONBLOCK = 1,
		SOCKET_NONBLOCK_BIT = (1 << SOCKET_NONBLOCK),
		SOCKET_REUSEADDR = 2,
		SOCKET_REUSEADDR_BIT = (1 << SOCKET_REUSEADDR),
		SOCKET_REUSEPORT = 3,
		SOCKET_REUSEPORT_BIT = ( 1 << SOCKET_REUSEPORT),
	};

	enum {
		SOCKET_ANY_ADDR = INADDR_ANY,
	};

	bool open(int domain, int type, int protocol, unsigned int flags = 0);
	bool bind(unsigned int addr, unsigned short port);
	bool listen(int backlog) {
		return (0 == ::listen(sock_, backlog));
	}

	int sock_;
};

typedef std::shared_ptr<Socket> SocketPtr;

#endif
