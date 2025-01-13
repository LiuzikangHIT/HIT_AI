import numpy as np
import matplotlib.pyplot as plt

np.random.seed(16)  # 设置随机数种子，便于结果复现

# 多项式函数，degree为最高阶数
def polynomial(x, coef, degree):
    y = 0
    for i in range(degree + 1):
        y += coef[i] * x ** i
    return y

# 损失函数MSE，flag表示是否加入正则化项
def loss(y_true, y_pred, flag=False):
    if flag:
        return np.mean((y_true - y_pred) ** 2) + np.sum(lamda * coef ** 2)
    return np.mean((y_true - y_pred) ** 2)

# 计算梯度，flag表示是否加入正则化项
def gradient(x, y_true, y_pred, degree, flag=False):
    grad = []
    if flag:
        for i in range(degree + 1):
            grad.append(-2 * np.sum((y_true - y_pred) * x ** i) / len(x) + 2 * lamda * coef[i])
    else:
        for i in range(degree + 1):
            grad.append(-2 * np.sum((y_true - y_pred) * x ** i) / len(x))
    return np.array(grad)


if __name__ == '__main__':
    # 样本量，阶数
    sample_num = 10
    degrees = [3, 4, 5, 10]

    # 正弦函数
    x = np.linspace(-1, 1, sample_num)
    y = np.sin(x * np.pi)

    # 高斯噪声
    sigma = 0.05
    y_noise = y + np.random.normal(0, sigma, y.shape)

    # 测试集（训练集两侧扩充）
    x_test = np.linspace(-1.2, 1.2, int(sample_num * 1.2))
    y_test = np.sin(x_test * np.pi)
    y_test_noise = y_test + np.random.normal(0, sigma, y_test.shape)
    y_pred_all = []     # 不同阶数的拟合函数

    # 梯度下降法求解系数
    epochs = 10000  # 迭代次数
    lr = 0.1        # 学习率
    flag = False    # 是否加入惩罚项
    lamda = 0.003   # 惩罚项系数
    y_fit = np.zeros(sample_num)

    for degree in degrees:  # 不同阶数的拟合
        coef = np.zeros(degree + 1)
        for epoch in range(epochs):
            y_fit = polynomial(x, coef, degree)                     # 多项式拟合函数
            loss_value = loss(y_noise, y_fit, flag)                 # 计算loss值
            coef -= lr * gradient(x, y_noise, y_fit, degree, flag)  # 梯度下降

            if epoch % 2000 == 0:
                print(f"Epoch {epoch}, Loss: {loss_value}")

        y_pred = polynomial(x_test, coef, degree)
        y_pred_all.append(y_pred)

        print(f"阶数为{degree}时，拟合函数为：y = {coef[degree]:.3f}x^{degree}", end=' ')
        for i in range(degree-1, 1, -1):
            if coef[i] < 0:
                print(f"- {-coef[i]:.3f}x^{i}", end=' ')
            else:
                print(f"+ {coef[i]:.3f}x^{i}", end=' ')
        if coef[-2] < 0:
            print(f"- {-coef[-2]:.3f}x", end=' ')
        else:
            print(f"+ {coef[-2]:.3f}x", end=' ')
        if coef[-1] < 0:
            print(f"- {-coef[-1]:.3f}")
        else:
            print(f"+ {coef[-1]:.3f}")
        print(f"最终误差为：{loss(y_noise, y_fit, flag):.6f}")
        print("-------------------------------------------------------------------------------------")

    # 绘图
    fig, ax = plt.subplots()
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    plt.plot(x_test, y_test, label='y=sin(x)')                  # 原始函数
    plt.scatter(x_test, y_test_noise, s=10, label='y_noise')    # 加入高斯噪声的样本点
    for degree, y_pred in zip(degrees, y_pred_all):             # 不同阶数的拟合函数
        plt.plot(x_test, y_pred, label='y_fit of degree' + str(degree))
    plt.legend()
    plt.xlabel('x')
    plt.ylabel('y')
    plt.grid()
    plt.show()

