import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.datasets import make_classification


random_seed = 43
np.random.seed(random_seed)

# 生成二分类数据，flag表示是否满足朴素贝叶斯假设
def generate_data(n_samples=1000, flag=True):
    global random_seed
    if flag:
        X, y = make_classification(n_samples=n_samples, n_features=2, n_informative=2, n_redundant=0, n_clusters_per_class=1, random_state=random_seed)
        X[:, 0], X[:, 1] = X[:, 0] + np.random.normal(0, 0.36, n_samples), X[:, 1] + np.random.normal(0, 0.36, n_samples)
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25, random_state=random_seed)
    else:
        X, y = make_classification(n_samples=n_samples, n_features=2, n_informative=1, n_redundant=1, n_clusters_per_class=1, random_state=random_seed)
        X[:, 0], X[:, 1] = X[:, 0] + np.random.normal(0, 0.64, n_samples), X[:, 1] + np.random.normal(0, 0.64, n_samples)
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25, random_state=random_seed)
    return X_train, X_test, y_train, y_test

# Sigmoid 激活函数
def sigmoid(z):
    return 1 / (1 + np.exp(-z))

# 交叉熵损失函数Cross-Entropy，flag表示是否加入惩罚项
def ce_loss(y_true, y_pred, w, lamda=0.5, flag=False):
    epsilon = 1e-8  # 防止log(0)问题
    y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
    if flag:
        return -np.mean(y_true * np.log(y_pred) + (1 - y_true) * np.log(1 - y_pred)) + lamda * np.sum(w ** 2)
    return -np.mean(y_true * np.log(y_pred) + (1 - y_true) * np.log(1 - y_pred))

# 梯度计算函数，flag表示是否加入惩罚项
def gradient(X, y_true, y_pred, w, lamda=0.5, flag=False):
    m = y_true.shape[0] # 样本量
    y = y_true.reshape(-1, 1)
    if flag:
        dw = (1/m) * np.dot(X.T, (y_pred - y)) + lamda * w  # 权重的梯度
        db = (1/m) * np.sum(y_pred - y)                     # 偏置的梯度
        return dw, db
    dw = (1/m) * np.dot(X.T, (y_pred - y))  # 权重的梯度
    db = (1/m) * np.sum(y_pred - y)         # 偏置的梯度
    return dw, db

# 逻辑回归模型训练，flag表示是否加入惩罚项
def train_logistic_regression(X, y, lr=0.01, epochs=1000, lamda=0.5, flag=False):
    m, n = X.shape  # m: 样本量, n: 特征数
    # 初始化参数
    w = np.zeros((n, 1))
    b = 0

    # 训练过程
    for epoch in range(epochs):
        # 计算预测值
        z = np.dot(X, w) + b
        y_pred = sigmoid(z)

        # 计算损失和梯度
        loss_value = ce_loss(y, y_pred, w, lamda=lamda, flag=flag)
        dw, db = gradient(X, y, y_pred, w, lamda=lamda, flag=flag)

        # 更新参数
        w -= lr * dw
        b -= lr * db

        # 每迭代1/10打印一次loss
        if epoch % (epochs // 10) == 0:
            print(f"Epoch {epoch}, Loss: {loss_value}")

    return w, b

# 预测函数
def predict(X, w, b):
    z = np.dot(X, w) + b
    y_pred = sigmoid(z)
    return (y_pred >= 0.5).astype(int).reshape(-1)

# 准确率
def accuracy(y_true, y_pred):
    return np.mean(y_true == y_pred)

# 主程序
if __name__ == "__main__":
    is_Bayes = True
    is_punish = False

    ''' 任务1 手动生成数据集，构建并测试逻辑回归模型 '''
    X_train, X_test, y_train, y_test = generate_data(flag=is_Bayes)

    weights, bias = [], []
    for flag in [False, True]:
        # 逻辑回归模型训练
        w, b = train_logistic_regression(X_train, y_train, flag=flag)

        # 结果预测
        y_pred = predict(X_test, w, b)

        # 计算准确率
        acc = accuracy(y_test, y_pred)
        print(f"Test Accuracy: {acc:.3f}")
        print("=" * 50)

        weights.append(w)
        bias.append(b)

    #绘图
    x_class = np.linspace(np.min(X_train[:, 0]), np.max(X_train[:, 0]), 100)
    y_class = [(-w[0] * x_class - b) / w[1] for w, b in zip(weights, bias)]

    plt.scatter(X_train[y_train == 0, 0], X_train[y_train == 0, 1], c= 'red', s=10, label='category_1')
    plt.scatter(X_train[y_train == 1, 0], X_train[y_train == 1, 1], c= 'blue', s=10, label='category_2')
    plt.plot(x_class, y_class[0], color='green', label='classification_line_noL2')
    plt.plot(x_class, y_class[1], color='orange', label='classification_line_L2')
    plt.xlabel('x1')
    plt.ylabel('x2')
    plt.legend()
    plt.show()


    ''' 任务2 使用UCI的Diabetes数据集，构建并测试逻辑回归模型 '''
    # 加载并划分数据集
    diabetes = pd.read_csv('diabetes.csv')
    X = diabetes.drop('Outcome', axis=1).to_numpy()
    y = diabetes['Outcome'].to_numpy()
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25, random_state=random_seed)

    # 逻辑回归模型训练
    w, b = train_logistic_regression(X_train, y_train, lr=0.05, epochs=10000, lamda=0.1, flag=is_punish)

    # 结果预测
    y_pred = predict(X_test, w, b)

    # 计算准确率
    acc = accuracy(y_test, y_pred)
    print(f"Test Accuracy: {acc:.3f}")
