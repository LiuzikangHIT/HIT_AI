import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs
from sklearn.model_selection import train_test_split


np.random.seed(42)

# 生成K个类别二维高斯分布数据
def generate_data(n_samples, K=4):
    X, Y = make_blobs(n_samples=n_samples, n_features=2, centers=K, cluster_std=2.56, random_state=42)
    # 将标签进行 one-hot 编码
    Y = np.eye(K)[Y]
    # 划分数据集
    X_train, X_test, y_train, y_test = train_test_split(X, Y, test_size=0.2)

    return X_train, X_test, y_train, y_test

# 激活函数及其导数
def Sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))

def Sigmoid_derivative(x):
    return x * (1.0 - x)

# 多层感知机模型
class MLP():
    def __init__(self, input_size, hidden_size1, hidden_size2, output_size, learning_rate=1e-3):
        self.input_size = input_size
        self.hidden_size1 = hidden_size1
        self.hidden_size2 = hidden_size2
        self.output_size = output_size
        self.learning_rate = learning_rate

        # 初始化权重和偏置
        self.W1 = np.random.randn(self.input_size, self.hidden_size1)
        self.W2 = np.random.randn(self.hidden_size1, self.hidden_size2)
        self.W3 = np.random.randn(self.hidden_size2, self.output_size)
        self.b1 = np.zeros((1, self.hidden_size1))
        self.b2 = np.zeros((1, self.hidden_size2))
        self.b3 = np.zeros((1, self.output_size))

    # 前向传播
    def forward(self, x):
        self.x = x
        self.h1 = Sigmoid(np.dot(x, self.W1) + self.b1)
        self.h2 = Sigmoid(np.dot(self.h1, self.W2) + self.b2)
        self.y = Sigmoid(np.dot(self.h2, self.W3) + self.b3)
        return self.y

    # 反向传播 更新参数
    def backward(self, y):
        # 输出层误差
        output_error = y - self.y
        output_delta = output_error * Sigmoid_derivative(self.y)

        # 第二个隐藏层误差
        hidden_error2 = np.dot(output_delta, self.W3.T)
        hidden_delta2 = hidden_error2 * Sigmoid_derivative(self.h2)

        # 第一个隐藏层误差
        hidden_error1 = np.dot(hidden_delta2, self.W2.T)
        hidden_delta1 = hidden_error1 * Sigmoid_derivative(self.h1)

        # 更新权重和偏置
        self.W3 += self.learning_rate * np.dot(self.h2.T, output_delta)
        self.b3 += self.learning_rate * np.sum(output_delta, axis=0)
        self.W2 += self.learning_rate * np.dot(self.h1.T, hidden_delta2)
        self.b2 += self.learning_rate * np.sum(hidden_delta2, axis=0)
        self.W1 += self.learning_rate * np.dot(self.x.T, hidden_delta1)
        self.b1 += self.learning_rate * np.sum(hidden_delta1, axis=0)

# 线性分类器
class LinearClassifier():
    def __init__(self, input_size, output_size, learning_rate=1e-3):
        self.input_size = input_size
        self.output_size = output_size
        self.learning_rate = learning_rate

        # 初始化权重和偏置
        self.W = np.random.randn(self.input_size, self.output_size)
        self.b = np.zeros((1, self.output_size))

    # 前向传播
    def forward(self, x):
        self.x = x
        self.y = Sigmoid(np.dot(x, self.W) + self.b)
        return self.y

    # 反向传播 更新参数
    def backward(self, y):
        # 输出层误差
        error = y - self.y
        delta = error * Sigmoid_derivative(self.y)

        # 更新权重和偏置
        self.W += self.learning_rate * np.dot(self.x.T, delta)
        self.b += self.learning_rate * np.sum(delta, axis=0)

# 训练模型
def train(model, X, y, epochs=1000):
    acc, loss = [], []
    for epoch in range(epochs):
        # 前向传播
        y_pred = model.forward(X)
        # 反向传播
        model.backward(y)
        # 输出误差及准确率
        if (epoch + 1) % (epochs // 10) == 0:
            acc.append(test(model, X, y))
            loss.append(np.mean(np.abs(y_pred - y)))
            print(f'Epoch {epoch + 1}, Accuracy: {acc[-1] * 100:.2f}%, Loss: {loss[-1]:.6f}')
    return acc, loss

# 测试模型
def test(model, X, y):
    y_pred = model.forward(X)
    correct = np.sum(np.argmax(y_pred, axis=1) == np.argmax(y, axis=1))
    return correct / len(y)

# 预测值
def pred(model, X):
    y_pred = model.forward(X)
    return np.argmax(y_pred, axis=1)

# 绘制数据分布及决策边界
def plot_decision_boundary(model, X, y):
    h = 0.01  # 网格步长
    color = [np.array(np.where(y[i][:] != 0))[1] for i in range(len(y))]
    fig, axes =  plt.subplots(1, 3, figsize=(18, 5))

    for i in range(3):
        axes[i].scatter(X[i][:, 0], X[i][:, 1], c=color[i], s=10)
        axes[i].set_title('Train Samples: %d' % (X[i].shape[0]))
        axes[i].set_xlabel('Feature 1')
        axes[i].set_ylabel('Feature 2')

        x_min, x_max = X[i][:, 0].min() - 1, X[i][:, 0].max() + 1
        y_min, y_max = X[i][:, 1].min() - 1, X[i][:, 1].max() + 1
        xx, yy = np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h))

        Z = pred(model[i], np.c_[xx.ravel(), yy.ravel()])
        Z = Z.reshape(xx.shape)

        axes[i].contourf(xx, yy, Z, alpha=0.3)

    plt.show()

