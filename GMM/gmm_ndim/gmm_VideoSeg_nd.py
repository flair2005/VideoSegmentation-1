#! /usr/bin/python2.7
# -*-encoding=utf-8-*-

import random
import cv2
import numpy as np
import math

class gmm_hyper_param:
    """
    gmm背景建模的超参数
    """
    def __init__(self):
        pass

# param = gmm_hyper_param()
# param.D_Gauss = 2   # 使用几维高斯模型
# param.C = 3 # number of gaussian components (typically 3-5)高斯个数
# param.M = 3 # number of background components背景个数
# param.D = 2.5   # positive deviation threshold比较门槛值
# param.alpha = 0.01  # learning rate (between 0 and 1) (from paper 0.01)初始化的学习率
# param.thresh = 0.25 # foreground threshold (0.25 or 0.75 in paper)前景门槛
# param.sd_init = 6   # initial standard deviation (for new components) var = 36 in paper初始化偏差


class gmm_VideoSeg_nd:
    """
    多维的混合高斯背景建模
    """
    def __init__(self, _param):
        self.hyper_param = _param
        self.weight = np.zeros([_param.height, _param.width, _param.C, _param.D_Gauss])  # initialize weights array初始化权重
        self.mean = np.zeros([_param.height, _param.width, _param.C, _param.D_Gauss])  # pixel means像素均值
        self.sd = np.zeros([_param.height, _param.width, _param.C,
                            _param.D_Gauss])  # pixel standard deviations像素标准偏差，注意与下面的方差区别，平方的关系
        self.u_diff = np.zeros([_param.height, _param.width, _param.C,
                                _param.D_Gauss])  # difference of each pixel from mean每个像素与均值的差值
        self.p = _param.alpha / (1 / float(_param.C))
        # self.rank = np.zeros([1, _param.C])
        self.X = np.zeros([_param.height, _param.width, _param.D_Gauss])  #存放建模样本
        self.fr = 0 # 存放当前帧
        self.fr_pre=0 # 存放前一帧
        self.bg_mask = np.zeros([_param.height, _param.width], dtype=np.uint8)
        self.fg_mask = np.zeros([_param.height, _param.width], dtype=np.uint8)
        self.n_frame=0 # 当前帧数
        self.init_gmm_param()

    def init_gmm_param(self):
        """
        初始化模型的各种权重,高斯模型的均值,方差,置信度
        :return: 
        """
        pixel_depth = 8 # 8 - bit resolution
        pixel_range = math.pow(2, pixel_depth) - 1    # pixel range(  # of possible values)
        # 初始化每个点的每个高斯模型的均值，权重，方差
        for h in range(self.hyper_param.height):
            for w in range(self.hyper_param.width):
                for c in range(self.hyper_param.C): # 每个像素点设置了三个高斯模型
                    for dg in range(self.hyper_param.D_Gauss):
                        self.mean[h, w, c, dg] = random.random() * pixel_range  # means random(0 - 255)
                        self.sd[h, w, c, dg] = self.hyper_param.sd_init # 初始化方差
                        self.weight[h, w, c, dg] = 1 / float(self.hyper_param.C)  # weights uniformly dist

    def create_X(self):
        """
        构造高斯模型的样本X,用于多维混合高斯建模在线学习
        :return: 
        """
        self.X[:, :, 0] = self.fr
        for h in range(self.hyper_param.height):
            for w in range(self.hyper_param.width):
                self.X[h, w, 1] = self.filter2(self.fr_pre, self.fr, [h, w], 3)

    def filter2(self, frame_pre, frame, point, kSize):
        """
        获取某像素点的空间特征
        :param frame_pre: 
        :param frame: 
        :param point: 
        :param kSize: 
        :return: 
        """
        shape = frame.shape
        val = 0
        n = 0
        for i in range(kSize):
            x = point[1] + (i - int(kSize) + 2)
            if x >= 0 and x < shape[1]:
                for j in range(kSize):
                    y = point[0] + (j - int(kSize) + 2)
                    if y >= 0 and y < shape[0]:
                        # print frame[y, x],frame_pre[y, x]
                        val = val + abs(float(frame[y, x]) - float(frame_pre[y, x]))
                        n = n + 1
        val = val / n
        return val

    def comput_diff(self):
        """
        计算当前帧每个像素值与每个高斯模型均值的差，方便之后进行模型的匹配
        :return: 
        """
        for c in range(self.hyper_param.C):
            for dg in range(self.hyper_param.D_Gauss):
                self.u_diff[:, :, c, dg] = abs(self.X[:, :, dg] - self.mean[:, :, c, dg])

    def pix_traverse(self):
        """
        逐像素处理图像
        :return: 
        """
        for h in range(self.hyper_param.height):
            for w in range(self.hyper_param.width):
                self.update_gmm(h,w)
                self.update_background(h,w)
                self.update_foreground(h,w)

    def update_gmm(self,h,w):
        self.match = [0, 0]
        for c in range(self.hyper_param.C):
            if (abs(self.u_diff[h, w, c, 0]) <= self.hyper_param.D * self.sd[h, w, c, 0]):
                self.match[0] = 1
                self.weight[h, w, c, 0] = (1 - self.hyper_param.alpha) * self.weight[
                    h, w, c, 0] + self.hyper_param.alpha  # 权值的更新
                self.p = self.hyper_param.alpha / self.weight[h, w, c, 0]  # 更新率的更新
                self.mean[h, w, c, 0] = (1 - self.p) * self.mean[h, w, c, 0] + self.p * self.fr[h, w]  # 当前像素点的均值的更新
                self.sd[h, w, c, 0] = math.sqrt((1 - self.p) * (math.pow(self.sd[h, w, c, 0], 2))) + self.p * (
                    (self.fr[h, w] - math.pow(self.mean[h, w, c, 0], 2)))  # 标准差的更新（也可以用方差）
            else:
                self.weight[h, w, c, 0] = (1 - self.hyper_param.alpha) * self.weight[h, w, c, 0]

            if (abs(self.u_diff[h, w, c, 1]) <= self.hyper_param.D * self.sd[h, w, c, 1]):
                self.match[0] = 1
                self.weight[h, w, c, 1] = (1 - self.hyper_param.alpha) * self.weight[
                    h, w, c, 1] + self.hyper_param.alpha  # 权值的更新
                self.p = self.hyper_param.alpha / self.weight[h, w, c, 1]  # 更新率的更新
                self.mean[h, w, c, 1] = (1 - self.p) * self.mean[h, w, c, 1] + self.p * self.fr[h, w]  # 当前像素点的均值的更新
                self.sd[h, w, c, 1] = math.sqrt((1 - self.p) * (math.pow(self.sd[h, w, c, 1], 2))) + self.p * (
                    (self.fr[h, w] - math.pow(self.mean[h, w, c, 1], 2)))  # 标准差的更新（也可以用方差）
            else:
                self.weight[h, w, c, 1] = (1 - self.hyper_param.alpha) * self.weight[h, w, c, 1]

        # 归一化模型的置信度
        self.weight[h, w, :, 0] = self.weight[h, w, :, 0] / sum(self.weight[h, w, :, 0])
        self.weight[h, w, :, 1] = self.weight[h, w, :, 1] / sum(self.weight[h, w, :, 1])

    def update_background(self, h, w):
        """
        跟新背景
        :param h: 
        :param w: 
        :return: 
        """
        self.bg_mask[h, w] = 0
        for c in range(self.hyper_param.C):
            self.bg_mask[h, w] = int(self.bg_mask[h, w] + (self.mean[h, w, c, 0] * self.weight[h, w, c, 0] +
                                                       self.mean[h, w, c, 1] * self.weight[h, w, c, 1])/2)
        # 当所有的高斯模型都不匹配的时候，用当前的图像参数建立一个新的高斯模型，取代权重最小的模型
        if self.match[0] == 0:
            min_w_index = np.where(np.min(self.weight[h, w, :, 0]))[0][0]
            # [min_w, min_w_index] = np.min(self.weight[h, w, :, 0])
            self.mean[h, w, min_w_index, 0] = self.X[h, w, 0]
            self.sd[h, w, min_w_index, 0] = self.hyper_param.sd_init

        if self.match[1] == 0:
            min_w_index = np.where(np.min(self.weight[h, w, :, 1]))[0][0]
            # [min_w, min_w_index] = np.min(self.weight[h, w,:, 1])
            self.mean[h, w, min_w_index, 1] = self.X[h, w,1]
            self.sd[h, w, min_w_index, 1] = self.hyper_param.sd_init

    def rank_mode_by_weight(self,h,w):
        """
        按照置信度对模型排序
        :return: 
        """
        rank = (self.weight[h, w,:, 0] / self.sd[h, w,:, 0] + self.weight[h, w,:, 1] / self.sd[h, w,:, 1]) / 2 # calculate component rank
        rank_ind = range(self.hyper_param.C) #计算权重与标准差的比值，用于对背景模型进行排序
        # sort rank values
        for c in range(1, self.hyper_param.C):
            for m in range((c - 1)):
                if (rank[c] > rank[m]):
                    # swap max values
                    rank_temp = rank[m]
                    rank[m] = rank[c]
                    rank[c] = rank_temp

                     # swap max index values
                    rank_ind_temp = rank_ind[m]
                    rank_ind[m] = rank_ind[c]
                    rank_ind[c] = rank_ind_temp
        return rank_ind

    def update_foreground(self,h,w):
        """
        跟新前景
        :return: 
        """
        rank_ind=self.rank_mode_by_weight(h,w)
        match = 0
        k = 0
        self.fg_mask[h, w] = 0
        while ((match == 0) and (k < self.hyper_param.M)):
            if (self.weight[h, w, rank_ind[k],0] >= self.hyper_param.thresh):
                if (abs(self.u_diff[h, w, rank_ind[k], 0]) * 2 / 3 + abs(self.u_diff[h, w, rank_ind[k], 1]) * 1 / 3 <= self.hyper_param.D * (
                            self.sd[h, w, rank_ind[k], 0] * 2 / 3 + self.sd[h, w, rank_ind[k], 1] * 1 / 3)):
                    self.fg_mask[h, w] = 255
                    match = 1
                else:
                    self.fg_mask[h, w] = 0
            k = k + 1


    def apply(self, fr):
        if len(fr.shape) == 3:
            self.fr = cv2.cvtColor(fr, cv2.COLOR_BGR2GRAY)
        elif len(fr.shape) == 2:
            self.fr = fr
        else:
            pass
        # cv2.imshow('aa',self.fr)
        # cv2.waitKey()
        if self.n_frame==0:
            self.fr_pre = self.fr.copy()
        self.create_X()
        self.comput_diff()
        self.pix_traverse()

        self.fr_pre = self.fr.copy()
        self.n_frame = self.n_frame+1

    def get_background_mask(self):
        return self.bg_mask

    def get_foreground_mask(self):
        return self.fg_mask

