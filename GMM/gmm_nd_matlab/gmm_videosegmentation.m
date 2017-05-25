N=200;width=238;height=158;
dir_path = '/home/yangzheng/testData/ucsd/vidf1_33_010.y';
dir_info=dir(fullfile(dir_path,'*.png'));
file_array={dir_info.name};
vedio_data.frame(1,1).vector=[0 0;0 0];
vedio_data.frame(1,2).vector=[0 0;0 0];
% vedio_data=zeros([width,height,1,2]);]
% gmm_mod.mod(1,1).Mu=0;
% gmm_mod.mod(1,1).Sigma=0;
% gmm_mod.mod(1,1).Pi=0;
% gmm_mod.mod(1,2).Mu=0;
% gmm_mod.mod(1,2).Sigma=0;
% gmm_mod.mod(1,2).Pi=0;
K=1;

frame=imread(fullfile(dir_path,file_array{1}));
shape=size(frame);
for w=1:shape(2)
	for h=1:shape(1)
        gmm_mod.mod(h,w).Mu=[double(frame(h,w)),double(frame(h,w))];
        gmm_mod.mod(h,w).Sigma=rand(2,2)+6;
        gmm_mod.mod(h,w).Pi=1/K;
    end
end

for n=1:N
    frame=imread(fullfile(dir_path,file_array{n}));
    shape=size(frame);
    output = zeros(shape);
    dim=size(shape);
    if dim(2)==3
        g_frame=rgb2gray(frame);
    else
        g_frame=frame;
    end

    % 遍历每个像素
    for w=1:1:shape(2)
        for h=1:1:shape(1)
            x=double(g_frame(h,w));
            y=filter(g_frame,[h,w],3);
            vedio_data.frame(h,w).vector(n,:)=[x,y];
            %             vedio_data(w,h,n,:)=[x,y];
%             fprintf('%d,%d\n',w,h)
            if n>=1
                %                 vector=vedio_data.frame(h,w).vector(1:n,:);
                if abs(x-gmm_mod.mod(h,w).Mu(1)) < 2*(gmm_mod.mod(h,w).Sigma(1,1)^2)
%                     if abs(y-gmm_mod.mod(h,w).Mu(2))<2*gmm_mod.mod(h,w).Sigma(2,2)
%                         [Mu,Sigma,Pi]=em_gaussmod_2d_test([x,x],1,gmm_mod.mod(h,w).Mu,gmm_mod.mod(h,w).Sigma,gmm_mod.mod(h,w).Pi,false);
%                         gmm_mod.mod(h,w).Mu=Mu;
%                         gmm_mod.mod(h,w).Sigma=Sigma;
%                         gmm_mod.mod(h,w).Pi=Pi;
%                         output(h,w)=255;
%                     end
                    [Mu,Sigma,Pi]=em_gaussmod_2d_test([x,x],1,gmm_mod.mod(h,w).Mu,gmm_mod.mod(h,w).Sigma,gmm_mod.mod(h,w).Pi,false);
                    gmm_mod.mod(h,w).Mu=Mu;
                    gmm_mod.mod(h,w).Sigma=Sigma;
                    gmm_mod.mod(h,w).Pi=Pi;
                    output(h,w)=255;
                end   
            end
        end
    end
    
    figure(2);subplot(2,1,1);
    imshow(frame);
    subplot(2,1,2);
    imshow(output);
end,