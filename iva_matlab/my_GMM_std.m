%initialization
%it spends aboud 0.36 seconds to initialize
%//1 matlab clear
clear;
close all;
%//2 initialize some coeffs according to the paper 

Ns                  = 4;
fft_length          = 512;                      %unit is sample
window_length       = fft_length;               %unit is sample
shift_size          = 256;                      %unit is sample
reverbration_time   = 250;                      %unit is ms
windows             = hanning(fft_length);      %hanning window
expect_sample_rate  = 8000;                     %8kHz sample rate
start_simulate_point= 200000;                   %point start used to simulate
simulation_length   = 90000;                    %length used to simulate, unit is ms
bias                = 0.001;

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
options = statset('Display','final');
GMModel1 = fitgmdist(abs(audio_cut1_fft),9,'CovarianceType','diagonal','Options',options);
GMModel2 = fitgmdist(abs(audio_cut2_fft),9,'CovarianceType','diagonal','Options',options);
V=zeros(2,2,fft_length);
% for k=1:fft_length
%     tmp_fft = [audio_cut1_fft(:,k),audio_cut2_fft(:,k)];
%     tmp = cov(tmp_fft);
%     [E,D]=eig(tmp);
%     V(:,:,k) = diag((diag(D).^(-1/2)))*E';
%     Z = (V(:,:,k)*tmp_fft.').';
%     cov(Z)
%     audio_cut1_fft(:,k) = Z(:,1);
%     audio_cut2_fft(:,k) = Z(:,2);
% end
% omega_1 = zeros(slide_number,Ns);
% omega_2 = zeros(slide_number,Ns);
% Phi_1   = abs(randn(Ns,1));
% Phi_1   = Phi_1./sum(Phi_1,1);
% Phi_2   = abs(randn(Ns,1));
% Phi_2   = Phi_2./sum(Phi_2,1);
% Sigma_1 = abs(randn(fft_length,fft_length,Ns)/2);
% Sigma_2 = abs(randn(fft_length,fft_length,Ns)/2);
% mu1     = abs(zeros(Ns,fft_length));
% mu2     = abs(zeros(Ns,fft_length));



% 
% for i=1:Ns
%     Sigma_1(:,:,i) = Sigma_1(:,:,i) + Sigma_1(:,:,i)';
%     Sigma_2(:,:,i) = Sigma_2(:,:,i) + Sigma_2(:,:,i)';
% end
% Sigma_1 = Sigma_1 + bias;
% Sigma_2 = Sigma_2 + bias;
% omega_P1= zeros(slide_number,Ns,fft_length);
% omega_P2= zeros(slide_number,Ns,fft_length);
% inv_Sigma1_half = zeros(size(Sigma_1));
% inv_Sigma2_half = zeros(size(Sigma_2));
% const1          = zeros(Ns,1);
% const2          = zeros(Ns,1);
% for time = 1:15
%     Sigma_1(Sigma_1<bias)=Sigma_1(Sigma_1<bias)+bias;
%     Sigma_2(Sigma_2<bias)=Sigma_2(Sigma_2<bias)+bias;
%     for n = 1:Ns
%         inv_Sigma1_half(:,:,n) = inv(Sigma_1(:,:,n))*-1/2;
%         inv_Sigma2_half(:,:,n) = inv(Sigma_2(:,:,n))*-1/2;
%         %const1(n)              = log(Phi_1(n)*1/sqrt(2*pi)^fft_length/sqrt(abs(det(Sigma_1(:,:,n))*1e9)));
%         %const2(n)              = log(Phi_2(n)*1/sqrt(2*pi)^fft_length/sqrt(abs(det(Sigma_2(:,:,n))*1e9)));
%         const1(n)               = 0;
%         const2(n)               = 0;
%         
%     end
%     for i = 1:slide_number
%         for n=1:Ns
%             omega_1(i,n) = const1(n)...
%                     +(real(conj(audio_cut1_fft(i,:)-mu1(n,:))*(inv_Sigma1_half(:,:,n)*(audio_cut1_fft(i,:)-mu1(n,:)).')));
%             omega_2(i,n) = const2(n)...
%                     +(real(conj(audio_cut2_fft(i,:)-mu2(n,:))*((inv_Sigma2_half(:,:,n))*(audio_cut2_fft(i,:)-mu2(n,:)).')));
% 
%         end
%         omega_1(i,:)          =  omega_1(i,:) - max(omega_1(i,:));
%         omega_2(i,:)          =  omega_2(i,:) - max(omega_2(i,:));
%         
%         omega_1(i,:)          =  (exp(omega_1(i,:)))/sum(exp(omega_1(i,:)));
%         omega_2(i,:)          =  (exp(omega_2(i,:)))/sum(exp(omega_2(i,:)));
%     end
%     for n=1:Ns
%         mu1(n,:) = omega_1(:,n).'*audio_cut1_fft/slide_number;
%         mu2(n,:) = omega_2(:,n).'*audio_cut2_fft/slide_number;
%     end
%     for n=1:Ns
%         tmp_matrix1 = zeros(fft_length,fft_length);
%         tmp_matrix2 = zeros(fft_length,fft_length);
%         for i=1:slide_number
%             tmp_matrix1 = tmp_matrix1 + omega_1(i,n) * abs((audio_cut1_fft(i,:)-mu1(n,:)).' * conj(audio_cut1_fft(i,:)-mu1(n,:)));
%             tmp_matrix2 = tmp_matrix2 + omega_2(i,n) * abs((audio_cut2_fft(i,:)-mu2(n,:)).' * conj(audio_cut2_fft(i,:)-mu2(n,:)));
%         end
%         Sigma_1(:,:,n) = tmp_matrix1  / slide_number;
%         Sigma_2(:,:,n) = tmp_matrix2  / slide_number;
%     end
%     for n=1:Ns
%         Phi_1(n) = sum(omega_1(:,n))/slide_number;
%         Phi_2(n) = sum(omega_2(:,n))/slide_number;
%     end
%     pause(0.1)
% end
toc
                             
