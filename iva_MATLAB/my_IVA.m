cle%% start
close all;clear all;

WINDOW_LEN=4096;
%WIN=rectwin(WINDOW_LEN);
WIN=hanning(WINDOW_LEN);
FFT_LEN=WINDOW_LEN;
%NAP=fix(WINDOW_LEN/2);
NAP=3896;
%WIN=WIN(2:WINDOW_LEN+1);
%% part1
% 
% [x1,fs1]=audioread('mix21.m4a');
% [x2,fs2]=audioread('mix22.m4a');
% x2=x2(26600:end);
% x2=x2(1:length(x1));
% x1=x1(1:fix(length(x1)/WINDOW_LEN)*WINDOW_LEN)';
% x2=x2(1:fix(length(x1)/WINDOW_LEN)*WINDOW_LEN)';
%% part2
% 
% MY_L=20000;
% source1=sin((1:MY_L)/MY_L*2*pi).*rand(1,MY_L);
% source2=heaviside(cos((1:MY_L)/(MY_L)*2*pi)).*(rand(1,MY_L)-0.5);
% figure
% subplot(211)
% plot(source1);
% title('源1');
% subplot(212)
% plot(source2);
% title('源2');
% 
% source1_fft=fft(source1);
% source2_fft=fft(source2);
% 
% M=[0.4,0.2;0.3,0.4];
% 
% XX_fft=M*[source1_fft;source2_fft];
% x1=ifft(XX_fft(1,:));
% x2=ifft(XX_fft(2,:));
% fs1=MY_L/10;
%% part3
% 
[source1,fs1]=audioread('nihao.m4a');
[source2,fs2]=audioread('weishenm.m4a');
source1=source1(1:250000);
source2=source2(1:250000);

source1=source1(1:fix(length(source1)/WINDOW_LEN)*WINDOW_LEN);
source2=source2(1:fix(length(source1)/WINDOW_LEN)*WINDOW_LEN);
source1=source1';
source2=source2';
% source1=sin((1:MY_L)/MY_L*2*pi).*rand(1,MY_L);
% source2=heaviside(cos((1:MY_L)/(MY_L)*2*pi)).*(rand(1,MY_L)-0.5);
h3=figure;
subplot(211)
plot((1:length(source1))/fs1,source1);
title('源信号1');
xlabel('时间(s)');
subplot(212)
plot((1:length(source1))/fs1,source2);
title('源信号2');
xlabel('时间(s)');
c = 340; % Sound velocity(m/s)
fs = fs1; % Sample frequency(samples/s)
r1 = [ 2 2 2 ] ; % Receiver position[ x y z ](m)
s1 = [ 2 2.5 1 ] ; % Source position[ x y z ](m)
r2 = [ 2 2.1 2 ] ; % Receiver position[ x y z ](m)
s2 = [ 3 2.5 3 ] ; % Source position[ x y z ](m)
L = [ 5 4 6 ] ; % Room dimensions[ x y z ](m)
beta = 0.2 ; % Reverberation time( s )
n = 500; % Number of samples
h11 = rir_generator(c,fs,r1,s1,L,beta,n ) ;
h12 = rir_generator(c,fs,r2,s1,L,beta,n ) ;
h21 = rir_generator(c,fs,r1,s2,L,beta,n ) ;
h22 = rir_generator(c,fs,r2,s2,L,beta,n ) ;

x1=conv(source1,h11)+conv(source2,h21);
x2=conv(source1,h12)+conv(source2,h22);

x1=x1(1:length(source1));
x2=x2(1:length(source2));
%% part4
h2=figure;
subplot(211)
plot((1:length(x1))/fs1,x1);
title('合成信号1');
xlabel('时间(s)');
subplot(212)
plot((1:length(x1))/fs1,x2);
title('合成信号2');
xlabel('时间(s)');

