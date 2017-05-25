function[val] = filter(frame,p,kSize)
shape=size(frame);
val=0;
n=0;
for i=1:kSize
    x=p(1)+(i-int32(kSize)+1);
    if x>0 && x<=shape(1)
        for j=1:kSize
            y=p(2)+(j-int32(kSize)+1);
            if y>0 && y<=shape(2)
                val=val+double(frame(x,y));
                n=n+1;
            end
        end
    end
end
val=abs(val/n-double(frame(p(1),p(2))));
