#!usr/bin/python2.7
# -*- coding=utf-8 -*-

import os
import os.path
from DocException import*

class DocManager(object):
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

    def __dirSearch(self, dirPath):
        """
        文件夹遍历,结果存放在
        """
        pass

    def SetCatalogObj(self, dirPath):
        """
        使用__dirSearch()方法搜索指定目录文件,放入list对象self.docList(只有用这个函数搜索才产生此对象)
        :param dirPath:
        """
        pass

    def GetCatalogObj(self):
        pass

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
                strNum = (_order - order)* '0' + str(_int)
                break
            order += 1
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
        pass


def main():
    DM = DocManager()
    dirPath = '/home/yangzheng/myPrograms/MLTools_PyQt4'
    # DM.GetDirTree('/home/yangzheng/testData/BodyDataset/training')
    # DM.docList.sort(cmp=lambda x, y: cmp(x, y))
    # print DM.IntToSeqNum(11, 6)
    pass

if __name__ == "__main__":
    main()

