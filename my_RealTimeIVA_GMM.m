%initialization
%it spends aboud 0.36 seconds to initialize
%//1 matlab clear
clear;
close all;
%//2 initialize some coeffs according to the paper 
gamma_whiten        = 0.001;
lambda              = 1;
Ns                  = 5;
fft_length          = 256;                      %unit is sample
window_length       = fft_length;               %unit is sample
shift_size          = 128;                      %unit is sample
reverbration_time   = 250;                      %unit is ms
windows             = hanning(fft_length);      %hanning window
expect_sample_rate  = 8000;                     %8kHz sample rate
start_simulate_point= 200000;                   %point start used to simulate
simulation_length   = 5000;                    %length used to simulate, unit is ms
c                   = 344;                      % Sound velocity(m/s)
reciver_position1   = [ 2.0 2.0 2.4 ];          % Receiver position[ x y z ](m)
source_position1    = [ 2.0 2.4 2.0 ];          % Source position[ x y z ](m)
reciver_position2   = [ 2.0 2.0 1.9 ];          % Receiver position[ x y z ](m)
source_position2    = [ 2.0 1.6 2.7 ];          % Source position[ x y z ](m)
room_size           = [ 5.0 4.0 6.0 ];          % Room dimensions[ x y z ](m)
number_of_samples   = reverbration_time/1000 ...
                        * expect_sample_rate;   % Number of samples

