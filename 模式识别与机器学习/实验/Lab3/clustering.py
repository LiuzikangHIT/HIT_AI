import numpy as np
import pandas as pd
from scipy.stats import multivariate_normal
from sklearn.metrics import adjusted_rand_score
import matplotlib.pyplot as plt

np.random.seed(21)

# 生成k组高斯分布数据
def generate_data(K, n_samples, mean=None, var=None):
    if mean is None:
        mean = np.random.randint(-5, 5, K * 2).reshape(K, 2)
    if var is None:
        var = np.random.uniform(1, 2, K * 2).reshape(K, 2)
    data = [[], []]
    for i in range(K):
        data[0].extend(np.random.normal(mean[i, 0], var[i, 0], n_samples))
        data[1].extend(np.random.normal(mean[i, 1], var[i, 1], n_samples))

    return np.array(data).T

def Kmeans(K, data):
    n_samples, n_features = data.shape
    result = [[] for _ in range(K)] # 聚类结果
    labels = np.zeros(n_samples, dtype=int)     # 各样本聚类标签
    centers = np.zeros((K, n_features))         # 当前聚类中心
    centers_last = np.zeros((K, n_features))    # 上一次的聚类中心

    # 初始化聚类中心
    sample_size = n_samples // K   # 将数据等距分成K份
    center_ini = sample_size // 2
    for i in range(K):
        centers[i, :] = data[center_ini, :]
        center_ini += sample_size

    while centers.all() != centers_last.all():
        # 计算样本所属簇
        for i in range(n_samples):
            dist = []
            for j in range(K):
                dist.append(np.linalg.norm(data[i] - centers[j]))
            labels[i] = np.argmin(dist)
            result[labels[i]].append(i)

        # 更新聚类中心
        centers_last = centers.copy()
        for i in range(K):
            centers[i, :] = np.mean(data[result[i], :], axis=0)

    return labels

class GMM_EM:
    def __init__(self, K, max_iter=100, tol=1e-6):
        self.K = K
        self.max_iter = max_iter
        self.tol = tol

    # 模型迭代
    def fit(self, X):
        # 初始化模型参数
        n_samples, n_features = X.shape
        self.alphas = np.ones(self.K) / self.K
        self.mus = np.array([X[i] for i in np.random.choice(n_samples, self.K, replace=False)])
        self.sigmas = np.array([np.cov(X.T) for _ in range(self.K)])
        self.likelihood_history = []

        # 迭代过程
        for iter in range(self.max_iter):
            # E-step: 计算每个数据点属于每个高斯分布的后验概率
            responsiveness = self._e_step(X)

            # M-step: 更新模型参数
            self._m_step(X, responsiveness)

            # 计算对数似然函数
            likelihood = self.log_likelihood(X)
            self.likelihood_history.append(likelihood)
            # print(f"Iter {iter + 1}, Likelihood: {likelihood}")

            # 判断是否收敛
            if iter > 0 and abs(self.likelihood_history[-1] - self.likelihood_history[-2]) < self.tol:
                break

        return self

    # E-Step：计算每个数据点属于每个高斯分布的后验概率
    def _e_step(self, X):
        responsiveness = np.zeros((X.shape[0], self.K))
        for i in range(self.K):
            rv = multivariate_normal(mean=self.mus[i], cov=self.sigmas[i])
            responsiveness[:, i] = self.alphas[i] * rv.pdf(X)
        responsiveness /= responsiveness.sum(axis=1, keepdims=True)

        return responsiveness

    # M-Step：更新模型参数
    def _m_step(self, X, responsiveness):
        n_samples = X.shape[0]
        for i in range(self.K):
            n_k = responsiveness[:, i].sum()
            # 计算加权均值
            self.mus[i] = np.sum(responsiveness[:, i][:, np.newaxis] * X, axis=0) / n_k

            # 计算加权协方差
            diff = X - self.mus[i]
            self.sigmas[i] = np.dot((responsiveness[:, i][:, np.newaxis] * diff).T, diff) / n_k

            # 计算加权权重
            self.alphas[i] = responsiveness[:, i].sum() / n_samples

    # 对数似然函数
    def log_likelihood(self, X):
        likelihood = 0
        for i in range(self.K):
            rv = multivariate_normal(mean=self.mus[i], cov=self.sigmas[i])
            likelihood += self.alphas[i] * rv.pdf(X)

        return np.sum(np.log(likelihood))




if __name__ == '__main__':
    K = 3
    n_samples = 100

    # 生成k组高斯分布数据
    data = generate_data(K, n_samples)
    true_labels = [0] * n_samples + [1] * n_samples + [2] * n_samples
    # print(data.shape)

    # K-means聚类算法
    kmeans_labels = Kmeans(K, data)
    ARI = adjusted_rand_score(true_labels, kmeans_labels)
    print("=" * 30 + " K-means聚类结果 " + "=" * 30)
    print(kmeans_labels)
    print(f"调整兰德指数ARI：{ARI:.4f}\n")

    # 高斯混合模型+EM算法
    gmm_em = GMM_EM(K)
    gmm_em.fit(data)
    gmm_labels = np.argmax(gmm_em._e_step(data), axis=1)    # 获取每个点的聚类标签
    ARI = adjusted_rand_score(true_labels, gmm_labels)
    print("=" * 32 + " GMM聚类结果 " + "=" * 32)
    print(gmm_labels)
    print(f"调整兰德指数ARI：{ARI:.4f}\n")

    # 绘图
    cmap = plt.get_cmap("viridis")
    fig1, axs = plt.subplots(1, 3, figsize=(20, 4))
    scatter1 = axs[0].scatter(data[:, 0], data[:, 1], c=true_labels, s=10)
    axs[0].set_title("True Labels")
    scatter2 = axs[1].scatter(data[:, 0], data[:, 1], c=kmeans_labels, s=10)
    axs[1].set_title("K-means Clustering")
    scatter3 = axs[2].scatter(data[:, 0], data[:, 1], c=gmm_labels, s=10)
    axs[2].set_title("GMM Clustering with EM")
    fig1.colorbar(scatter1, ax=axs, fraction=0.02, pad=0.03, ticks=[0, 1, 2, 3])
    # plt.show()


    # UCI鸢尾花数据集
    iris = pd.read_csv('./iris.csv').drop(['Id', 'Species'], axis=1).to_numpy()
    true_labels = [0] * 50 + [1] * 50 + [2] * 50

    gmm_em = GMM_EM(K=3)
    gmm_em.fit(iris)
    gmm_labels = np.argmax(gmm_em._e_step(iris), axis=1)    # 获取每个点的聚类标签
    ARI = adjusted_rand_score(true_labels, gmm_labels)
    print("=" * 27 + " Iris数据集 GMM聚类结果 " + "=" * 27)
    print(gmm_labels)
    print(f"调整兰德指数ARI：{ARI:.4f}")

    # 绘图
    fig2, axs = plt.subplots(1, 2, figsize=(10, 5))
    scatter4 = axs[0].scatter(iris[:, 0], iris[:, 1], c=true_labels, s=10)
    axs[0].set_title("True Labels")
    axs[0].set_xlabel("Feature 1")
    axs[0].set_ylabel("Feature 2")
    scatter5 = axs[1].scatter(iris[:, 0], iris[:, 1], c=gmm_labels, s=10)
    axs[1].set_title("GMM Clustering with EM")
    axs[1].set_xlabel("Feature 1")
    axs[1].set_ylabel("Feature 2")
    fig2.colorbar(scatter4, ax=axs, fraction=0.02, pad=0.03, ticks=[0, 1, 2, 3])
    plt.show()
