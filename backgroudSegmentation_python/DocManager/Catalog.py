#! /usr/bin/python2.7
# -*-encoding=utf-8-*-

import os
from DocException import*

OKGREEN = '\033[92m'
ENDC = '\033[0m'

class CatalogItem:
    """
    目录表项对象
    """
    def __init__(self, _name, _parent=None, _children=None):
        if isinstance(_name, str) and _name != None:
            self.__name = self.setName(_name)
        else:
            raise DocException('Parameter \'_name\' must be object str.')

        if _parent == None:
            self.__parent = None
        else:
            if isinstance(_parent, CatalogItem):
                self.__parent = _parent
            else:
                raise DocException('Parameter \'_parent\' must be object CatalogItem.')

        if _children == None:
            self.__children = None
        else:
            # print type(_children)
            # print isinstance(_children, CatalogList)
            if isinstance(_children, CatalogList):
                self.__children = _children
            else:
                raise DocException('Parameter \'_children\' must be object CatalogList.')
    
    def name(self):
        return self.__name

    def absName(self):
        return self.__name[0] + self.__name[1]
    
    def parent(self):
        return self.__parent
    
    def children(self):
        return self.__children

    def setName(self, _name):
        return os.path.splitext(_name)

    def setParent(self, _parent):
        if isinstance(_parent, CatalogItem):
            self.__parent = _parent
        else:
            raise DocException('Parameter \'_parent\' must be object CatalogItem.')

    def setChildren(self, _children):
        if isinstance(_children, CatalogList):
            self.__children = _children
        else:
            raise DocException('Parameter \'_children\' must be object CatalogList.')
        return self.__children

    def isFile(self):
        #self.__children == None is True demonstrate that a file
        return self.__children == None

class CatalogList:
    def __init__(self, _path, _parentItem=None):
        self.__path = _path
        self.__catalogList = []
        # self.__indexList = []

    def __getitem__(self, item):
        return self.__catalogList[item]

    def addItem(self, _item):
        if isinstance(_item, CatalogItem):
            self.__catalogList.append(_item)
            # self.__indexList.append(_item.absName())
        else:
            raise DocException('Parameter \'_name\' must be object CatalogItem.')

    def removeItem(self, _name):
        if isinstance(_name, str):
            for it in self.__catalogList:
                if it.absname() == _name:
                    self.__catalogList.remove(it)
        else:
            raise DocException('Parameter \'_name\' must be object str.')

    def catalogList(self):
        return self.__catalogList

    def dirPath(self):
        return self.__path

    def __repr__(self):
        return OKGREEN + '<' + self.dirPath()+'>\n'+getTree(self) + ENDC

def getTree(_catalogList, _nspace=''):
    if not isinstance(_catalogList, CatalogList):
        raise DocException('param \'_catalogList\' must be CatalogList obj')
    t_str = ''
    for item in _catalogList.catalogList():
        if item.absName() != '':
            if item == _catalogList.catalogList()[-1]:
                if item.children() == None:
                    t_str += _nspace + '└─'  + item.absName() + '\n'
                else:
                    t_str += _nspace + '└>' + item.absName() + '\n'
            else:
                if item.children() == None:
                    t_str += _nspace + '├─' + item.absName() + '\n'
                else:
                    t_str += _nspace + '├>' + item.absName() + '\n'
            if item.children() != None:
                if item == _catalogList.catalogList()[-1]:
                    t_str += getTree(item.children(), _nspace+' ')
                else:
                    t_str += getTree(item.children(), _nspace + '│')
    return t_str

def main():
    c = CatalogList()
    b = CatalogItem('root', None, c)
    try:
        a = CatalogItem('test.txt', b, None)
    except DocException, e:
        e.what()
    print a.name()

if __name__ == "__main__":
    main()