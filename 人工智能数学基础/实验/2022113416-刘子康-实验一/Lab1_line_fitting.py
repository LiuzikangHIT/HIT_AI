import numpy as np
import matplotlib.pyplot as plt
from math import sqrt
from scipy.stats import linregress
from collections import OrderedDict

np.random.seed(28)

data_num = 30
line_num = 3

# 直线方程（以3x-y+16=0为例)
def linear_func(x, k, b):
    return k * x + b

# 生成训练集散点并添加噪声和异常点
k0, b_list = 3, [-20, 60, 140]
x_train, y_train = [], []
X, Y = [], []   # 添加异常点

for b in b_list:
    x_train.append(list(np.random.uniform(-20, 20, size=data_num)))
    y_train.append(list(np.array([linear_func(x, k0, b) + np.random.normal(0, 4) for x in x_train[-1]])))

for x, y, b in zip(x_train, y_train, b_list):
    out_num = int(0.4 * data_num)  # 外点数量
    X.append(x.copy())
    X[-1].extend(np.random.uniform(-20, 20, size=out_num))
    Y.append(y.copy())
    Y[-1].extend(np.random.uniform(-20 * k0 + b, 20 * k0 + b, size=out_num))

print(f"原始方程：y={k0}x+{b_list[0]}  y={k0}x+{b_list[1]}  y={k0}x+{b_list[2]}")

# 计算距离
def dis(x, y, A, B, C):
    return abs(A * x + B * y + C) / sqrt(A ** 2 + B ** 2)

# 最小二乘法
def least_square(x_train, y_train):
    # 线性回归
    slope, intercept, r_value, p_value, std_err = linregress(x_train, y_train)
    return slope, intercept, std_err

# RANSAC方法
def ransac(data_num, x_train, y_train):
    t = 8   # 阈值
    train_times = 200    # 训练次数
    S_lines = []    # 一致集

    # 随机选点并计算斜率、截距和一致集
    for i in range(train_times):
        sample = np.random.randint(int(data_num * 1.4), size=2)
        x1, y1, x2, y2 = x_train[sample[0]], y_train[sample[0]], x_train[sample[1]], y_train[sample[1]]
        if x1 != x2:
            k = (y1 - y2) / (x1 - x2)
            b = y1 - k * x1
            Interior = 0
            for x, y in zip(x_train, y_train):
                if x != x1 and x != x2:
                    if dis(x, y, k, -1, b) < t:
                        Interior += 1
            S_lines.append([k, b, Interior])

    # 计算最大一致集
    Interior = [line[2] for line in S_lines]
    index = np.argmax(Interior)
    return S_lines[index][0], S_lines[index][1]

lq_k, lq_b = [], []
rs_k, rs_b = [], []
E_RMS = []
for i in range(line_num):
    k1, b1, E = least_square(x_train[i], y_train[i])
    lq_k.append(k1)
    lq_b.append(b1)
    E_RMS.append(E)
    k2, b2 = ransac(data_num, X[i], Y[i])
    rs_k.append(k2)
    rs_b.append(b2)

print(f"最小二乘法结果：y={lq_k[0]:.2f}x+{lq_b[0]:.2f}(E={E_RMS[0]:.4f})  y={lq_k[1]:.2f}x+{lq_b[1]:.2f}(E={E_RMS[1]:.4f})  y={lq_k[2]:.2f}x+{lq_b[2]:.2f}(E={E_RMS[2]:.4f})")
print(f"RANSAC方法结果：y={rs_k[0]:.2f}x+{rs_b[0]:.2f}  y={rs_k[1]:.2f}x+{rs_b[1]:.2f}  y={rs_k[2]:.2f}x+{rs_b[2]:.2f}")

x = np.linspace(-20, 20, 51)
y_pred_lq = [k * x + b for k, b in zip(lq_k, lq_b)]
y_pred_rs = [k * x + b for k, b in zip(rs_k, rs_b)]
y_real = [k0 * x + b for b in b_list]

fig, ax = plt.subplots()
for i in range(line_num):
    ax.scatter(X[i], Y[i], color='red', marker='x', s=20, zorder=20, label='train points')  # 训练集散点
    ax.plot(x, y_pred_lq[i], color='blue', zorder=10, label='least square prediction') # 最小二乘法预测曲线
    ax.plot(x, y_pred_rs[i], color='green', zorder=10, label='ransac prediction')  # RANSAC方法预测曲线
    ax.plot(x, y_real[i], color='yellow', zorder=10, label='real line')    # 真实曲线
plt.grid(color='lightgray', linestyle='--')
handles, labels = plt.gca().get_legend_handles_labels()
by_label = OrderedDict(zip(labels, handles))
plt.legend(by_label.values(), by_label.keys())
fig.tight_layout()
plt.minorticks_on()
plt.show()