% 程序使用em算法,建立二维混合高斯模型
% 程序中Psi对应二维高斯函数，其中的(x-μ)与其转置的顺序与上述提到的高斯函数不同，这样是为了保证矩阵可乘性，不影响结果.
% Gamma为隐变量值矩阵，Gamma(i,j)代表第i个样本属于第j个模型的概率。
% Mu为期望，Sigma为协方差矩阵
% LM???
% Pi为各模型的权值系数
% 产生2个二维正态数据
MU1 = [1 2];
SIGMA1 = [1 0; 0 0.5];
MU2 = [-1 -1];
SIGMA2 = [1 0; 0 1];
MU3 = [4 -4];
SIGMA3 = [1 0; 0 1];
% 生成1000行2列(默认)均值为mu标准差为sigma的正态分布随机数
X = [mvnrnd(MU1, SIGMA1, 1000);mvnrnd(MU2, SIGMA2, 1000);mvnrnd(MU3, SIGMA3, 1000)];
% 显示样本点
figure(1);
scatter(X(:,1),X(:,2),10,'.');
% ====================
K=3;
[N,D]=size(X);
Gamma=zeros(N,K);
Psi=zeros(N,K);
Mu=zeros(K,D);
% LM=zeros(K,D);
Sigma =zeros(D, D, K);
Pi=zeros(1,K);
% 选择随机的K个样本点作为期望迭代初值
for k=1:K
    if k==1
        range_start =1;
    else
        range_start =(k-1)*int32(N/k);
    end
    range_end = (k)*int32(N/K);
    Mu(k,:)=X(randi([range_start range_end],1,1),:);
end
% 所有数据的协方差作为协方差初值
for k=1:K
    Pi(k)=1/K;
    Sigma(:, :, k)=cov(X);
end
LMu=Mu;
LSigma=Sigma;
LPi=Pi;

while true
    % Estimation Step
    for k = 1:K
        Y = X - repmat(Mu(k,:),N,1);
        Psi(:,k) = (2*pi)^(-D/2)*det(Sigma(:,:,k))^(-1/2)*diag(exp(-1/2*Y/(Sigma(:,:,k))*(Y')));      %Psi第一列代表第一个高斯分布对所有数据的取值
    end
    Gamma_SUM=zeros(D,D);
    for j = 1:N
        for k=1:K
            Gamma(j,k) = Pi(1,k)*Psi(j,k)/sum(Psi(j,:)*Pi');                                               %Psi的第一行分别代表两个高斯分布对第一个数据的取值
        end
    end
    % Maximization Step
    for k = 1:K
        % update Mu
        Mu_SUM= zeros(1,D);
        for j=1:N
            Mu_SUM=Mu_SUM+Gamma(j,k)*X(j,:);
        end
        Mu(k,:)= Mu_SUM/sum(Gamma(:,k));
        % update Sigma
        Sigma_SUM= zeros(D,D);
        for j = 1:N
            Sigma_SUM = Sigma_SUM+ Gamma(j,k)*(X(j,:)-Mu(k,:))'*(X(j,:)-Mu(k,:));
        end
        Sigma(:,:,k)= Sigma_SUM/sum(Gamma(:,k));
        % update Pi
        Pi_SUM=0;
        for j=1:N
            Pi_SUM=Pi_SUM+Gamma(j,k);
        end
        Pi(1,k)=Pi_SUM/N;
    end
    
    R_Mu=sum(sum(abs(LMu- Mu)));
    R_Sigma=sum(sum(abs(LSigma- Sigma)));
    R_Pi=sum(sum(abs(LPi- Pi)));
    R=R_Mu+R_Sigma+R_Pi;
    
    obj=gmdistribution(Mu,Sigma);
%     figure,h = ezmesh(@(x,y)pdf(obj,[x,y]),[-8 6], [-8 6]);
    figure(2);
    ezmesh(@(x,y)pdf(obj,[x,y]),[-8 6], [-8 6]);
    
    if ( R<1e-10)
        disp('期望');
        disp(Mu);
        disp('协方差矩阵');
        disp(Sigma);
        disp('权值系数');
        disp(Pi);
%         obj=gmdistribution(Mu,Sigma);
%         figure,h = ezmesh(@(x,y)pdf(obj,[x,y]),[-8 6], [-8 6]);
        break;
    end
    LMu=Mu;
    LSigma=Sigma;
    LPi=Pi;
    
    pause(0.1);
end
%=====================


%%%%%%%%运行结果%%%%%%%
%
% 期望
%    1.0334    2.0479
%    -0.9703   -0.9879
%
% 协方差矩阵
%
% (:,:,1) =
%
%     0.9604   -0.0177
%    -0.0177    0.4463
%
%
% (:,:,2) =
%
%     0.9976    0.0667
%     0.0667    1.0249
%
% 权值系数
%     0.4935    0.5065
%%%%%%%%%%%%%%%%%%%%%%%%%