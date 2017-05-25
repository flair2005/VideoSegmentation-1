%本程序中每个像素点使用了三个高斯模型，故设置了三维数组
clear all

dir_path = '/home/yangzheng/testData/mall_dataset/frames/Sample-213x160';
dir_info=dir(fullfile(dir_path,'*.jpg'));
file_array={dir_info.name};

numFrames = length(file_array);
% -----------------------  frame size variables -----------------------
%
%fr = source(1).cdata;           % read in 1st frame as background frame
fr=imread(fullfile(dir_path,file_array{1}));
dim=size(size(fr));
if dim(2)==3
    fr_bw=rgb2gray(fr);
else
    fr_bw=fr;
end
fr_size = size(fr);
width = fr_size(2);
height = fr_size(1);
fg = zeros(height, width); % 用于计算前景，储存前景像素
bg_bw = zeros(height, width);% 用于背景的更新

% --------------------- mog variables -----------------------------------
D_Gauss = 2;                            % 使用几维高斯模型
C = 3;                                  % number of gaussian components (typically 3-5)高斯个数
M = 3;                                  % number of background components背景个数
D = 2.5;                                % positive deviation threshold比较门槛值
alpha = 0.01;                           % learning rate (between 0 and 1) (from paper 0.01)初始化的学习率
thresh = 0.25;                          % foreground threshold (0.25 or 0.75 in paper)前景门槛
sd_init = 6;                            % initial standard deviation (for new components) var = 36 in paper初始化偏差
weight = zeros(height,width,C,D_Gauss);              % initialize weights array初始化权重
mean = zeros(height,width,C,D_Gauss);           % pixel means像素均值
sd = zeros(height,width,C,D_Gauss);             % pixel standard deviations像素标准偏差，注意与下面的方差区别，平方的关系
u_diff = zeros(height,width,C,D_Gauss);         % difference of each pixel from mean每个像素与均值的差值
p = alpha/(1/C);                        % initial p variable (used to update mean and sd)均值，方差的更新系数
rank = zeros(1,C);                      % rank of components (w/sd)用于排序
X = zeros(height,width,D_Gauss);

% --------------------- initialize component means and weights -----------

