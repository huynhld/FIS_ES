function neural_tem = neural_template(tem)
% Xay dung mau cho mang neural
N1=0;
N2=0;
N3=0;
N4=0;
mi(1:3,1:4)=0;
m=length(tem(1,:));
for i=3:m
    if (tem(1,i)>=0)&&(tem(2,i)>=0)
        N1=N1+1;
        mi(1,1)=mi(1,1)+tem(1,i);
        mi(2,1)=mi(2,1)+tem(2,i);
    elseif (tem(1,i)>=0)&&(tem(2,i)<=0)
        N2=N2+1;
        mi(1,2)=mi(1,2)+tem(1,i);
        mi(2,2)=mi(2,2)+tem(2,i);
    elseif (tem(1,i)<=0)&&(tem(2,i)<=0)
        N3=N3+1;
        mi(1,3)=mi(1,3)+tem(1,i);
        mi(2,3)=mi(2,3)+tem(2,i);
    else
        N4=N4+1;
        mi(1,4)=mi(1,4)+tem(1,i);
        mi(2,4)=mi(2,4)+tem(2,i);
    end;
end;
if N1~=0
    mi(:,1)=abs(mi(:,1)/N1);
    mi(3,1)=N1;
end;
if N2~=0
    mi(:,2)=abs(mi(:,2)/N2);
    mi(3,2)=N2;
end;
if N3~=0
    mi(:,3)=abs(mi(:,1)/N3);
    mi(3,3)=N3;
end;
if N4~=0
    mi(:,4)=abs(mi(:,4)/N4);
    mi(3,4)=N4;
end;
neural_tem=[tem(1,1);tem(2,1);tem(1,2);tem(2,2);mi(:,1);mi(:,2);mi(:,3);mi(:,4)];