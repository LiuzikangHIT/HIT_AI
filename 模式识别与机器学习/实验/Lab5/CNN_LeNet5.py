import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torch.utils.data.dataset import Subset
import torch.optim as optim
from torchvision import transforms, datasets
import matplotlib.pyplot as plt
import numpy as np
import time


class LeNet5(nn.Module):
    def __init__(self, activation=nn.Sigmoid(), p=0.2):
        super(LeNet5, self).__init__()
        self.conv1 = nn.Conv2d(in_channels=1, out_channels=6, kernel_size=5)
        self.activation = activation
        self.maxpool1 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.conv2 = nn.Conv2d(in_channels=6, out_channels=16, kernel_size=5)
        self.maxpool2 = nn.MaxPool2d(kernel_size=2, stride=2)
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.dropout = nn.Dropout(p)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 10)

    def forward(self, x):
        # CONV1, ReLU1, POOL1
        x = self.conv1(x)
        x = self.maxpool1(x)
        x = self.activation(x)
        # CONV2, ReLU2, POOL2
        x = self.conv2(x)
        x = self.maxpool2(x)
        x = self.activation(x)
        x = x.view(-1, 16 * 5 * 5)
        # FC1
        x = self.fc1(x)
        x = self.activation(x)
        x = self.dropout(x)
        # FC2
        x = self.fc2(x)
        x = self.activation(x)
        x = self.dropout(x)
        # FC3
        x = self.fc3(x)
        output = F.log_softmax(x, dim=1)
        return output

# 随机选择样本
def create_subset(data, sample_size):
    indices = np.random.choice(len(data), sample_size, replace=False)  # 随机选择样本索引
    return Subset(data, indices)

# 训练
def train(model, device, train_loader, optimizer, epoch):
    model.train()
    correct, loss, train_loss = 0, 0, 0
    sample_size = len(train_loader.dataset) # 训练集样本总数
    for batch_idx, (data, target) in enumerate(train_loader):
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)    # 前向传播，保存训练结果
        pred = output.argmax(dim=1)
        correct += pred.eq(target.view_as(pred)).sum().item()

        loss = F.cross_entropy(output, target)  # 交叉熵损失函数
        train_loss += loss.item()
        loss.backward()     # 反向传播
        optimizer.step()    # 更新参数

        if batch_idx % 50 == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                epoch, batch_idx * len(data), sample_size,
                100. * batch_idx / len(train_loader), loss.item()))

    train_loss /= sample_size
    train_acc = correct / sample_size
    return train_loss, train_acc

# 测试
def test(model, device, test_loader):
    model.eval()
    test_loss, correct = 0, 0
    sample_size = len(test_loader.dataset)  # 测试集样本总数
    # 仅预测结果，不计算梯度和更新参数
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)

            test_loss += F.cross_entropy(output, target).item()
            pred = output.argmax(dim=1, keepdim=True)
            correct += pred.eq(target.view_as(pred)).sum().item()

    test_loss /= sample_size
    print('Test set: Average loss: {:.6f}, Accuracy: {}/{} ({:.2f}%)\n'.format(
        test_loss, correct, sample_size,
        100. * correct / sample_size))

if __name__ == '__main__':
    totalLoss, totalAccuracy = [], []
    epochs = 10
    transform = transforms.Compose([
        transforms.Resize((32, 32)),
        transforms.ToTensor(),
        transforms.Normalize((0.1307,),(0.3081,))
    ])

    # 获取MNIST数据集
    sample_sizes = [10000, 30000, 60000]
    train_data = datasets.MNIST(root="./data", train=True, download=True, transform=transform)
    create_subset(train_data, sample_sizes[0])
    test_data = datasets.MNIST(root="./data", train=False, download=True, transform=transform)

    # 加载数据集
    batch_sizes = [64, 128, 256, 512, 1024]
    train_dataloader = DataLoader(dataset=train_data, batch_size=batch_sizes[1], shuffle=True)
    test_dataloader = DataLoader(dataset=test_data, batch_size=32, shuffle=False)

    # 创建模型，部署GPU
    activation = [nn.Sigmoid(), nn.ReLU(), nn.Tanh()]   # 不同激活函数
    dropout_p = [0, 0.1, 0.2, 0.3, 0.4, 0.5]    # 不同dropout比例
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = LeNet5(activation=activation[1], p=dropout_p[1]).to(device)

    # 优化器
    lrs = [1e-5, 1e-4, 1e-3, 1e-2, 1e-1]    # 不同学习率
    optimizer = optim.Adam(model.parameters(), lr=lrs[1])

    # 训练
    Loss, Accuracy = [], []
    print(f"Start Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}")
    for epoch in range(1, epochs+1):
        loss, acc = train(model, device, train_dataloader, optimizer, epoch)
        Loss.append(loss)
        Accuracy.append(acc)
    print(f"End Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}")
    totalLoss.append(Loss)
    totalAccuracy.append(Accuracy)

    # 测试
    test(model, device, test_dataloader)

    # 可视化
    x_ticks = np.arange(1, epochs+1)
    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    axes[0].plot(x_ticks, totalLoss[0])
    axes[0].set_title('Training Loss')
    axes[0].set_xlabel('Epoch')
    axes[0].set_ylabel('Loss')
    axes[0].set_xticks(x_ticks)
    axes[0].legend()
    axes[1].plot(x_ticks, totalAccuracy[0])
    axes[1].set_title('Training Accuracy')
    axes[1].set_xlabel('Epoch')
    axes[1].set_ylabel('Accuracy')
    axes[1].set_xticks(x_ticks)
    axes[1].legend()

    plt.show()