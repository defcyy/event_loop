# -*- coding: utf-8 -*-
import time
import selectors

EVENT_READ = selectors.EVENT_READ
EVENT_WRITE = selectors.EVENT_WRITE

class EventLoop(object):
    """A test event loop

    Write a simple event loop like ioloop.
    """
    __instance = None

    @classmethod
    def instance(cls):
        if cls.__instance is None:
            cls.__instance = EventLoop()
            cls.__instance.initialize()
        return cls.__instance

    def initialize(self):
        self.selector = selectors.DefaultSelector()
        self.__running = False
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

            events = self.selector.select(poll_timeout)
            for key, event in events:
                self.handlers[key.fd]()

    def stop(self):
        self.__running = False

    def register(self, fd, event, callback):
        """Add fd in the event loop
        """
        self.selector.register(fd, event)
        self.handlers[fd] = callback

    def unregister(self, fd):
        """Remove fd from the event loop
        """
        self.selector.unregister(fd)

    def call_at(self, call_time, callback):
        self.timeouts.append((call_time, callback))
        self.timeouts.sort(key=lambda x: x[0])