# 绘制条形图
def plot_bar(acc, x_label, x_ticks):
    bar_width = 0.2 # 条形图宽度
    index = np.arange(len(x_ticks)) * 0.75   # 数据的索引位置
    # MLP 模型
    plt.bar(index - bar_width / 2, acc[0], bar_width, label='MLP', edgecolor='black', zorder=2)
    # 线性分类器
    plt.bar(index + bar_width / 2, acc[1], bar_width, label='Linear Classifier', edgecolor='black', zorder=2)

    plt.xlabel(x_label)
    plt.ylabel('Accuracy')
    plt.title('Comparison of Model Performance')
    plt.xticks(index, x_ticks)
    plt.yticks(np.arange(0, 1.1, 0.1))
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.5, zorder=0)
    plt.show()

if __name__ == '__main__':
    data_sizes = [40, 400, 4000]
    lrs = [1e-5, 1e-4, 1e-3, 1e-2, 1e-1]
    model, X, y = [], [], []

    '''======================================== MLP 训练过程 ========================================'''
    # 不同数据量时样本点分布及决策边界
    for n_samples in data_sizes:
        print("=" * 15 + f'n_samples: {n_samples}' + "=" * 15)
        # 生成数据集
        X_train, X_test, y_train, y_test = generate_data(n_samples=n_samples)
        X.append(X_train)
        y.append(y_train)

        # 多层感知机
        model_MLP = MLP(input_size=X_train.shape[1], hidden_size1=8, hidden_size2=32, output_size=y_train.shape[1])
        acc, loss = train(model_MLP, X_train, y_train)
        model.append(model_MLP)
        acc_MLP = test(model_MLP, X_test, y_test)
        print("Final Accuracy: %.2f%%\n" % (acc_MLP * 100))

        if n_samples == 400:
            epoch = np.linspace(0, 1000, 11)[1:]
            fig, axes = plt.subplots(1, 2, figsize=(12, 5), sharex=True)

            axes[0].plot(epoch, acc)
            axes[0].set_title('MLP: Training Accuracy over Epoch')
            axes[0].set_xlabel('Epoch')
            axes[0].set_ylabel('Accuracy')
            axes[0].set_xticks(epoch)

            axes[1].plot(epoch, loss)
            axes[1].set_xlabel('Epoch')
            axes[1].set_ylabel('Loss')
            axes[1].set_title('MLP: Training Loss over Epoch')
            axes[1].set_xticks(epoch)

            plt.show()

    plot_decision_boundary(model, X, y)

    '''======================================== 不同数据量 ========================================'''
    acc = [[], []]
    for n_samples in data_sizes:
        print("=" * 15 + f'n_samples: {n_samples}' + "=" * 15)
        # 生成数据集
        X_train, X_test, y_train, y_test = generate_data(n_samples=n_samples)
        # print(X_train.shape, y_train.shape, X_test.shape, y_test.shape)

        # 多层感知机
        print("MLP:")
        model_MLP = MLP(input_size=X_train.shape[1], hidden_size1=8, hidden_size2=32, output_size=y_train.shape[1])
        train(model_MLP, X_train, y_train)
        acc_MLP = test(model_MLP, X_test, y_test)
        acc[0].append(acc_MLP)
        print("Final Accuracy: %.2f%%\n" % (acc_MLP * 100))

        # 线性分类器
        print("Linear:")
        model_Linear = LinearClassifier(input_size=X_train.shape[1], output_size=y_train.shape[1])
        train(model_Linear, X_train, y_train)
        acc_Linear = test(model_Linear, X_test, y_test)
        acc[1].append(acc_Linear)
        print("Final Accuracy: %.2f%%\n" % (acc_Linear * 100))

    plot_bar(acc, 'Data Size', data_sizes)

    '''======================================== 不同学习率 ========================================'''
    acc = [[], []]
    for lr in lrs:
        print("=" * 12 + f'learning rate: {lr}' + "=" * 12)
        # 生成数据集
        X_train, X_test, y_train, y_test = generate_data(n_samples=400)
        # print(X_train.shape, y_train.shape, X_test.shape, y_test.shape)

        # 多层感知机
        print("MLP:")
        model_MLP = MLP(input_size=X_train.shape[1], hidden_size1=8, hidden_size2=32, output_size=y_train.shape[1], learning_rate=lr)
        train(model_MLP, X_train, y_train)
        acc_MLP = test(model_MLP, X_test, y_test)
        acc[0].append(acc_MLP)
        print("Final Accuracy: %.2f%%\n" % (acc_MLP * 100))

        # 线性分类器
        print("Linear:")
        model_Linear = LinearClassifier(input_size=X_train.shape[1], output_size=y_train.shape[1], learning_rate=lr)
        train(model_Linear, X_train, y_train)
        acc_Linear = test(model_Linear, X_test, y_test)
        acc[1].append(acc_Linear)
        print("Final Accuracy: %.2f%%\n" % (acc_Linear * 100))

    plot_bar(acc, 'Learning Rate', lr)