%//3 read audio from files and resample to expect sample rate
[source1,fs1] = audioread('female_record.wav');
[source2,fs2] = audioread('male_record.wav');
source1 = resample(source1,expect_sample_rate,fs1);
source2 = resample(source2,expect_sample_rate,fs2);
source1 = source1(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
source2 = source2(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
%//4 generate the reverbration audio

impulse11 = rir_generator(c,expect_sample_rate,reciver_position1,source_position1,...
                    room_size,reverbration_time/1000,number_of_samples ) ;
impulse12 = rir_generator(c,expect_sample_rate,reciver_position2,source_position1,...
                    room_size,reverbration_time/1000,number_of_samples ) ;
impulse21 = rir_generator(c,expect_sample_rate,reciver_position1,source_position2,...
                    room_size,reverbration_time/1000,number_of_samples ) ;
impulse22 = rir_generator(c,expect_sample_rate,reciver_position2,source_position2,...
                    room_size,reverbration_time/1000,number_of_samples ) ;

observation1=conv(source1,impulse11)+conv(source2,impulse21);
observation2=conv(source1,impulse12)+conv(source2,impulse22);
contrast_plot(source1,source2,expect_sample_rate,'source');
contrast_plot(observation1,observation2,expect_sample_rate,'observation');


%//5 initialize demixing matrix
W = zeros(2,2,fft_length);
for i=1:fft_length
    W(1,1,i)=1;W(2,2,i)=1;
end
gaussian_s1         = 1:Ns;
gaussian_s2         = 1:Ns;
gaussian_st         = meshgrid(gaussian_s1,gaussian_s2);
gassian_v1          = ones(fft_length,Ns);
gassian_v2          = ones(fft_length,Ns)/2;
p_s1                = ones(1,Ns)/Ns;
p_s2                = ones(1,Ns)/Ns;
p_st                = ones(size(gaussian_st))/Ns/Ns;
q_s1                = ones(1,Ns)/Ns;
q_s2                = ones(1,Ns)/Ns;
q_st                = meshgrid(q_s1,q_s2);
step1_tmp           = zeros(size(q_st));
V                   = W;                                %whitening matrix
I                   = [1,0;0,1];                        %unit matrix 
M                   = W;
beta                = zeros(1,fft_length);
audio_estimate      = zeros(fft_length,2);
audio_estimate_trans= audio_estimate.';
estimate_out1       = zeros(length(observation1),1);
estimate_out2       = zeros(length(observation2),1);
m_1r                = q_s1;
m_1r_old            = zeros(1,Ns);
m_2r                = q_s2;
m_2r_old            = zeros(1,Ns);
signal_whitened     = zeros(2,fft_length);
akbk_star           = zeros(2,1,fft_length);
tic;
%//6 iterator
for n = 1:(floor((length(observation1)-window_length)/shift_size))       % at Nth times
    audio_cut1 = observation1((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows;
    audio_cut2 = observation2((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows;
    audio_cut1_fft = fft(audio_cut1);
    audio_cut2_fft = fft(audio_cut2);
    audio_cut_fft  = [audio_cut1_fft, audio_cut2_fft].';
    for k = 1:floor(fft_length/2+1)
        signal_whitened(:,k) = V(:,:,k)*audio_cut_fft(:,k);
        V(:,:,k) = V(:,:,k)+gamma_whiten*(I-signal_whitened(:,k)*signal_whitened(:,k)')*V(:,:,k);
    end
%     for k = floor(fft_length/2+1)+1:fft_length
%         signal_whitened(:,k) = conj(signal_whitened(:,fft_length-k+2));
%     end
    %E-Step
    for index_s1=1:Ns
        for index_s2=1:Ns
            product_frequency = 0;
            for k=1:floor(fft_length/2+1)
                product_frequency = product_frequency+log(multi_guassian_calculate...
                                            (signal_whitened(:,k),W(:,:,k)'...
                                            *diag([gassian_v1(k,index_s1),gassian_v2(k,index_s2)])*W(:,:,k)));
            end
            q_st(index_s1,index_s2)=log(p_s1(index_s1))+log(p_s2(index_s2))+product_frequency;
        end
    end
    
    q_st = exp(q_st)/sum(sum(exp(q_st))); 
    %M-Step
    for k = 1:floor(fft_length/2+1)
        
        for index_s1=1:Ns
            for index_s2=1:Ns
                step1_tmp(index_s1,index_s2)=...
                    (gassian_v1(k,index_s1)-gassian_v2(k,index_s2))*q_st(index_s1,index_s2);    
            end
        end
        
        M(:,:,k) = lambda*M(:,:,k)+sum(sum(step1_tmp))*signal_whitened(:,k)*signal_whitened(:,k)';
        beta(k) = (M(1,1,k)+M(2,2,k))/2-sqrt(((M(1,1,k)-M(2,2,k))^2)/4+abs(M(1,2,k))^2);
        akbk_star(:,:,k) = 1/sqrt(1+((beta(k)-M(1,1,k))/M(1,2,k))^2)*[1;(beta(k)-M(1,1,k))/M(1,2,k)];
        tmp_a=akbk_star(1,1,k)';
        tmp_b=akbk_star(2,1,k)';
        W(:,:,k) = [tmp_a,tmp_b;-tmp_b',tmp_a'];
        q_s1 = sum(q_st,2).';
        q_s2 = sum(q_st,1);
        
        audio_estimate(k,:) = (W(:,:,k)*signal_whitened(:,k)).';
        m_1r_old = m_1r;
        m_2r_old = m_2r;
        for index=1:Ns
            m_1r = lambda * m_1r + q_s1(index);
            m_2r = lambda * m_2r + q_s2(index);
            gassian_v1(k,index) = 1/(1/gassian_v1(k,index)*m_1r_old(index)/m_1r(index)+...
                                q_s1(index)/m_1r(index)*audio_estimate(k,1)*audio_estimate(k,1)');
            gassian_v2(k,index) = 1/(1/gassian_v2(k,index)*m_2r_old(index)/m_2r(index)+...
                                q_s2(index)/m_2r(index)*audio_estimate(k,2)*audio_estimate(k,2)');
            p_s1(index) = m_1r(index);
            p_s2(index) = m_2r(index);
            
        end
        p_s1=p_s1/sum(p_s1);
        p_s2=p_s2/sum(p_s2);
        audio_estimate(k,:) = (diag(diag(inv(W(:,:,k)*V(:,:,k))))*audio_estimate(k,:).').';
    end
    for k = floor(fft_length/2+1)+1:fft_length
       audio_estimate(k,:) = conj(audio_estimate(fft_length-k+2,:));
    end
    
    estimate_out1((n-1)*shift_size+1:(n-1)*shift_size+window_length)=...
                  ifft(audio_estimate(:,1))+estimate_out1((n-1)*shift_size...
                    +1:(n-1)*shift_size+window_length);
    estimate_out2((n-1)*shift_size+1:(n-1)*shift_size+window_length)=...
                  ifft(audio_estimate(:,2))+estimate_out2((n-1)*shift_size...
                    +1:(n-1)*shift_size+window_length);
end

toc
estimate_out1 = estimate_out1/max(abs(estimate_out1));
estimate_out2 = estimate_out2/max(abs(estimate_out2));

contrast_plot(estimate_out1,estimate_out2,expect_sample_rate,'separated');
function contrast_plot(x,y,fs,title_str)
    figure;
    subplot(2,1,1);
    plot((1:length(x))/fs,x,'b');
    xlabel('time(s)');
    ylabel('relative sound intensity');
    title([title_str,'1']);
    subplot(2,1,2);
    plot((1:length(y))/fs,y,'b');
    title([title_str,'2']);
    xlabel('time(s)');
    ylabel('relative sound intensity');
    pause(0);
end

function p = multi_guassian_calculate(Y,Phi)
p = abs(sqrt(abs(det(Phi)))/2/pi*exp(-1/2*Y'*Phi*Y));
end
                             
