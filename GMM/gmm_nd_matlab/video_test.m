fileName = './test.avi'; 
obj = VideoReader(fileName);
numFrames = obj.NumberOfFrames;% 读取视频的帧数
 for i = 1 : numFrames
     frame = read(obj,i);% 读取每一帧
     imshow(frame);%显示每一帧
%      imwrite(frame,strcat(num2str(i),'.jpg'),'jpg');% 保存每一帧
end