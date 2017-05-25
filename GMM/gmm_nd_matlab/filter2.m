function[val] = filter2(frame_pre,frame,p,kSize)
shape=size(frame);
val=0;
n=0;
for i=1:kSize
    x=p(1)+(i-int32(kSize)+1);
    if x>0 && x<=shape(1)
        for j=1:kSize
            y=p(2)+(j-int32(kSize)+1);
            if y>0 && y<=shape(2)
                val=val+abs(double(frame(x,y))-double(frame_pre(x,y)));
                n=n+1;
            end
        end
    end
end
val=val/n;
