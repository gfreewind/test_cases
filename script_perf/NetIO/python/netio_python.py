#!/usr/bin/python
import sys, socket, select, threading, time

http_response = ("HTTP/1.0 200 OK\r\n"
                "Cache-Control: no-cache\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<html><body><h1>200 OK</h1>\nHello world.\n</body></html>\n")

rps_cnt = 0

def show_stats():
	global rps_cnt
	while (True):
		time.sleep(1)
		print "Total: " + str(rps_cnt) + " RPS"
		rps_cnt = 0

				
if __name__ == "__main__":
	if (len(sys.argv) < 2) :
		print "Please specify the listen port"
		exit(0) 

	master_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	master_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	master_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
	master_socket.bind(('', int(sys.argv[1])))
	master_socket.listen(1024)
	master_socket.setblocking(0)

	epoll = select.epoll()
	epoll.register(master_socket.fileno(), select.EPOLLIN)
	
	t = threading.Thread(target=show_stats)
	t.setDaemon(True)
	t.start()

	worker_sockets = {}

	while (True):
		request = ""
		events = epoll.poll(-1) # wait 3 seconds
		for fileno, event in events:
			if fileno == master_socket.fileno():
				worker_socket, client_addr = master_socket.accept()
				worker_socket.setblocking(0)
				epoll.register(worker_socket.fileno(), select.EPOLLIN|select.EPOLLHUP)
				worker_sockets[worker_socket.fileno()] = worker_socket
			elif (event & (select.EPOLLIN|select.EPOLLHUP)):
				request = worker_sockets[fileno].recv(1024)
				client_sock = worker_sockets[fileno]
				if (len(request)):
					client_sock.send(http_response)
					rps_cnt += 1
				epoll.unregister(fileno)
				client_sock.close()
				del worker_sockets[fileno]
			else:
				print "Unhandled event", event
	
	epoll.unregister(master_socket.fileno())
	master_socket.close();

	
	


	
