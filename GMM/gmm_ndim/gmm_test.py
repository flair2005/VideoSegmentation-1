#! /usr/bin/python2.7
# -*-encoding=utf-8-*-
import os.path as path
import cv2
import DocManager.DocManager as dm
import gmm_VideoSeg_nd as ngmm
import gmm_VideoSeg_1d as gmm

imgDir = '/home/yangzheng/testData/mall_dataset/frames/Sample-213x160'
docM = dm.DocManager(_docFilter=['.jpg'])
imgsList = docM.GetFileList(imgDir)

first_frame=cv2.imread(path.join(imgDir, imgsList[0]))

# param = ngmm.gmm_hyper_param()
param = gmm.gmm_hyper_param()
param.D_Gauss = 1   # 使用几维高斯模型
param.C = 3 # number of gaussian components (typically 3-5)高斯个数
param.M = 3 # number of background components背景个数
param.D = 2.5   # positive deviation threshold比较门槛值
param.alpha = 0.01  # learning rate (between 0 and 1) (from paper 0.01)初始化的学习率
param.thresh = 0.25 # foreground threshold (0.25 or 0.75 in paper)前景门槛
param.sd_init = 6   # initial standard deviation (for new components) var = 36 in paper初始化偏差
param.width=first_frame.shape[1]
param.height=first_frame.shape[0]

# gmm_videoSeg = ngmm.gmm_VideoSeg_nd(param)
gmm_videoSeg = gmm.gmm_VideoSeg_1d(param)
for it in imgsList:
    fr = cv2.imread(path.join(imgDir, it))
    gmm_videoSeg.apply(fr)
    cv2.imshow('frame', fr)
    cv2.imshow('foreground', gmm_videoSeg.get_foreground_mask())
    cv2.imshow('background', gmm_videoSeg.get_background_mask())
    cv2.waitKey(1)

