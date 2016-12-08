# -*- coding:utf-8 -*-
__author__ = 'Microcosm'

import cv2
import numpy as np

cap = cv2.VideoCapture("/home/yangzheng/testData/test.mp4")

# 设置 ShiTomasi 角点检测的参数
feature_params = dict(maxCorners=100,
                      qualityLevel=0.3,
                      minDistance=7,
                      blockSize=7)

# 设置 lucas kanade 光流场的参数
# maxLevel 为使用图像金字塔的层数
lk_params = dict(winSize=(15, 15),
                 maxLevel=2,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

# 产生随机的颜色值
color = np.random.randint(0, 255, (100, 3))

# 获取第一帧，并寻找其中的角点
ret, old_frame = cap.read()
old_gray = cv2.cvtColor(old_frame, cv2.COLOR_BGR2GRAY)
p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **feature_params)

# 创建一个掩膜为了后面绘制角点的光流轨迹
mask = np.zeros_like(old_frame)

while (1):
    ret, frame = cap.read()
    if ret:
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # 计算能够获取到的角点的新位置
        p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)

        # 选取好的角点，并筛选出旧的角点对应的新的角点
        good_new = p1[st == 1]
        good_old = p0[st == 1]

        # 绘制角点的轨迹
        for i, (new, old) in enumerate(zip(good_new, good_old)):
            a, b = new.ravel()
            c, d = old.ravel()
            cv2.line(mask, (a, b), (c, d), color[i].tolist(), 2)
            cv2.circle(frame, (a, b), 5, color[i].tolist(), -1)

        img = cv2.add(frame, mask)

        cv2.imshow("frame", img)
        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break

            # 更新当前帧和当前角点的位置
        old_gray = frame_gray.copy()
        p0 = good_new.reshape(-1, 1, 2)

    else:
        break

cv2.destroyAllWindows()
cap.release()