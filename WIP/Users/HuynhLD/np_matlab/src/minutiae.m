function R_minutiae = minutiae(I,R,core)
% Ham tim cac diem dac trung cua anh van tay I
% v: buoc nhay cua is, js
% sigma: 1/2 be rong van
% T: anh sau khi da giac hoa cua I
% T1: ma tran chua cac gia tri cua phuong tiep tuyen, tuong ung voi cac
% duong van sau khi da giac hoa.
% T2: ma tran chua cac gia tri cua phuong tiep tuyen tai cac diem dac trung
v=4;
sigma=5;
I=255-double(I);
[m, n]=size(I);
T(1:m,1:n)=0;
for is=26:v:m-25
    for js=26:v:n-25
        if ((is>core(1)-20)&&(is<core(1)+20)&&(js>core(2)-15)&&(js<core(2)+15))||(R(is,js)==255)
        else
            [ic, jc]=ridgenearest(I,is,js,sigma);
            label=true;
            if R(ic,jc)==255
                label=false;
            else
                for i=ic-4:ic+4
                    for j=jc-4:jc+4
                        if T(i,j)==255
                            label=false;
                        end;
                    end;
                end;
            end;
            if label
                [T, R]=ridgefollowing(I,T,R,ic,jc,core,sigma,2.4);
                [T, R]=ridgefollowing(I,T,R,ic,jc,core,sigma,-2.4);
            end;
        end;
    end;
end;
R_minutiae=R;
%--------------------------------------------------------------------------
function [T,R] = ridgefollowing(I,T,R,ic,jc,core,sigma,muy)
% Ham do theo duong van tay
% muy: buoc do
[m, n]=size(I);
io=ic;
jo=jc;
if muy>0
    object=0;
    muyO=muy;
else
    object=pi;
    muyO=-muy;
    muy=0;
end;
object1=0;
phiOO=tangentDir(I,io,jo,9);
phiO=phiOO+object;
esc=true;
while (esc)
    it=round(io+muy*sin(phiO));
    jt=round(jo+muy*cos(phiO));
    muy=muyO;
    if ((it>26)&&(it<m-25)&&(jt>26)&&(jt<n-25))&&(R(it,jt)~=255)
        if (it>core(1)-20)&&(it<core(1)+20)&&(jt>core(2)-15)&&(jt<core(2)+15)
            R(it,jt)=-3; % Diem ngat do cham vao vung core
            esc=false;
        else
            phit=tangentDir(I,it,jt,9);
            [In, in, jn]=localmax(I,it,jt,phit,sigma);
            if (io~=ic)&&(jo~=jc)&&(in==io)&&(jn==jo)
                esc=false;
            else
                phinn=tangentDir(I,in,jn,9);
                if abs(phinn-phiOO)>5*pi/6 % Duong van doi huong tu 90 do
                    if phiO>pi/2 % sang -90 do hoac nguoc lai
                        object1=0;
                    else
                        object1=pi;
                    end;
                    object=0;
                end;
                phin=phinn+object+object1;
                [T, R, esc]=stopcriteria(I,T,R,In,in,jn,phin);
                io=in;
                jo=jn;
                phiOO=phinn;
                phiO=phin;
            end;
        end;
    else
        esc=false;
    end;
end;
%--------------------------------------------------------------------------
function [ic,jc] = ridgenearest(I,is,js,sigma)
% Ham xac dinh diem cuc dai cuc bo (ic,jc) gan (is,js) nhat
phiC=tangentDir(I,is,js,9);
i1=round(is-(sigma-3)*cos(phiC+pi/2));
j1=round(js-(sigma-3)*sin(phiC+pi/2));
[~, ic1, jc1]=localmax(I,i1,j1,phiC,sigma);
i2=round(is+(sigma-3)*cos(phiC+pi/2));
j2=round(js+(sigma-3)*sin(phiC+pi/2));
[~, ic2, jc2]=localmax(I,i2,j2,phiC,sigma);
d1=(ic1-is)^2+(jc1-js)^2;
d2=(ic2-is)^2+(jc2-js)^2;
if d1<d2
    ic=ic1;
    jc=jc1;
else
    ic=ic2;
    jc=jc2;
end;
%--------------------------------------------------------------------------
function [T,R,esc] = stopcriteria(~,T,R,In,in,jn,phin)
% Ham dieu kien dung cua thuat toan
% Ham nay se xet xem diem (in,jn) voi phuong tiep tuyen la phin co thoa
% dieu kien dung cua thuat toan khong.
% phiO: phuong tiep tuyen cua diem (io,jo) lien truoc diem (in,jn).
esc=true;
phin=phin*180/pi;
I_nguong=20;
if ((phin>-90)&&(phin<=-67.5))||((phin>247.5)&&(phin<=270))
    case_phin=1;
elseif (phin>-67.5)&&(phin<=-22.5)
    case_phin=2;
elseif (phin>-22.5)&&(phin<=22.5)
    case_phin=3;
elseif (phin>22.5)&&(phin<=67.5)
    case_phin=4;
elseif (phin>67.5)&&(phin<=112.5)
    case_phin=5;
elseif (phin>112.5)&&(phin<=157.5)
    case_phin=6;
elseif (phin>157.5)&&(phin<=202.5)
    case_phin=7;
else
    case_phin=8;