tic;
[S1,F1,T1]=sg(x1,FFT_LEN,fs1,WIN,NAP);
[S2,F2,T2]=sg(x2,FFT_LEN,fs2,WIN,NAP);
Out1=zeros(size(S1));
Out2=zeros(size(S2));
FL=length(F1);
TL=length(T1);
W=zeros(2,2,FL);
for i=1:FL
W(:,:,i)= diag([1.0,1.0]);
end
pause(0.1)
ibxu=ones(1,FL);
for times=1:TL
        for temp_f_p=1:FL        
           temp= W(:,:,temp_f_p)*[S1(temp_f_p,times);S2(temp_f_p,times)];
           Out1(temp_f_p,times)=temp(1);
           Out2(temp_f_p,times)=temp(2); 
        end
    beta=0.2;   
    yita=2.0;
    for temp_f_p=1:FL
        phi=[Out1(temp_f_p,times)/sqrt(sum(abs(Out1(:,times)).^2));Out2(temp_f_p,times)/sqrt(sum(abs(Out2(:,times)).^2))];
        yx=[conj(Out1(temp_f_p,times)), conj(Out2(temp_f_p,times))];
        R=phi*yx;
        I=diag(diag(R));
        delta_G=(I-R)*W(:,:,temp_f_p);
        ibxu(temp_f_p)=beta*ibxu(temp_f_p)+(1-beta)*(abs(S1(temp_f_p,times))^2+abs(S2(temp_f_p,times))^2)/2;
        W(:,:,temp_f_p)=W(:,:,temp_f_p)+yita*((ibxu(temp_f_p))^(-1/2))*delta_G;
    end
    W(:,:,temp_f_p)
   disp(num2str(times/TL))
end

% for temp_t_p=1:TL
%    for temp_f_p=1:FL
%      temp= W(:,:,temp_f_p)*[S1(temp_f_p,temp_t_p);S2(temp_f_p,temp_t_p)];
%        Out1(temp_f_p,temp_t_p)=temp(1);
%        Out2(temp_f_p,temp_t_p)=temp(2); 
%    end
% end
% 
% figure
% subplot(211);
% surf(T1,F1,10*log10(abs(Out1)),'edgecolor','none'); axis tight;
% title('分离信号的短时傅里叶变换1')
% view(0,90);
% subplot(212);
% surf(T2,F2,10*log10(abs(Out2)),'edgecolor','none'); axis tight;
% view(0,90);
% title('分离信号的短时傅里叶变换2')

% OutSound1=[];
% for i=1:TL
% OutSound1=[OutSound1;ifft(Out1(:,i),WINDOW_LEN)./WIN];
% end
% %sound(real(OutSound1),fs1/2)
% 
% OutSound2=[];
% for i=1:TL
% OutSound2=[OutSound2;ifft(Out2(:,i),WINDOW_LEN)./WIN];
% end

OutSound1=invspecgram(Out1,FFT_LEN,fs1,WIN,NAP);
OutSound2=invspecgram(Out2,FFT_LEN,fs2,WIN,NAP);
%sound(real(OutSound2),fs2)
fss=length(OutSound1)/(length(x1)/fs1);
h1=figure;
subplot(211)
%OutSound1=OutSound1/max(OutSound1);
plot((1:length(OutSound1))/fss,real(OutSound1))
title('分离信号1')
xlabel('时间(s)');
subplot(212)
plot((1:length(OutSound1))/fss,real(OutSound2))
title('分离信号2')
xlabel('时间(s)')

toc
saveas(h1,'h1.jpg')
saveas(h2,'h2.jpg')
saveas(h3,'h3.jpg')
audiowrite('source1.wav',source1,fs1);
audiowrite('source2.wav',source2,fs1);
audiowrite('mix1.wav',x1,fs1);
audiowrite('mix2.wav',x2,fs1);
audiowrite('sperate1.wav',OutSound1,fs1);
audiowrite('sperate2.wav',OutSound2,fs1);


