# encoding:utf-8
import numpy as np
import cv2
import globalMotionEstimation as me

# import video #Opencv Python自带的读取

help_message = '''''
USAGE: opt_flow.py [<video_source>]

Keys:
 1 - toggle HSV flow visualization
 2 - toggle glitch

'''


def draw_flow(img, flow, step=16):
    h, w = img.shape[:2]
    y, x = np.mgrid[step / 2:h:step, step / 2:w:step].reshape(2, -1)  # 以网格的形式选取二维图像上等间隔的点，这里间隔为16，reshape成2行的array
    fx, fy = flow[y, x].T  # 取选定网格点坐标对应的光流位移
    lines = np.vstack([x, y, x + fx, y + fy]).T.reshape(-1, 2, 2)  # 将初始点和变化的点堆叠成2*2的数组
    lines = np.int32(lines + 0.5)  # 忽略微笑的假偏移，整数化
    vis = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    cv2.polylines(vis, lines, 0, (0, 255, 0))  # 以初始点和终点划线表示光流运动
    for (x1, y1), (x2, y2) in lines:
        cv2.circle(vis, (x1, y1), 1, (0, 255, 0), -1)  # 在初始点（网格点处画圆点来表示初始点）
    return vis


def draw_hsv(flow):
    h, w = flow.shape[:2]
    fx, fy = flow[:, :, 0], flow[:, :, 1]
    ang = np.arctan2(fy, fx) + np.pi  # 得到运动的角度
    v = np.sqrt(fx * fx + fy * fy)  # 得到运动的位移长度
    hsv = np.zeros((h, w, 3), np.uint8)  # 初始化一个0值空3通道图像
    hsv[..., 0] = ang * (180 / np.pi / 2)  # B通道为角度信息表示色调
    hsv[..., 1] = 255  # G通道为255饱和度
    hsv[..., 2] = np.minimum(v * 4, 255)  # R通道为位移与255中较小值来表示亮度
    bgr = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)  # 将得到的HSV模型转换为BGR显示
    return bgr

def Mod_flow(flow):
    # mod_flow = np.empty(flow.shape)
    mod_flow = np.zeros((flow.shape[0], flow.shape[1], 1), np.uint8)
    np.power(flow, 2)
    for i in range(flow.shape[0]):
        for j in range(flow.shape[1]):
            mod_flow[i, j] = flow[i, j, 0] + flow[i, j, 1]
    return mod_flow


def warp_flow(img, flow):
    h, w = flow.shape[:2]
    flow = -flow
    flow[:, :, 0] += np.arange(w)
    flow[:, :, 1] += np.arange(h)[:, np.newaxis]
    res = cv2.remap(img, flow, None, cv2.INTER_LINEAR)  # 图像几何变换（线性插值），将原图像的像素映射到新的坐标上去
    return res


if __name__ == '__main__':
    import sys

    print help_message
    try:
        fn = sys.argv[1]
    except:
        fn = 0

    cam = cv2.VideoCapture("/home/yangzheng/testData/test.mp4")  # 读取视频
    ret, prev = cam.read()  # 读取视频第一帧作为光流输入的当前帧֡
    # prev = cv2.imread('E:\lena.jpg')
    prevgray = cv2.cvtColor(prev, cv2.COLOR_BGR2GRAY)
    show_hsv = False
    show_glitch = False
    cur_glitch = prev.copy()

    while True:
        ret, img = cam.read()  # 读取视频的下一帧作为光流输入的当前帧
        if ret == True:  # 判断视频是否结束
            if cv2.waitKey(10) == 27:
                break
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            flow = cv2.calcOpticalFlowFarneback(prevgray, gray, None, pyr_scale=0.5,
                    levels=3, winsize=15, iterations=3, poly_n =5,
                    poly_sigma=1.5, flags=cv2.OPTFLOW_LK_GET_MIN_EIGENVALS)  # Farnback光流法
            prevgray = gray  # 计算完光流后，将当前帧存储为下一次计算的前一帧

            me.LinearRegression(flow)

            cv2.imshow('flow', draw_flow(gray, flow))
            # cv2.imshow("mod flow", Mod_flow(flow))
            if show_hsv:
                cv2.imshow('flow HSV', draw_hsv(flow))
            if show_glitch:
                cur_glitch = warp_flow(cur_glitch, flow)
                cv2.imshow('glitch', cur_glitch)

            ch = 0xFF & cv2.waitKey(50)
            # print type(ch), ch, str(ch), chr(ch)
            if chr(ch) == 'q':
                break
            if ch == ord('1'):
                show_hsv = not show_hsv
                print 'HSV flow visualization is', ['off', 'on'][show_hsv]
            if ch == ord('2'):
                show_glitch = not show_glitch
                if show_glitch:
                    cur_glitch = img.copy()
                print 'glitch is', ['off', 'on'][show_glitch]
        else:
            break
    cv2.destroyAllWindows()