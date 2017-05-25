#! /usr/bin/python2.7
# -*-encoding=utf-8-*-

import os
import os.path
import Catalog
from DocException import*
import Catalog
from DocManager_m import*

class DocManagerList(DocManager):
    def __init__(self, _docFilter=['.doc', '.py', '.md']):
        super(DocManagerList, self).__init__(_docFilter)
        self.__CatalogObj = None

    def __dirSearch(self, dirPath, nRetract=0):
        """
        文件夹遍历,结果存放在list对象self.__CatalogObj中，格式如下：
        [文件名(str), 文件缩进级别(int), 是否是文件(bool)]
        :param dirPath: 文件夹路径
        :param nRetract: 文件缩进级别，用于判断文件的父目录
        :return:
        """
        dirlist = os.listdir(dirPath)
        if len(dirlist) <= 0:
            return False
        dirName = os.path.split(dirPath)
        self.__CatalogObj.append([dirName[1], nRetract, 0])
        dirHaveRightDoc = False
        for t_file in dirlist:
            # print os.path.join(dirPath, t_file)
            if not os.path.isdir(os.path.join(dirPath, t_file)):
                if self.isFIleInFilter(t_file):
                    self.__CatalogObj.append([t_file, nRetract + 1, 1])
                    dirHaveRightDoc = dirHaveRightDoc or True
            else:
                a = self.__dirSearch(os.path.join(dirPath, t_file), nRetract=nRetract + 1)
                dirHaveRightDoc = dirHaveRightDoc or a
        if not dirHaveRightDoc:
            del self.__CatalogObj[-1]
        return dirHaveRightDoc

    def SetCatalogObj(self, dirPath):
        """
        使用__dirSearch()方法搜索目录,产生CatalogList对象,存放独有成员self.catalogTree
        :param dirPath:
        :return:
        """
        self.__CatalogObj = []
        self.__dirSearch(dirPath, 0)
        return self.GetCatalogObj()

    def GetCatalogObj(self):
        return self.__CatalogObj

    def __repr__(self):
        """
        打印目录
        :return: 目录的文本
        """
        reprStr = ''
        for t_file in self.__CatalogObj:
            if t_file[2] == 0:#文件夹
                reprStr += '|' * (t_file[1]) + '>' + t_file[0] + '\n'
            else:
                reprStr += '|' * (t_file[1]) + t_file[0] + '\n'
        return reprStr

if __name__=='__main__':
    dirPath = '/home/yangzheng/myPrograms/MLTools_PyQt4'
    test = DocManagerList()
    test.SetCatalogObj(dirPath)
    ct = test.GetCatalogObj()
    print repr(test)
