#!/usr/bin/env python
import time
import functools
import socket
from event_loop import EventLoop, EVENT_READ, EVENT_WRITE


def on_connection(sock):
    conn, address = sock.accept()
    print("get connection: ", address)
    
    loop = EventLoop.instance()
    callback = functools.partial(on_recieve, conn)
    loop.register(conn.fileno(), EVENT_READ, callback)
    
def on_recieve(conn):
    data = conn.recv(1024)
    print("get data:", data)

def on_timeout():
    print("timeout call: ", time.time())

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", 8000))
    sock.setblocking(False)
    sock.listen(128)

    loop = EventLoop.instance()
    callback = functools.partial(on_connection, sock)
    loop.register(sock.fileno(), EVENT_READ | EVENT_WRITE, callback)

    call_time = time.time() + 5
    loop.call_at(call_time, on_timeout)
    # stop_time = time.time() + 10
    # loop.call_at(stop_time, loop.stop)

    print("io loop start: ", time.time())
    loop.start()

if __name__ == "__main__":
    main()
