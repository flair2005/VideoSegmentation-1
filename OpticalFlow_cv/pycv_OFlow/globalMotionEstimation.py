# encoding:utf-8
import numpy as np
import cv2
from sklearn import linear_model

def LinearRegression(flow):
    reg_u = linear_model.LinearRegression()
    reg_v = linear_model.LinearRegression()
    X = np.empty((flow.shape[0]*flow.shape[1], 2))
    Y_u = np.empty((flow.shape[0] * flow.shape[1]))
    Y_v = np.empty((flow.shape[0] * flow.shape[1]))
    for i in range(0, flow.shape[0]):
        for j in range(0, flow.shape[1]):
            X[(i)*flow.shape[1]+j] = i+1, j+1
            Y_u[(i)*flow.shape[1]+j] = flow[i, j, 0]
            Y_v[(i) * flow.shape[1] + j] = flow[i, j, 1]
    # print X
    # print Y_u
    # print Y_v
    reg_u.fit(X, Y_u)
    reg_v.fit(X, Y_v)
    # print reg_u.score(X,Y_u), reg_v.score(X,Y_v)
    pred_u = reg_u.predict(X)
    pred_v = reg_v.predict(X)
    residual = np.power(pred_u - Y_u, 2) + np.power(pred_v - Y_v, 2)
    return cv2.convertScaleAbs(residual,alpha=1,beta=0)