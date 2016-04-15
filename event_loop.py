# -*- coding: utf-8 -*-
import time
import select

class EventLoop(object):
    """A test event loop

    Write a simple event loop like ioloop.
    """
    __instance = None

    @staticmethod
    def instance():
        if EventLoop.__instance is None:
            EventLoop.__instance = EventLoop()
            EventLoop.__instance.initialize()
        return EventLoop.__instance

    def initialize(self):
        self.__running = False
        self.read_fds = set()
        self.write_fds = set()
        self.handlers = {}
        self.timeouts = []

    def start(self):
        """Run a event loop
        """
        self.__running = True

        while True:
            if not self.__running:
                break
            poll_timeout = 1

            while self.timeouts:
                now = time.time()
                if self.timeouts[0][0] <= now:
                    timeout_item = self.timeouts.pop()
                    timeout_item[1]()
                elif self.timeouts[0][0] - now < poll_timeout:
                    poll_timeout = self.timeouts[0][0] - now
                else:
                    break

            readable, writable, errors = select.select(self.read_fds, self.write_fds, [], poll_timeout)
            for item in readable:
                self.handlers[item]()
            for item in writable:
                self.handlers[item]()

    def stop(self):
        self.__running = False

    def register(self, fd, event, callback):
        """Add fd in the event loop
        """
        if event == "r":
            self.read_fds.add(fd)
        elif event == "w":
            self.write_fds.add(fd)
        self.handlers[fd] = callback

    def unregister(self, fd, event):
        """Remove fd from the event loop
        """
        if event == "r":
            self.read_rds.discard(fd)
            self.handlers.remove(fd)

    def call_at(self, call_time, callback):
        self.timeouts.append((call_time, callback))
        self.timeouts.sort(key=lambda x: x[0])
