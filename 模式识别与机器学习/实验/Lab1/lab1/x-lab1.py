# --*-- coding:utf-8 --*--
"""

"""
import numpy as np
import matplotlib.pyplot as plt


# 画出sin函数
def plot_sin():
	x = np.arange(0, 1, 0.01)
	y = np.sin(2 * np.pi * x)
	plt.plot(x, y, label='sin(2*pi*x)')


# 生成数据（高斯噪声）
def generate_data(m=8, num=20, var=0.01, mean=0, left=0, right=1, plot=True):
	x = np.linspace(left, right, num)
	Y = np.sin(2 * np.pi * x)
	Y += np.random.normal(mean, var ** 0.5, num)
	X = np.zeros((m + 1, num))
	for i in range(m + 1):
		X[i] = x ** i
	if plot:
		plt.plot(x, Y, 'bo')
	Y = Y.reshape(1, num)
	# print(Y.shape)
	return X, Y


# 求解梯度
def gradient_theta(x_train, y_train, theta, lamuda):
	gradient = np.dot(np.dot(theta, x_train) - y_train, x_train.T) + np.sum(lamuda * theta)
	return gradient


# 损失函数
def poly_cost(x_train, y_train, theta):
	return 0.5 * np.sum((np.dot(theta, x_train) - y_train) ** 2)


def least_square(x_train, y_train):
	theta = np.linalg.inv(np.dot(x_train, x_train.T)).dot(x_train).dot(y_train.T).reshape(1, -1)
	# print(np.linalg.inv(np.dot(x_train, x_train.T)))
	# print(theta)
	return theta


# 梯度下降
def gradient_descent(x_train, y_train, alpha=0.01, m=8, iter=100000, lamuda=1, threshold=1e-5):
	"""
	:param threshold
	:param m: degree
	:param x_train: shape(m+1, num)
	:param y_train: shape(1, num)
	:param alpha: learning rate
	:param iter: iteration
	:param lamuda: penalty term
	:return: theta
	"""
	# 参数初始化
	# theta: shape(1, m+1)
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
		# if i % 1000 == 0:
			# print(f'iter:\t{i},\tcost:\t{cost}')
	print(f'拟合的参数theta分别为：', end='')
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


# 测试学习率变化
def test_alpha():
	num = 20
	m = 8
	var = 0.01
	iteration = 100000
	learning_rate = [0.01, 0.02, 0.03, 0.04]
	lamuda = 1
	plot_sin()
	x_train, y_train = generate_data(m=m, var=var, num=num)
	for alpha in learning_rate:
		theta = gradient_descent(x_train, y_train, m=m, lamuda=lamuda, iter=iteration, alpha=alpha)
		test(theta, label='alpha='+f'{alpha}')
	plt.legend()
	plt.show()


# 测试不同迭代次数
def test_iter():
	num = 40
	m = 8
	var = 0.01
	iteration = [1000, 10000, 50000, 100000]
	learning_rate = 0.01
	lamuda = 1
	plot_sin()
	x_train, y_train = generate_data(m=m, var=var, num=num)
	for iter in iteration:
		theta = gradient_descent(x_train, y_train, m=m, lamuda=lamuda, iter=iter, alpha=learning_rate)
		test(theta, label='iter='+f'{iter}')
	plt.legend()
	plt.show()


# 测试训练样本个数
def test_num():
	nums = [5, 10, 20, 40]
	plot_sin()
	for num in nums:
		x_train, y_train = generate_data(num=num, plot=False)
		theta = gradient_descent(x_train, y_train)
		test(theta, label='num='+f'{num}')
	plt.legend()
	plt.show()


# 测试多项式阶数
def test_degree():
	plot_sin()
	degree = [5, 10, 20, 100]
	for m in degree:
		x_train, y_train = generate_data(m=m, plot=False)
		theta = gradient_descent(x_train, y_train, m=m)
		test(theta, label='degree=' + f'{m}')
	plt.legend()
	plt.show()


# 测试惩罚项
def test_lamuda():
	plot_sin()
	m = 20
	lamudas = [0.1, 0.5, 1, 2]
	iter = 100000
	threshold = 1e-8
	num = 20
	x_train, y_train = generate_data(num=num, m=20)
	for lamuda in lamudas:
		theta = gradient_descent(x_train, y_train, lamuda=lamuda, threshold=threshold, m=20, iter=iter)
		test(theta, label='lamuda=' + f'{lamuda}')
	plt.legend()
	plt.show()


# 测试主函数
def test_main():
	num = 20
	m = 8
	var = 0.01
	iteration = 100000
	learning_rate = 0.05
	lamuda = 1
	plot_sin()
	x_train, y_train = generate_data(m=m, var=var, num=num)
	# theta = least_square(x_train, y_train)
	theta = gradient_descent(x_train, y_train, m=m, lamuda=lamuda, iter=iteration, alpha=learning_rate)
	test(theta)
	test_degree()
	plt.legend()
	plt.show()


if __name__ == '__main__':
	test_alpha()
	test_degree()
