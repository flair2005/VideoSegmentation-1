MU1 = [1 2];
SIGMA1 = [1 0; 0 0.5];
MU2 = [-1 -1];
SIGMA2 = [1 0; 0 1];
MU3 = [4 -4];
SIGMA3 = [1 0; 0 1];
% 生成1000行2列(默认)均值为mu标准差为sigma的正态分布随机数
X = [mvnrnd(MU1, SIGMA1, 100);mvnrnd(MU2, SIGMA2, 100);mvnrnd(MU3, SIGMA3, 100)];
% 显示样本点
figure(2);
scatter(X(:,1),X(:,2),10,'.');

[Mu, Sigma,Pi] = em_gaussmod_2d(X,1,true);