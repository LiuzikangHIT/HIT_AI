import numpy as np
import matplotlib.pyplot as plt
import mnist
from scipy import linalg
from sklearn.linear_model import LogisticRegression
from sklearn.neighbors import KNeighborsClassifier

# 查询某张图片
def display_sample(data, num):
    image = data[num].reshape([28, 28])
    plt.imshow(image, cmap=plt.get_cmap('gray_r'))
    plt.show()

# display_sample(train_img, 4366)

# PCA降维
def mnistPCA(train_data, test_data, direction):
    train_res = np.dot(train_data, direction)
    test_res = np.dot(test_data, direction)
    print(f"PCA降维: {train_data.shape[1]}维->{train_res.shape[1]}维")
    return train_res, test_res

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

# SVD求特征值和特征向量
eig_V = linalg.eig(np.dot(train_img.T, train_img))
eigenvalues, eigenvector = eig_V[0], eig_V[1]

# 计算投影矩阵
n = np.sum(eigenvalues > 100) # 降维后维数为大于100的特征值个数
sorted_indices = sorted(enumerate(eigenvalues), key=lambda x: x[1], reverse=True)
max_index = [index for index, value in sorted_indices[:n]]
direction = eigenvector[:, max_index]

# PCA降维
train_data, test_data = mnistPCA(train_img, test_img, direction)

# 分类
log_pred = Logistic(train_data, train_lbl, test_data, test_lbl)
knn_pred = KNN(train_data, train_lbl, test_data, test_lbl)

# 绘图
plt.figure(figsize=(15,25))
for i in range(25):
    axes = plt.subplot(5, 5, i+1)
    plt.imshow(test_img[i].reshape(28,28))
    axes.axis('off')
    axes.set_title(f'\nreal:{test_lbl[i]}  KNN:{log_pred[i]}  Log:{knn_pred[i]}')
plt.show()

