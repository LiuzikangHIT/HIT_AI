import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torch.utils.data.dataset import Subset
from torchvision import datasets, transforms
from torchvision.utils import save_image
import numpy as np
import matplotlib.pyplot as plt
import time


class VAE(nn.Module):
    def __init__(self, latent_dim=20, activation=nn.ReLU(), p=0.1):
        super(VAE, self).__init__()
        self.latent_dim = latent_dim    # 潜在空间维度
        self.activation = activation
        self.dropout = nn.Dropout(p)

        # 编码器
        self.fc1 = nn.Linear(28 * 28, 400)
        self.fc2_mu = nn.Linear(400, latent_dim)
        self.fc2_logvar = nn.Linear(400, latent_dim)

        # 解码器
        self.fc3 = nn.Linear(latent_dim, 400)
        self.fc4 = nn.Linear(400, 28 * 28)

    # 编码
    def encode(self, x):
        h1 = self.activation(self.fc1(x))
        h1 = self.dropout(h1)
        return self.fc2_mu(h1), self.fc2_logvar(h1)

    # 重参数化
    def reparameterize(self, mu, logvar):
        std = torch.exp(0.5 * logvar)   # 标准差
        eps = torch.randn_like(std)     # 高斯噪声
        return mu + eps * std

    # 解码
    def decode(self, z):
        h3 = self.activation(self.fc3(z))
        h3 = self.dropout(h3)
        return torch.sigmoid(self.fc4(h3))

    def forward(self, x):
        mu, logvar = self.encode(x)
        z = self.reparameterize(mu, logvar)
        return self.decode(z), mu, logvar

# 随机选择样本
def create_subset(data, sample_size):
    indices = np.random.choice(len(data), sample_size, replace=False)  # 随机选择样本索引
    return Subset(data, indices)

def loss_function(recon_x, x, mu, logvar):
    BCE = F.binary_cross_entropy(recon_x, x, reduction='sum')
    KLD = -0.5 * torch.sum(1 + logvar - mu.pow(2) - logvar.exp())
    return BCE + KLD

def train(model, device, train_loader, optimizer, epoch):
    model.train()
    train_loss = 0
    for batch_idx, (data, _) in enumerate(train_loader):
        data = data.to(device)
        optimizer.zero_grad()   # 梯度初始化
        recon_batch, mu, logvar = model(data)   # 前向传播
        loss = loss_function(recon_batch, data, mu, logvar) # 计算损失
        loss.backward() # 反向传播
        train_loss += loss.item()
        optimizer.step()    # 更新参数
        if batch_idx % 50 == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.4f}'.format(
                epoch, batch_idx * len(data), len(train_loader.dataset),
                100. * batch_idx / len(train_loader), loss.item() / len(data)))

    train_loss /= len(train_loader.dataset)
    print(f'====> Epoch: {epoch} Average loss: {train_loss:.4f}')
    return train_loss

# 生成图像
def generate_images(model, device, n_images=16, latent_dim=20, param=None, value=None):
    model.eval()
    with torch.no_grad():
        # 从标准正态分布中采样
        z = torch.randn(n_images, latent_dim, device=device)
        sample = model.decode(z).cpu()
        save_image(sample.view(n_images, 1, 28, 28), f'image_{param, value}.png')

if __name__ == "__main__":
    epoch = 10
    Loss = []
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Lambda(lambda x: x.view(-1))
    ])

    # 获取MNIST数据集
    sample_sizes = [10000, 30000, 60000]
    train_dataset = datasets.MNIST(root="./data", train=True, download=True, transform=transform)

    # 加载数据集
    batch_sizes = [64, 128, 256, 512, 1024]
    train_loader = DataLoader(dataset=create_subset(train_dataset, sample_sizes[2]), batch_size=batch_sizes[1], shuffle=True)

    # 部署GPU，创建VAE模型实例
    activation = [nn.ReLU(), nn.Sigmoid(), nn.Tanh()]
    dropout_p = [0, 0.1, 0.2, 0.3, 0.4, 0.5]  # 不同dropout比例
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = VAE(activation=activation[0], p=dropout_p[0]).to(device)

    # 创建优化器
    lrs = [1e-5, 1e-4, 1e-3, 0.01, 0.1]  # 不同学习率
    optimizer = optim.Adam(model.parameters(), lr=lrs[3])

    # 模型训练
    train_losses = []
    print(f"Start Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}")
    for i in range(epoch):
        train_loss = train(model, device, train_loader, optimizer, i)
        train_losses.append(train_loss)

    # 生成图像
    generate_images(model, device, n_images=16, param='sample')
    print(f"End Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}\n")
    Loss.append(train_losses)

    # 绘制训练损失曲线
    x_ticks = np.arange(1, epoch + 1)
    plt.plot(x_ticks, Loss[0])
    plt.xticks(x_ticks)
    plt.xlabel('Epoch')
    plt.ylabel('Train loss')
    plt.title('Train loss curve')
    # plt.legend()
    plt.show()
