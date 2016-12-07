#ifndef DIRBROWSER_H
#define DIRBROWSER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

class myDirBrowser
{
    public:
        myDirBrowser();
        virtual ~myDirBrowser();
        void SetDirPath(std::string dirPath);
        void GetAllFilesName();
        void PrintAllFilesName();
        std::string GetNextFramePath();
        bool GetNextFramePath(std::string *outFilePath);
        inline bool IsLastFrame(){ return m_currentFrame>=(m_files.size()) ? true : false; };

    private:
        /*文件夹路径*/
        string m_dirPath;
        /*按名称排序的文件名*/
        std::vector<string> m_files;
        /*记录读取时的当前帧*/
        unsigned m_currentFrame;
};

#endif // DIRBROWSER_H
