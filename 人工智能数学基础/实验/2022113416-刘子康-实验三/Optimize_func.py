import numpy as np
import matplotlib.pyplot as plt
import sympy as sym
from sympy import hessian
from math import pi, cos, sqrt
from scipy.optimize import minimize
import torch
import torch.optim as optim

np.random.seed(17)

# 测试函数1：Booth函数（极小点在(1,3),最优值为0）
def Booth(x, type=0):
    if type:
        x1, x2 = sym.symbols('x1 x2')
        f = (x1 + 2*x2 - 7) ** 2 + (2*x1 + x2 - 5) ** 2
        return x1, x2, f
    else:
        return (x[0] + 2*x[1] - 7) ** 2 + (2*x[0] + x[1] - 5) ** 2

# 测试函数2：Rosenbrock Banana函数
def Banana(x, type=0):
    if type:
        x1, x2 = sym.symbols('x1 x2')
        f = (1 - x1) ** 2 + 5 * (x2 - x1**2) ** 2
        return x1, x2, f
    else:
        return (1 - x[0]) ** 2 + 5 * (x[1] - x[0]**2) ** 2

# Goldstein规则
def Goldstein_rule(f, x, x_, index, rho=0.1, sigma=0.9):
    s = x_ - x
    x1, x2, expr = f(x, 1)
    gradient_vector = sym.Matrix([sym.diff(expr, x1), sym.diff(expr, x2)])
    different = np.dot(np.array([grad for grad in gradient_vector.subs({x1:x[0], x2:x[1]})]).astype(float), s)
    if index == 1:
        if f(x_) - f(x) <= rho * different:
            return True
    elif index == 2:
        if f(x_) - f(x) >= sigma * different:
            return True
    return False

# 求步长λ
def step(f, x, x_, alpha=1.5, beta=0.5):
    lamda = 0.01
    while True:
        if Goldstein_rule(f, x, x_, 1):
            if Goldstein_rule(f, x, x_, 2):
                break
            else:
                lamda = alpha * lamda
        else:
            lamda = beta * lamda
        # print(lamda)
    return lamda

# 梯度下降法
def gradient_descent(f, x, epsilon=1e-6, lamda=0.01, max_iter=1000):
    x_k = x.copy()
    iter = 0
    # 求梯度向量
    x1, x2, expr = f(x, 1)
    gradient_vector = sym.Matrix([sym.diff(expr, x1), sym.diff(expr, x2)])
    # 迭代过程
    while sqrt(sum(i**2 for i in gradient_vector.subs({x1:x_k[0], x2:x_k[1]}))) > epsilon and iter <= max_iter:
        p = -1 * np.array([grad for grad in gradient_vector.subs({x1:x_k[0], x2:x_k[1]})]).astype(float)
        x_k += lamda * p
        iter += 1

    return x_k, f(x_k)

# 牛顿法
def newton(f, x, epsilon=1e-6, lamda=0.1, max_iter=1000):
    x_k = x.copy()
    iter = 0
    # 求梯度向量和Hessian矩阵
    x1, x2, expr = f(x, 1)
    gradient_vector = sym.Matrix([sym.diff(expr, x1), sym.diff(expr, x2)])
    Hessian = hessian(expr, (x1, x2))
    # 迭代过程
    while sqrt(sum(i ** 2 for i in gradient_vector.subs({x1: x_k[0], x2: x_k[1]}))) > epsilon and iter <= max_iter:
        p = -1 * np.array([grad for grad in gradient_vector.subs({x1: x_k[0], x2: x_k[1]})]).astype(float)
        x_k += lamda * np.dot(p, np.linalg.inv(np.array([i for i in Hessian.subs({x1: x_k[0], x2: x_k[1]})]).astype(float).reshape((2, -1))))
        iter += 1

    return x_k, f(x_k)

# 使用PyTorch的SGD优化器
def pytorch_SGD(f, lr=0.01, max_iter=1000):
    x1 = torch.tensor([np.random.uniform(-10, 10)], requires_grad=True)
    x2 = torch.tensor([np.random.uniform(-10, 10)], requires_grad=True)
    params = [x1, x2]
    y = f(params)
    optimizer = optim.SGD(params, lr=lr)
    # 梯度下降迭代过程
    for i in range(max_iter):
        optimizer.zero_grad()
        y.backward()
        optimizer.step()
        y = f(params)
        # if (i + 1) % 10 == 0:
        #     print(f"Iteration {i + 1}, x = {x.item()}, f(x) = {y.item()}")

    return np.array([x1.item(), x2.item()]), y.item()

test_func = Banana

x = np.random.uniform(-10, 10, size=2)
print(f"初始点：{x}")

x_res_grad, min_val_grad = gradient_descent(test_func, x, lamda=0.001, max_iter=10000)
print(f"梯度下降法 极小点：{x_res_grad}，最优值：{min_val_grad}")

x_res_newton, min_val_newton = newton(test_func, x, lamda=0.01, max_iter=10000)
print(f"牛顿法 极小点：{x_res_newton}，最优值：{min_val_newton}")

res = minimize(test_func, x)
print(f"Scipy库函数 极小点：{res.x}，最优值：{res.fun}")

x_res_SGD, min_val_SGD = pytorch_SGD(test_func, lr=0.01, max_iter=10000)
print(f"PyTorch库函数(SGD) 极小点：{x_res_SGD}，最优值：{min_val_SGD}")