pixel_depth = 8;                        % 8-bit resolution
pixel_range = 2^pixel_depth -1;         % pixel range (# of possible values)
%初始化每个点的每个高斯模型的均值，权重，方差
for h=1:height
    for w=1:width
        for c=1:C %每个像素点设置了三个高斯模型
            for dg=1:D_Gauss
                mean(h,w,c,dg) = rand*pixel_range;     % means random (0-255)
                weight(h,w,c,dg) = 1/C;                     % weights uniformly dist
            end
            for dg=1:D_Gauss
                sd(h,w,c,dg) = sd_init;                % initialize to sd_init
            end
        end
    end
end

%--------------------- process frames -----------------------------------

for n = 1:numFrames% 计算视频的总帧数
    
    % 读取帧,并转化为灰度图
    fr = imread(fullfile(dir_path,file_array{n}));
    dim=size(size(fr));
    if dim(2)==3
        fr_bw=rgb2gray(fr);
    else
        fr_bw=fr;
    end
    if n==1
        fr_bw_pre=fr_bw;
        continue;
    end
    % 构造高斯模型的样本X,增加函数,X = create_X(D_Gauss)
    X(:,:,1)=double(fr_bw);
    for h=1:height
        for w=1:width
            X(h,w,2)=filter2(fr_bw_pre,fr_bw,[h,w],3);
        end
    end
    
    % calculate difference of pixel values from mean
    %计算当前帧每个像素值与每个高斯模型均值的差，即进行相应的匹配，
    for c=1:C
        for dg=1:D_Gauss
            u_diff(:,:,c,dg) = abs(X(:,:,2) - double(mean(:,:,c,dg)));
        end
    end
    
    % update gaussian components for each pixel
    % 将当前帧的每个像素值进行匹配
    for h=1:height
        for w=1:width
            match = [0,0];
            for c=1:C
                if (abs(u_diff(h,w,c,1)) <= D*sd(h,w,c,1))       % pixel matches component
                    % pixel matches component
                    % 当前像素与某一高斯模型匹配，匹配准
                    % 则是与高斯模型差值小与2.5倍的标准差
                    match(1) = 1;     % variable to signal component match
                    % 匹配的时候，匹配标志设置为1
                    % variable to signal component match
                    
                    % update weights, mean, sd, p 更新每个像素的高斯模型，如果当前像素与某个背景高斯模型相匹配，则对背景进行更新
                    weight(h,w,c,1) = (1-alpha)*weight(h,w,c,1) + alpha; % 权值的更新
                    p = alpha/weight(h,w,c,1);  % 更新率的更新
                    mean(h,w,c,1) = (1-p)*mean(h,w,c,1) + p*double(fr_bw(h,w));% 当前像素点的均值的更新
                    sd(h,w,c,1) =   sqrt((1-p)*(sd(h,w,c,1)^2) + p*((double(fr_bw(h,w)) - mean(h,w,c,1)))^2);% 标准差的更新（也可以用方差）
                else                                    % pixel doesn't match component当像素与任何一个高斯模型都不匹配的时候
                    weight(h,w,c,1) = (1-alpha)*weight(h,w,c,1);      % weight slighly decreases% 对权重进行更新，不匹配的高斯模型权重减小
                end
                
                if (abs(u_diff(h,w,c,2)) >= D*sd(h,w,c,2))       % pixel matches component
                    % pixel matches component
                    % 当前像素与某一高斯模型匹配，匹配准
                    % 则是与高斯模型差值小与2.5倍的标准差
                    match(2) = 1;                          % variable to signal component match
                    % 匹配的时候，匹配标志设置为1
                    % variable to signal component match
                    
                    % update weights, mean, sd, p 更新每个像素的高斯模型，如果当前像素与某个背景高斯模型相匹配，则对背景进行更新
                    weight(h,w,c,2) = (1-alpha)*weight(h,w,c,2) + alpha; % 权值的更新
                    p = alpha/weight(h,w,c,2);                 % 更新率的更新
                    mean(h,w,c,2) = (1-p)*mean(h,w,c,2) + p*double(fr_bw(h,w));% 当前像素点的均值的更新
                    sd(h,w,c,2) =   sqrt((1-p)*(sd(h,w,c,2)^2) + p*((double(fr_bw(h,w)) - mean(h,w,c,2)))^2);% 标准差的更新（也可以用方差）
                else                                    % pixel doesn't match component当像素与任何一个高斯模型都不匹配的时候
                    weight(h,w,c,2) = (1-alpha)*weight(h,w,c,2);      % weight slighly decreases% 对权重进行更新，不匹配的高斯模型权重减小
                end
            end
            
            weight(h,w,:,1) = weight(h,w,:,1)./sum(weight(h,w,:,1));% 各个像素所有高斯背景模型的值之和，一个统计值
            weight(h,w,:,2) = weight(h,w,:,2)./sum(weight(h,w,:,2));
            
            % 跟新背景
            bg_bw(h,w)=0;
            for k=1:C
                bg_bw(h,w) = bg_bw(h,w)+ (mean(h,w,c,1)*weight(h,w,c,1)+mean(h,w,c,2)*weight(h,w,c,2))/2;%更新背景
                %%5............
                %if(bg_bw(i,j)>thresh)
                % k=k-1;
                %  M=k;
                %end%????..........
            end
            
            % if no components match, create new component
            % 当所有的高斯模型都不匹配的时候，用当前的图像参数建立一个新的高斯模型，取代权重最小的模型
            if (match(1) == 0)
                [min_w, min_w_index] = min(weight(h,w,:,1));  %找出每一个像素对应的权重最小的高斯模型
                %找出w中的所有维数(也就是所有高斯模型)对应的第i行，第j列的最小值（也就是权重），
                %返回值min_w为数值（权重）大小，min_w_index为对应的维数（高斯模型）
                mean(h,w,min_w_index,1) = double(fr_bw(h,w));%fr_bw为背景灰度图像，由于没有匹配，所以保留原值不变
                %我认为是当前像素值作为新模型的均值
                sd(h,w,min_w_index,1) = sd_init;%没有匹配，保留原值不变
            end
            if (match(2) == 0)
                [min_w, min_w_index] = min(weight(h,w,:,2));  %找出每一个像素对应的权重最小的高斯模型
                %找出w中的所有维数(也就是所有高斯模型)对应的第i行，第j列的最小值（也就是权重），
                %返回值min_w为数值（权重）大小，min_w_index为对应的维数（高斯模型）
                mean(h,w,min_w_index,2) = double(fr_bw(h,w));%fr_bw为背景灰度图像，由于没有匹配，所以保留原值不变
                %我认为是当前像素值作为新模型的均值
                sd(h,w,min_w_index,2) = sd_init;%没有匹配，保留原值不变
            end
            
            
            rank = (weight(h,w,:,1)./sd(h,w,:,1)+weight(h,w,:,2)./sd(h,w,:,2))/2;             % calculate component rank
            rank_ind = [1:1:C];  %计算权重与标准差的比值，用于对背景模型进行排序
            
            % sort rank values
            for k=2:C
                for m=1:(k-1)
                    
                    if (rank(:,:,k) > rank(:,:,m))
                        % swap max values
                        rank_temp = rank(:,:,m);
                        rank(:,:,m) = rank(:,:,k);
                        rank(:,:,k) = rank_temp;
                        
                        % swap max index values
                        rank_ind_temp = rank_ind(m);
                        rank_ind(m) = rank_ind(k);
                        rank_ind(k) = rank_ind_temp;
                        
                    end
                end
            end
            
            % 计算前景calculate foreground
            match = 0;
            k=1;
            
            % 跟新前景
            fg(h,w) = 0; %前景
            while ((match(1) == 0)&&(k<=M))% 没有匹配，而且当前高斯数号小于背景数
                
                if (weight(h,w,rank_ind(k)) >= thresh)
                    if (abs(u_diff(h,w,rank_ind(k))) <= D*sd(h,w,rank_ind(k)))
                        fg(h,w) = 255; %判断是否是前景，如果符合匹配准则就认为是背景
                        match = 1;
                    else                   %否则认为是前景，前景值为255，即白色
                        fg(h,w) = 0;
                    end
                end
                k = k+1; %计算下一个高斯模型
                if(k==5)
                    k=k-1;
                    break
                end
            end
        end 
    end
    figure(1),subplot(3,1,1),imshow(fr)%显示输入图像三行一列定位第一个
    subplot(3,1,2),imshow(uint8(bg_bw))%显示背景图像三行一列定位第2个
    subplot(3,1,3),imshow(uint8(fg)) %显示前景图像
    fr_bw_pre=fr_bw;
end