#! /usr/bin/python2.7
# -*-encoding=utf-8-*-

import os
import os.path
import Catalog
from DocException import*
import Catalog
from DocManager_m import*

class DocManagerTree(DocManager):
    def __init__(self, _docFilter=['.doc', '.py', '.md']):
        super(DocManagerTree, self).__init__(_docFilter)
        self.__CatalogObj = None

    def __dirSearch(self, _rootCatalog, _ndeep=10):
        """
        文件夹遍历,结果存放在自定义的目录对象(CatalogList)，用于存放一个文件夹包含的所有文件(CatalogItem)对象
        :param _rootCatalog: 目录对象(CatalogList)
        :param _ndeep: 允许的最大递归索引深度
        :return:
        """
        if _ndeep-1 < 0:
            return False
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
                    a = self.__dirSearch(t_childrenlist, _ndeep=_ndeep-1)
                    dirHaveRightDoc = dirHaveRightDoc or a
            return dirHaveRightDoc

    def SetCatalogObj(self, dirPath, _ndeep=10):
        """
        使用__dirSearch()方法搜索目录,并返回结果
        :param dirPath:
        :param _ndeep:索引深度
        :return:self.__CatalogObj:目录对象CatalogList
        """
        self.__CatalogObj = Catalog.CatalogList(dirPath)
        self.__dirSearch(self.__CatalogObj, _ndeep)
        return self.GetCatalogObj()

    def GetCatalogObj(self):
        return self.__CatalogObj



if __name__=='__main__':
    dirPath = '/home/yangzheng/myPrograms/MLTools_PyQt4'
    test = DocManagerTree()
    test.SetCatalogObj(dirPath, 1)
    ct = test.GetCatalogObj()
    try:
        print Catalog.getTree(ct)
    except DocException, e:
        e.what()
