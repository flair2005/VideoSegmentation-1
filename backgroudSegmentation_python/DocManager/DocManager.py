#!usr/bin/python2.7
# -*- coding=utf-8 -*-

import os
import os.path
import Catalog
from DocException import*

class DocManager:
    """
    @functions: manage and search the documents
    """
    def __init__(self, _docFilter=['.doc', '.py', '.md']):
        """
        构造DocManager对象,初始化文件筛选器
        :param _docFilter: interesting documents' extension name
        """
        if type(_docFilter) is not list and _docFilter != None:
            raise DocException('param \'_docFilter\' must be list or None.')
        self.__docFilter = _docFilter

    def ResetFilter(self, newFilter):
        """
        重置文件筛选器
        :param newFilter:
        :return:
        """
        if type(newFilter) is not list:
            raise DocException('param \'newFilter\' must be list.')
        self.__docFilter = newFilter

    def AddFilter(self, afilter):
        """
        原有基础上增加文件筛选器
        :param afilter:
        :return:
        """
        if type(afilter) is not str:
            raise DocException('param \'afilter\' must be list.')
        self.__docFilter.append(afilter)
    
    def ShowFilter(self):
        return self.__docFilter

    def isFIleInFilter(self, filePath):
        """
        :判断是否是感兴趣文件
        :param filePath: 文件绝对路径
        :return:
        """
        fileType = os.path.splitext(filePath)
        if fileType[1] in self.__docFilter:
            return True
        else:
            return False

    def __dirSearch(self, dirPath, nRetract=0):
        """
        文件夹遍历,结果存放在list对象中，格式如下：
        [文件名(str), 文件缩进级别(int), 是否是文件(bool)]
        :param dirPath: 文件夹路径
        :param nRetract: 文件缩进级别，用于判断文件的父目录
        :return:
        """
        dirlist = os.listdir(dirPath)
        if len(dirlist) <= 0:
            return False
        dirName = os.path.split(dirPath)
        self.docList.append([dirName[1], nRetract, 0])
        dirHaveRightDoc = False
        for t_file in dirlist:
            # print os.path.join(dirPath, t_file)
            if not os.path.isdir(os.path.join(dirPath, t_file)):
                if self.isFIleInFilter(t_file):
                    self.docList.append([t_file, nRetract+1, 1])
                    dirHaveRightDoc = dirHaveRightDoc or True
            else:
                a = self.__dirSearch(os.path.join(dirPath, t_file), nRetract=nRetract+1)
                dirHaveRightDoc = dirHaveRightDoc or a
        if not dirHaveRightDoc:
            del self.docList[-1]
        return dirHaveRightDoc

    def GetCatalogList(self, dirPath):
        """
        使用__dirSearch()方法搜索指定目录文件,放入list对象self.docList(只有用这个函数搜索才产生此对象)
        :param dirPath:
        :return:
        """
        self.docList = []
        self.__dirSearch(dirPath)
        return self.docList

    def __dirSearch1(self, _rootCatalog, _ndeep=10):
        """
        文件夹遍历,结果存放在自定义的目录对象(CatalogList)，用于存放一个文件夹包含的所有文件(CatalogItem)对象
        :param _rootCatalog: 目录对象(CatalogList)
        :param _ndeep: 允许的最大递归索引深度
        :return:
        """
        if isinstance(_rootCatalog, Catalog.CatalogList):
            dirPath = _rootCatalog.dirPath()
            dirlist = []
            if os.path.isdir(dirPath) and (dirPath != '/'):
                dirlist = os.listdir(dirPath)
            if len(dirlist) <= 0:
                return False
            dirHaveRightDoc = False
            for t_file in dirlist:
                # print os.path.join(dirPath, t_file)
                if not os.path.isdir(os.path.join(dirPath, t_file)):
                    if self.isFIleInFilter(t_file):
                        _rootCatalog.addItem(Catalog.CatalogItem(t_file))
                        dirHaveRightDoc = dirHaveRightDoc or True
                else:
                    t_childrenlist = Catalog.CatalogList(os.path.join(dirPath, t_file))
                    t_catalogItem = Catalog.CatalogItem(t_file, _children=t_childrenlist)
                    _rootCatalog.addItem(t_catalogItem)
                    if _ndeep >= 0:
                        a = self.__dirSearch1(t_childrenlist, _ndeep=_ndeep-1)
                        dirHaveRightDoc = dirHaveRightDoc or a
            return dirHaveRightDoc

    def GetCatalogTree(self, dirPath):
        """
        使用__dirSearch1()方法搜索目录,产生CatalogList对象,存放独有成员self.catalogTree
        :param dirPath:
        :return:
        """
        self.catalogTree = Catalog.CatalogList(dirPath)
        self.__dirSearch1(self.catalogTree)
        return self.catalogTree

    def __dirSearch2(self, dirPath, deep=0):
        """
        根据深度deep搜索文件夹文件,结果存放在self.docList中.此种方式忽略文件是否放在文件夹的子目录中.
        存放格式:
        [
            filepath0,
            filepath1,
            ...
        ]
        :param dirPath: 文件夹路径
        :param deep: 文件夹搜索深度
        :return:
        """
        dirHaveRightDoc = False
        if deep<0:
            return dirHaveRightDoc
        dirlist = os.listdir(dirPath)
        if len(dirlist) <= 0:
            return False
        # dirName = os.path.split(dirPath)
        # self.docList.append([dirName[1], nRetract, 0])
        for t_file in dirlist:
            # print os.path.join(dirPath, t_file)
            if not os.path.isdir(os.path.join(dirPath, t_file)):
                if self.isFIleInFilter(t_file):
                    splitFileName = os.path.splitext(t_file)
                    self.docList.append(t_file)
                    dirHaveRightDoc = dirHaveRightDoc or True
            else:
                a = self.__dirSearch2(os.path.join(dirPath, t_file), deep=deep-1)
                dirHaveRightDoc = dirHaveRightDoc or a
        if not dirHaveRightDoc:
            del self.docList[-1]
        return dirHaveRightDoc

    def GetFileList(self, dirPath):
        self.docList = []
        self.__dirSearch2(dirPath)
        self.docList.sort()
        return self.docList

    def __repr__(self):
        """
        打印目录,仅适用于__dirSearch()方法产生的目录
        :return: 目录的文本
        """
        reprStr = ''
        for t_file in self.docList:
            if t_file[2] == 0:#文件夹
                reprStr += '|' * (t_file[1]) + '>' + t_file[0] + '\n'
            else:
                reprStr += '|' * (t_file[1]) + t_file[0] + '\n'
        return reprStr

    def IntToSeqNum(self, _int, _order):
        """
        将数字序号转化成字符系列，且保持字符序列长度相同（不够的补零）
        :param _int[int] 数字序列
        :param _order[int] 规定字符序列的长度
        :return[string]
        """
        order = 1
        strNum = str(_int)
        while order <= _order:
            if _int < pow(10, order):
                strNum = (_order-order)*'0' + str(_int)
                break
            order+=1

        return strNum

    def DirectoriesMerge(self, srcPaths, tarPath, newFilter=None, _startNum=0, _order=5):
        """
        合并多文件中的文件到指定文件夹下
        :param srcPaths:
        :param tarPath:
        :param newFilter:
        :param _startNum: 指定文件名的起始序列
        :param _order: 指定文件名数字序列的长度
        :return:
        """
        import shutil
        if newFilter != None:
            self.__docFilter = newFilter
        startNum = _startNum
        for srcPath in srcPaths:
            print srcPath
            if os.path.isdir(srcPath):
                self.docList = []
                self.__dirSearch2(srcPath)
                for t_file in self.docList:
                    # print t_file
                    # shutil.copy()
                    t_filePath = os.path.join(srcPath, t_file)
                    shutil.copy(t_filePath, tarPath)
                    strNum = self.IntToSeqNum(startNum, _order)
                    os.rename(os.path.join(tarPath, t_file), os.path.join(tarPath, strNum + '_t' + os.path.splitext(t_file)[1]))
                    startNum += 1


def main():
    DM = DocManager()
    dirPath = '/home/yangzheng/myPrograms/MLTools_PyQt4'
    DM.GetCatalogTree(dirPath)
    print repr(DM.catalogTree)

    DM.GetCatalogList(dirPath)
    print repr(DM)
    # DM.GetDirTree('/home/yangzheng/testData/BodyDataset/training')
    # DM.docList.sort(cmp=lambda x, y: cmp(x, y))
    # print DM.IntToSeqNum(11, 6)
    pass

if __name__ == "__main__":
    main()

