import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize
from math import sin, cos, pi, exp, sqrt

np.random.seed(28)

def least_square():
    M = 9   # 阶数
    lambd = exp(-18)    # 权系数λ

    # 拟合多项式
    def fun_y(w, x):
        return w[0] + w[1]*x + w[2]*x**2 + w[3]*x**3 + w[4]*x**4 + w[5]*x**5 + w[6]*x**6 + w[7]*x**7 + w[8]*x**8 + w[9]*x**9

    # 误差平方和 & 正则化
    def fun_E(w, x, t):
        return 0.5 * sum((fun_y(w, x) - t) ** 2) + 0.5 * lambd * sum(w ** 2)

    # 生成训练集散点
    x_train = np.random.rand(8*(M+1))
    y_train = np.array([sin(2*pi*i) + np.random.normal(0, 0.09) for i in x_train])

    # 求解线性方程组
    coef = np.array([1.0] * (M+1))  # 初始系数向量
    res = minimize(fun_E, coef, args=(x_train, y_train))
    w = res.x
    x_pred = np.linspace(0, 1, 51)
    y_pred = w[0] + w[1]*x_pred + w[2]*x_pred**2 + w[3]*x_pred**3 + w[4]*x_pred**4 + w[5]*x_pred**5 + w[6]*x_pred**6 + w[7]*x_pred**7 + w[8]*x_pred**8 + w[9]*x_pred**9
    x = np.linspace(0, 1, 51)
    y_real = np.array([sin(2*pi*i) for i in x])

    fig, ax = plt.subplots()
    ax.scatter(x_train, y_train, color='red', marker='x', s=20, zorder=20, label='train points')  # 训练集散点
    ax.plot(x_pred, y_pred, color='blue', zorder=10, label='prediction curve')   # 预测曲线
    ax.plot(x, y_real, color='yellow', zorder=10, label='real curve')    # 真实曲线
    ax.legend(loc='best')
    ax.grid(color='lightgray', linestyle='--')
    plt.title("fit trigfunc by least_square")
    plt.minorticks_on()
    plt.show()

def ransac():
    data_num = 100  # 散点数量
    train_times = 10000 # 训练次数
    t = 1   # 阈值
    
    # 圆曲线(x-3)^2+(y-4)^2=25，生成训练集散点
    h0, k0, r0 = 3, 4, 5
    angle_train = np.random.rand(data_num)
    x_train = np.array([h0 + r0 * cos(2 * pi * i) + np.random.normal(0, 0.64) for i in angle_train])
    y_train = np.array([k0 + r0 * sin(2 * pi * j) + np.random.normal(0, 0.64) for j in angle_train])

    # 添加异常点
    out_num = int(0.4 * data_num)  # 外点数量
    x_outliers = np.random.randint(h0 - 2 * r0, h0 + 2 * r0, size=out_num)
    y_outliers = np.random.randint(k0 - 2 * r0, k0 + 2 * r0, size=out_num)
    X, Y = list(x_train), list(y_train)
    X.extend(x_outliers)
    Y.extend(y_outliers)

    # 根据三点坐标求圆方程
    def circle_from_3_points(x1, y1, x2, y2, x3, y3):
        # 转换为numpy数组，方便后续计算
        A = np.array([[x1, y1, 1], [x2, y2, 1], [x3, y3, 1]])
        B = np.array([x1 ** 2 + y1 ** 2, x2 ** 2 + y2 ** 2, x3 ** 2 + y3 ** 2])

        # 解线性方程组得到圆心坐标D
        D = np.linalg.solve(A.T.dot(A), A.T.dot(B))
        h, k, _ = D
        
        # 计算半径r
        r = np.sqrt((D[0] - x1) ** 2 + (D[1] - y1) ** 2)
        return h, k, r

    # 判断是否为外点
    def isOutlier(x, y, h, k, r):
        return abs(sqrt((x - h) ** 2 + (y - k) ** 2) - r) > t

    # 随机选点并计算圆心和半径
    res = []
    for i in range(train_times):
        sample = np.random.choice(data_num, size=3, replace=False)
        x1, y1, x2, y2, x3, y3 = X[sample[0]], Y[sample[0]], X[sample[1]], Y[sample[1]], X[sample[2]], Y[sample[2]]
        h, k, r = circle_from_3_points(x1, y1, x2, y2, x3, y3)
        inter = 0
        for x, y in zip(X, Y):
            if x not in [x1, x2, x3]:
                if not isOutlier(x, y, h, k, r):
                    inter += 1
        res.append([h, k, r, inter])

    # 计算最大一致集
    S = np.array([cir[3] for cir in res])
    index = np.argmax(S)
    h_fit, k_fit, r_fit = res[index][0], res[index][1], res[index][2]
    print(f"RANSAN方法：圆心：({h_fit:.2f}, {k_fit:.2f}) 半径：r={r_fit:.2f}")

    # RANSAC预测曲线
    angle = np.linspace(0, 1, num=51)
    x_pred = np.array([h_fit + r_fit * cos(2 * pi * i) for i in angle])
    y_pred = np.array([k_fit + r_fit * sin(2 * pi * j) for j in angle])
    x_real = np.array([h0 + r0 * cos(2 * pi * i) for i in angle])
    y_real = np.array([k0 + r0 * sin(2 * pi * i) for i in angle])

    fig, ax = plt.subplots()
    ax.scatter(X, Y, color='red', marker='x', s=20, zorder=20, label='train points')   # 训练集散点
    ax.plot(x_pred, y_pred, color='blue', zorder=10, label='prediction curve')  # 预测曲线
    ax.plot(x_real, y_real, color='yellow', zorder=10, label='real curve')  # 真实曲线
    ax.legend(loc='best')
    ax.grid(color='lightgray', linestyle='--')
    plt.title("fit circle by RANSAC")
    plt.minorticks_on()
    plt.show()

least_square()
ransac()
