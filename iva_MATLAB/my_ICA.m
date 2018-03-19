%% 产生混合矩阵
close all;clear all;
[s1,fs1]=audioread('hello.wav');
[s2,fs2]=audioread('why.wav');
figure
subplot(211)
plot(s1);
title('源信号1')
subplot(212)
plot(s2);
title('源信号2')
M=[0.5,1.3;0.9,0.4];
X=[s1,s2]*M;
x1=X(:,1);
x2=X(:,2);
%% 中心化
figure
subplot(211)
plot(x1);
title('语音混合信号1')
subplot(212)
plot(x2);
title('语音混合信号2')
x1=x1-mean(x1);
x2=x2-mean(x2);
X=[x1,x2];
%% 白化
[E,D]=eig(cov(X));
V = diag((diag(D).^(-1/2)))*E';
Z = (V*X')';
z1 = Z(:,1);
z2 = Z(:,2);
%% 计算各个方向的峭度
temp_z=zeros(1000,1);
theta=linspace(0,pi,1000);
for i=1:1000
    w = [cos(theta(i));sin(theta(i))];
    temp_z(i) = abs(my_kurt(Z*w));
end
figure
plot(theta,temp_z);
title('峭度随角度变化的图像');
indtop=find(diff(sign(diff(temp_z)))==-2)+1;
hold on
theta1=theta(indtop(1));
theta2=theta(indtop(2));
plot(theta1,temp_z(indtop(1)),'marker','.','markersize',15,'color','k');
plot(theta2,temp_z(indtop(2)),'marker','.','markersize',15,'color','r');
w=[cos(theta1),cos(theta2);sin(theta1),sin(theta2)];
EN=Z*inv(w');
figure
subplot(211)
plot(EN(:,1));
title('语音分离信号1')
subplot(212)
plot(EN(:,2));
title('语音分离信号2')
%w=[cos,cos;sin,sin];
%A=w*inv(V');
%% 用不动点法快速计算独立成分





