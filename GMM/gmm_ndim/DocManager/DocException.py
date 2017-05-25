#! /usr/bin/python2.7
# -*-encoding=utf-8-*-

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKGREEN = '\033[92m'
WARNING = '\033[93m'
ERROR = '\033[31m'
FAIL = '\033[91m'
ENDC = '\033[0m'

class DocException(BaseException):
    def __init__(self, _message):
        self.message = _message
    def what(self):
        print ERROR + '<Document Exception> ' + self.message + ENDC