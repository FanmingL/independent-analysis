%initialization
%it spends aboud 0.36 seconds to initialize
%//1 matlab clear
clear;
close all;
%//2 initialize some coeffs according to the paper 

Ns                  = 4;
fft_length          = 256;                      %unit is sample
window_length       = fft_length;               %unit is sample
shift_size          = 64;                      %unit is sample
reverbration_time   = 250;                      %unit is ms
windows             = hanning(fft_length);      %hanning window
expect_sample_rate  = 8000;                     %8kHz sample rate
start_simulate_point= 200000;                   %point start used to simulate
simulation_length   = 30000;                    %length used to simulate, unit is ms


%//3 read audio from files and resample to expect sample rate
[source1,fs1] = audioread('female_record.wav');
[source2,fs2] = audioread('male_record.wav');
source1 = resample(source1,expect_sample_rate,fs1);
source2 = resample(source2,expect_sample_rate,fs2);
source1 = source1(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
source2 = source2(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
audio_cut1_fft = zeros(ceil(length(source1)/shift_size+10),fft_length);
audio_cut2_fft = zeros(ceil(length(source1)/shift_size+10),fft_length);


tic;
%//6 iterator
for n = 1:(floor((length(source1)-window_length)/shift_size))       % at Nth times
    audio_cut1 = (source1((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows).';
    audio_cut2 = (source2((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows).';
    audio_cut1_fft(n,:) = fft(audio_cut1);
    audio_cut2_fft(n,:) = fft(audio_cut2);

end
audio_cut1_fft(n+1:end,:) = [];
audio_cut2_fft(n+1:end,:) = [];
slide_number = n;
for k=1:fft_length
    tmp_fft = [audio_cut1_fft(:,k),audio_cut2_fft(:,k)];
    tmp = cov(tmp_fft);
    [E,D]=eig(tmp);
    V = diag((diag(D).^(-1/2)))*E';
    Z = (V*tmp_fft.').';
    audio_cut1_fft(:,k) = Z(:,1);
    audio_cut2_fft(:,k) = Z(:,2);
end
omega_1 = zeros(slide_number,Ns,fft_length);
omega_2 = zeros(slide_number,Ns,fft_length);
Phi_1   = abs(randn(Ns,fft_length));
Phi_1   = Phi_1./sum(Phi_1,1);
Phi_2   = abs(randn(Ns,fft_length));
Phi_2   = Phi_2./sum(Phi_2,1);
Sigma_1 = abs(randn(Ns,fft_length));
Sigma_1(Sigma_1>0)=Sigma_1(Sigma_1>0)+0.01;
Sigma_2 = abs(randn(Ns,fft_length));
Sigma_2(Sigma_2>0)=Sigma_2(Sigma_2>0)+0.01;
Sigma_2(Sigma_2<0)=Sigma_2(Sigma_2<0)-0.04;
omega_P1= zeros(slide_number,Ns,fft_length);
omega_P2= zeros(slide_number,Ns,fft_length);
bias = 0.002;
for time = 1:30
    Sigma_1(Sigma_1>0&Sigma_1<bias)=Sigma_1(Sigma_1>0&Sigma_1<bias)+bias;
    Sigma_2(Sigma_2>0&Sigma_2<bias)=Sigma_2(Sigma_2>0&Sigma_2<bias)+bias;
    for i = 1:slide_number
        for k=1:fft_length
            for n=1:Ns
%                 omega_1(i,n,k) = *exp(-1/2/Sigma_1(n,k)^2);
%                 omega_2(i,n,k) = Phi_2(n,k)/(sqrt(2*pi)*abs(Sigma_2(n,k)))*exp(-1/2/Sigma_2(n,k)^2);
                omega_1(i,n,k) = log(Phi_1(n,k)/(sqrt(2*pi)*(Sigma_1(n,k))))+...
                                    (-1/2/Sigma_1(n,k)^2*abs(audio_cut1_fft(i,k))^2);
                omega_2(i,n,k) = log(Phi_2(n,k)/(sqrt(2*pi)*(Sigma_2(n,k))))+...
                                    (-1/2/Sigma_2(n,k)^2*abs(audio_cut2_fft(i,k))^2);
                
            end
            omega_1(i,:,k)      = omega_1(i,:,k)-max(omega_1(i,:,k));
            omega_2(i,:,k)      = omega_2(i,:,k)-max(omega_2(i,:,k));
            
            omega_1(i,:,k)          =  exp(omega_1(i,:,k))/sum(exp(omega_1(i,:,k)));
            omega_2(i,:,k)          =  exp(omega_2(i,:,k))/sum(exp(omega_2(i,:,k)));

        end
    end
    for i = 1:slide_number
        for k=1:fft_length
            for n=1:Ns
                omega_P1(i,n,k)= omega_1(i,n,k)*abs(audio_cut1_fft(i,k))^2;
                omega_P2(i,n,k)= omega_2(i,n,k)*abs(audio_cut2_fft(i,k))^2;
%                 if(sum(omega_1(:,n,k))==0||sum(omega_2(:,n,k))==0)
%                     fprintf([num2str(i),'   ',num2str(n),'   ',num2str(k),'\n']);
%                     return;
%                 end
            end
            
        end
    end
       
    for k=1:fft_length
        for n = 1:Ns
            Phi_1(n,k)=mean(omega_1(:,n,k));
            Phi_2(n,k)=mean(omega_2(:,n,k));
            
            Sigma_1(n,k) = sum(omega_P1(:,n,k))/sum(omega_1(:,n,k));
            
            Sigma_2(n,k) = sum(omega_P2(:,n,k))/sum(omega_2(:,n,k));
%             if(isnan(Sigma_1(n,k))||isnan(Sigma_1(n,k)))
%                fprintf([num2str(i),'   ',num2str(n),'   ',num2str(k),'\n']);
%                fprintf('ss');
%                return;
%             end
        end
    end
    Phi_1(:,1:10)
    pause(0.1)
end
toc
                             
