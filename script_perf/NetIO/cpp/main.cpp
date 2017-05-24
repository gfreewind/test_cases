#include <signal.h>

#include <iostream>
#include <vector>

#include "socket.hpp"
#include "event_poll.hpp"
#include "thread.hpp"

using namespace std;

static vector<uint32_t> rps_stats;
static uint32_t worker_cnt = 1;

static void start_worker_thread(ThreadPool &thrd_pool, uint32_t worker_cnt);
static void fake_http_thread(void *data);
static void show_stats(void);

#define LISTEN_PORT		8081

static const char http_response[] = "HTTP/1.0 200 OK\r\n"
				    "Cache-Control: no-cache\r\n"
				    "Connection: close\r\n"
				    "Content-Type: text/html\r\n"
				    "\r\n"
				    "<html><body><h1>200 OK</h1>\nHello world.\n</body></html>\n";


int main(int argc, const char ** argv)
{
	if (argc > 1) {
		worker_cnt = atoi(argv[1]);
		if (!worker_cnt) {
			cerr << "Invalid worker count" << endl;
			exit(1);
		}
	}

	signal(SIGPIPE, SIG_IGN);

	ThreadPool thrd_pool;

	rps_stats.reserve(worker_cnt);
	start_worker_thread(thrd_pool, worker_cnt);

	thrd_pool.start_all_threads();

	cout << "Start " << worker_cnt << " worker threads" << endl;

	show_stats();

	cout << "Done " << endl;
	return 0;
}


static void start_worker_thread(ThreadPool &thrd_pool, uint32_t worker_cnt)
{
	for (uint32_t i = 0; i < worker_cnt; ++i) {
		Thread::ThreadFunc func = fake_http_thread;
		ThreadPtr thr = make_shared<Thread> (func, &rps_stats[i], "http_server");

		thrd_pool.append_thread(thr);
	}
}

static void show_stats(void)
{
	while (1) {
		sleep(1);

		uint32_t total_rps = 0;
		for (uint32_t i = 0; i < worker_cnt; ++i) {
			total_rps += rps_stats[i];
			rps_stats[i] = 0;
		}

		cout << total_rps << " RPS" << endl;
	}
}

static void fake_http_thread(void *data)
{
	Socket master;
	uint32_t *rps = reinterpret_cast<uint32_t*>(data);

	if (!master.open(AF_INET, SOCK_STREAM, 0,
		Socket::SOCKET_NONBLOCK_BIT | Socket::SOCKET_REUSEADDR_BIT | Socket::SOCKET_REUSEPORT_BIT)) {
		cerr << "Fail to open" << endl;
		exit(1);
	}

	if (!master.bind(Socket::SOCKET_ANY_ADDR, LISTEN_PORT)) {
		cerr << "Fail to bind" << endl;
		exit(1);
	}

	if (!master.listen(1024)) {
		cerr << "Fail to listen" << endl;
		exit(1);
	}

	EventPoll ep;
	if (!ep.init()) {
		cerr << "Fail to init epoll" << endl;
		exit(1);
	}

	if (!ep.epoll_add_fd(master.sock_, EventPoll::EPOLL_EPOLLIN | EventPoll::EPOLL_EPOLLOUT)) {
		cerr << "Fail to add master sock" << endl;
		exit(1);
	}

	int ret = 0;
	char buf[1024];
	vector<EventPoll::EPEvent> ready_fds;

	while (1) {
		ret = ep.epoll_wait(ready_fds);

		if (ret == 0) {
			// timeout
			cout << "timeout" << endl;
			continue;
		} else if (ret) {
			cout << "ret: " << ret << " ready_fds:" << ready_fds.size() << endl;
			for (auto it = ready_fds.begin();
				it != ready_fds.end();
				++it) {
				if (it->fd_ == master.sock_) {
					// new conn
					int fd = accept(master.sock_, NULL, NULL);
					if (!ep.epoll_add_fd(fd, EventPoll::EPOLL_EPOLLIN)) {
						cerr << "Fail to add fd into epoll" << endl;
						break;
					}
				} else {
					ssize_t rdsize = read(it->fd_, buf, sizeof(buf));
					if (rdsize) {
						// Just send and close
						write(it->fd_, http_response, sizeof(http_response));
						*rps += 1;
						ep.epoll_del_fd(it->fd_);
						close(it->fd_);
						
					}					
					ep.epoll_del_fd(it->fd_);
					close(it->fd_);
				}
			}
		} else {
			cerr << "epoll_wait failed" << endl;
			break;
		}
	}
}
