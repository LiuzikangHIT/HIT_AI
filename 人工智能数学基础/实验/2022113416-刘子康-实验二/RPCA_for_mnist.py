import numpy as np
import matplotlib.pyplot as plt
import mnist
from scipy.linalg import svd
from sklearn.linear_model import LogisticRegression
from sklearn.neighbors import KNeighborsClassifier

# 查询某张图片
def display_sample(data, num):
    image = data[num].reshape([28, 28])
    plt.imshow(image, cmap=plt.get_cmap('gray_r'))
    plt.show()

# display_sample(train_img, 4366)

# RPCA
def RobustPCA(X, lambda_param=None, mu=None, tol=1e-6, max_iter=100):
    m, n = X.shape
    normX = np.linalg.norm(X, 'fro')

    if lambda_param is None:
        lambda_param = 1 / np.sqrt(max(m, n))
    if mu is None:
        mu = 10 * lambda_param

    L = np.zeros((m, n))    # 低秩矩阵
    S = np.zeros((m, n))    # 稀疏矩阵
    Y = np.zeros((m, n))    # 拉格朗日乘子

    for i in range(max_iter):
        # 更新矩阵L和S
        L = SVT(X - S + (1 / mu) * Y, 1 / mu)
        S = soft_threshold(X - L + (1 / mu) * Y, lambda_param / mu)

        Z = X - L - S
        Z[np.isnan(X)] = 0  # 缺失值置0
        Y += mu * Z

        err = np.linalg.norm(Z, 'fro') / normX
        if i == 0 or i % 10 == 0 or err < tol:
            print(f'iter: {i:04d}\terr: {err:.6f}\trank(L): {np.linalg.matrix_rank(L)}\tcard(S): {np.count_nonzero(S[~np.isnan(X)])}')
        if err < tol:
            break
    print(f"RPCA降维: {X.shape[1]}维->{np.linalg.matrix_rank(L)}维")
    return L, S

# 奇异值收缩算法
def SVT(X, tau):
    # shrinkage operator for singular values
    [U, s, Vh] = svd(X, full_matrices=False)
    s = soft_threshold(s, tau)
    return U @ np.diag(s) @ Vh

# 软阈值函数
def soft_threshold(X, tau):
    return np.sign(X) * np.maximum(np.abs(X) - tau, 0)

# 逻辑回归分类
def Logistic(x_train, y_train, x_test, y_test):
    model = LogisticRegression(solver='sag')
    model.fit(x_train, y_train)
    score = model.score(x_test, y_test)
    y_pred = model.predict(x_test)
    print(f"逻辑回归分类准确率：{score}")
    return y_pred

# KNN分类
def KNN(x_train, y_train, x_test, y_test):
    model = KNeighborsClassifier()
    model.fit(x_train, y_train)
    score = model.score(x_test, y_test)
    y_pred = model.predict(x_test)
    print(f"KNN分类准确率：{score}")
    return y_pred

# 读取数据集并初始化
train_img, train_lbl, test_img, test_lbl = mnist.load()

# RPCA获取低秩矩阵
L, S = RobustPCA(train_img)

# 分类
log_pred = Logistic(L, train_lbl, test_img, test_lbl)
knn_pred = KNN(L, train_lbl, test_img, test_lbl)

# 绘图
plt.figure(figsize=(15,25))
for i in range(25):
    axes = plt.subplot(5, 5, i+1)
    plt.imshow(test_img[i].reshape(28,28))
    axes.axis('off')
    axes.set_title(f'\nreal:{test_lbl[i]}  KNN:{log_pred[i]}  Log:{knn_pred[i]}')
plt.show()

