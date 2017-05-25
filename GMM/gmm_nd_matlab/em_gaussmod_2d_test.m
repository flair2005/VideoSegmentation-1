% 程序使用em算法,建立二维混合高斯模型
function [Mu, Sigma, Pi] = em_gaussmod_2d_test(X, K, oMu, oSigma, oPi, display)
% 参数:
%   X: 学习的数据集是,Nx2维矩阵
%   K: 选择要学习的高斯模型的数量,int32
%   display: bool类型,是否演示学习过程
% 输出:
%   Mu: 各模型期望矩阵
%   Sigma: 各模型协方差矩阵
%   Pi: 各模型的权值系数
% 程序中Psi对应二维高斯函数，其中的(x-μ)与其转置的顺序与上述提到的高斯函数不同，这样是为了保证矩阵可乘性，不影响结果.
% Gamma为隐变量值矩阵，Gamma(i,j)代表第i个样本属于第j个模型的概率。
%

[N,D]=size(X);
Gamma=zeros(N,K);
Psi=zeros(N,K);
Mu=oMu;
Sigma =oSigma;
Pi=oPi;

LMu=Mu;
LSigma=Sigma;
LPi=Pi;

step=1;

while true
    % Estimation Step
    for k = 1:K
        Y = X - repmat(Mu(k,:),N,1);
        %Psi第一列代表第一个高斯分布对所有数据的取值
        Psi(:,k) = (2*pi)^(-D/2)*det(Sigma(:,:,k))^(-1/2)*diag(exp(-1/2*Y/(Sigma(:,:,k))*(Y')));
    end
%     Gamma_SUM=zeros(D,D);
    for j = 1:N
        for k=1:K
            Gamma(j,k) = Pi(1,k)*Psi(j,k)/sum(Psi(j,:)*Pi');
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
    
    step = step+1;
%     if (R<1e-10)
    if step==5
        if display == true
            disp('期望');
            disp(Mu);
            disp('协方差矩阵');
            disp(Sigma);
            disp('权值系数');
            disp(Pi);
        end
        break;
    end
    LMu=Mu;
    LSigma=Sigma;
    LPi=Pi;
    
    if display == true
        obj=gmdistribution(Mu,Sigma);
        %     figure,h = ezmesh(@(x,y)pdf(obj,[x,y]),[-8 6], [-8 6]);
        figure(1);
        ezmesh(@(x,y)pdf(obj,[x,y]),[0 255], [0 255]);
        pause(0.1);
    end
end