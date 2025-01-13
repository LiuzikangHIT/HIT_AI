# -*- coding: utf-8 -*-
"""
Created on Sun Oct 15 19:42:24 2023

@author: DELL
"""

import numpy as np
import matplotlib.pyplot as plt



#画函数
def plot_sin():
    x = np.arange(0,1,0.01)
    y = np.sin(2*np.pi*x)
    plt.plot(x,y,label='sin(2*pi*x)')
    
#数据
def genetate_data(m=8, num=20,var=0.01,mean=0,left=0,right=1,plot=True):
    x = np.linspace(left,right,num)
    Y = np.sin(2 * np.pi * x)
    Y += np.random.normal(mean,var ** 0.5,num)
    X = np.zeros((m+1,num))
    for i in range(m+1):
        X[i] = x**i
    Y =Y.reshape(1,num)
    return X,Y

def gradient_theta(x_train,y_train,theta,lamuda):
    gradient = np.dot(np.dot(theta,x_train)-y_train,x_train.T) + np.sum(lamuda * theta)
    return gradient

def poly_cost(x_train,y_train,theta):
    return 0.5*np.sum((np.dot(theta,x_train)-y_train)**2)

def least_square(x_train,y_train):
    theta = np.linalg.inv(np.dot(x_train,x_train.T)).dot(x_train).dot(y_train.T).reshape(1,-1)
    return theta

# 梯度下降
def gradient_descent(x_train, y_train, alpha=0.01, m=8, iter=100000, lamuda=1, threshold=1e-5):

	theta = np.random.rand(m + 1).reshape(1, m + 1)
	cost = 1e10
	for i in range(iter):
		# 求解当前梯度
		gradient = gradient_theta(x_train, y_train, theta, lamuda)
		# 更新参数
		theta = theta - alpha * gradient
		# 结束迭代的条件
		if abs(cost - poly_cost(x_train, y_train, theta)) < threshold:
			cost = poly_cost(x_train, y_train, theta)
			print(f'iter:\t{i},\tcost:\t{cost}')
			break
		# 计算损失
		cost = poly_cost(x_train, y_train, theta)
		#if i % 1000 == 0:
			 #print(f'iter:\t{i},\tcost:\t{cost}')
	#print(f'拟合的参数theta分别为：', end='')
	for t in theta[0]:
		t = round(t, 5)
		print(t, end=' ')
	print()
	return theta
# 测试拟合效果
def test(theta, num=100, left=0, right=1, label="test"):
	de = theta.shape[1]
	# print((de, num))
	x = np.linspace(left, right, num).reshape(1, -1)
	X = np.zeros((de, num))
	for i in range(de):
		X[i] = x ** i
	y = np.dot(theta, X)
	# print(x.shape, y.shape)
	plt.plot(x[0], y[0], label=label)
    
def test_iter():
	num = 50
	m = 8
	var = 0.01
	#iteration = [1000, 10000, 50000, 100000]
	learning_rate = 0.01
	lamuda = 1
	plot_sin()
	x_train, y_train = genetate_data(m=m, var=var, num=num)
	#for iter in iteration:
	theta = gradient_descent(x_train, y_train, m=m, lamuda=lamuda, iter=100000, alpha=learning_rate)
	test(theta, label='iter='+f'{iter}')
	plt.legend()
	plt.show()
    
def test_num():
	nums = [5, 20, 100]
	plot_sin()
	for num in nums:
		x_train, y_train = genetate_data(num=num, plot=False)
		theta = gradient_descent(x_train, y_train)
		test(theta, label='num='+f'{num}')
	plt.legend()
	plt.show()

# 测试多项式阶数
def test_degree():
	plot_sin()
	degree = [5, 10, 20, 50,100]
	for m in degree:
		x_train, y_train = genetate_data(m=m, plot=False)
		theta = gradient_descent(x_train, y_train, m=m)
		test(theta, label='degree=' + f'{m}')
	plt.legend()
	plt.show()
def test_lamuda():
	plot_sin()
	lamudas = [0,1]
	iter = 100000
	threshold = 1e-8
	num = 10
	x_train, y_train = genetate_data(num=num, m=8)
	for lamuda in lamudas:
		theta = gradient_descent(x_train, y_train, lamuda=lamuda, threshold=threshold, m=8, iter=iter)
		test(theta, label='lamuda=' + f'{lamuda}')
	plt.legend()
	plt.show()
 
if __name__ =='__main__':
    plot_sin()
    x_train,y_train=genetate_data()
    theta = gradient_descent(x_train, y_train)
    #test(theta)
    #test_iter()
    #test_num()
    test_degree()
    #test_lamuda()
    