end;
%--------------------------------------------------------------------------
switch case_phin
    case 1
        if T(in-1,jn)==255
            label=true;
            for i=in-6:in+4
                for j=jn-4:jn+4
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2; % Mot diem minutae loi da duoc phat hien
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2; % Mot diem re nhanh
            end;
            esc=false;
        else
            for i=in-1:in+1
                for j=jn-1:jn+1
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-6:in+4
                    for j=jn-4:jn+4
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1; % Mot diem ket thuc
                end;
                esc=false;
            end;
        end;
    case 2
        if T(in-1,jn+1)==255
            label=true;
            for i=in-6:in+4
                for j=jn-4:jn+6
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for a=-1:1
                for b=-1:1
                    i=in-a+b;
                    j=jn-a-b;
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-6:in+4
                    for j=jn-4:jn+6
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 3
        if T(in,jn+1)==255
            label=true;
            for i=in-4:in+4
                for j=jn-4:jn+6
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for i=in-1:in+1
                for j=jn-1:jn+1
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-4:in+4
                    for j=jn-4:jn+6
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 4
        if T(in+1,jn+1)==255
            label=true;
            for i=in-4:in+6
                for j=jn-4:jn+6
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for a=-1:1
                for b=-1:1
                    i=in-a+b;
                    j=jn-a-b;
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-4:in+6
                    for j=jn-4:jn+6
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 5
        if T(in+1,jn)==255
            label=true;
            for i=in-4:in+6
                for j=jn-4:jn+4
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for i=in-1:in+1
                for j=jn-1:jn+1
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-4:in+6
                    for j=jn-4:jn+4
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 6
        if T(in+1,jn-1)==255
            label=true;
            for i=in-4:in+6
                for j=jn-6:jn+4
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for a=-1:1
                for b=-1:1
                    i=in-a+b;
                    j=jn-a-b;
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-4:in+6
                    for j=jn-6:jn+4
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 7
        if T(in,jn-1)==255
            label=true;
            for i=in-4:in+4
                for j=jn-6:jn+4
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for i=in-1:in+1
                for j=jn-1:jn+1
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-4:in+4
                    for j=jn-6:jn+4
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
    case 8
        if T(in-1,jn+1)==255
            label=true;
            for i=in-6:in+4
                for j=jn-6:jn+4
                    if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                        R(i,j)=-2;
                        label=false;
                    end;
                end;
            end;
            if (label)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                R(in,jn)=2;
            end;
            esc=false;
        else
            for a=-1:1
                for b=-1:1
                    i=in-a+b;
                    j=jn-a-b;
                    T(i,j)=255;
                end;
            end;
            if (In<I_nguong)&&(R(in-20,jn)~=255)&&(R(in+20,jn)~=255)&&(R(in,jn-20)~=255)&&(R(in,jn+20)~=255)
                label=true;
                for i=in-6:in+4
                    for j=jn-6:jn+4
                        if (R(i,j)==1)||(R(i,j)==2)||(R(i,j)==-2)||(R(i,j)==-3)
                            R(i,j)=-2;
                            label=false;
                        end;
                    end;
                end;
                if label
                    R(in,jn)=1;
                end;
                esc=false;
            end;
        end;
end;
%--------------------------------------------------------------------------
function phit = tangentDir(I,it,jt,anpha)
% Ham xac dinh phuong tiep tuyen phit tai diem (it,jt)
I=255-double(I);
A=0;
B=0;
C=0;
for h=it-(anpha-1)/2:it+(anpha-1)/2
    for k=jt-(anpha-1)/2:jt+(anpha-1)/2
        ahk=(-I(h-1,k-1)+I(h-1,k+1)+I(h+1,k+1)-I(h+1,k-1))/4;
        bhk=(-I(h-1,k-1)-I(h-1,k+1)+I(h+1,k+1)+I(h+1,k-1))/4;
        A=A+ahk^2;
        B=B+bhk^2;
        C=C+ahk*bhk;
    end;
end;
if C>0
    t1=1;
    t2=(B-A)/(2*C)-sqrt(((B-A)/(2*C))^2+1);
elseif C<0
    t1=1;
    t2=(B-A)/(2*C)+sqrt(((B-A)/(2*C))^2+1);
elseif A<=B
    t1=1;
    t2=0;
else
    t1=0;
    t2=1;
end;
if t1==0
    phit=pi/2;
else
    phit=atan(t2/t1);
end;
%--------------------------------------------------------------------------
function [In, in, jn] = localmax(I,it,jt,phit,sigma)
% Ham xac dinh diem cua dai cuc bo (in,jn) theo phuong vuong goc voi phit
In=0;
d=[1/23 2/23 5/23 7/23 5/23 2/23 1/23];
in=it;
jn=jt;
LM=[];
io=[];
jo=[];
for x=-sigma:sigma
    i=round(it+x*sin(phit+pi/2));
    j=round(jt+x*cos(phit+pi/2));
    % Io la gia tri trung bình cua I(i,j) voi hai diem lan can cua no
    Io=(I(round(i-sin(phit)),round(j-cos(phit)))+I(i,j)+...
        I(round(i+sin(phit)),round(j+cos(phit))))/3;
    LM=[LM Io]; % LM la vetor chua cac gia tri trung binh tren doan sigma
    io=[io i];
    jo=[jo j];
end;
LMn(1:2*sigma+1)=0;
for x=4:2*sigma-2
    LMn(x)=LM(x-3)*d(1)+LM(x-2)*d(2)+LM(x-1)*d(3)+LM(x)*d(4)+...
        LM(x+1)*d(5)+LM(x+2)*d(6)+LM(x+3)*d(7);% Loc Gaussian
    if In<LMn(x);
        in=io(x);
        jn=jo(x);
        In=LMn(x);
    end;
end;