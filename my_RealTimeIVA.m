%initialization
%it spends aboud 0.36 seconds to initialize
%//1 matlab clear
clear;
close all;
%//2 initialize some coeffs according to the paper 
fft_length          = 256;                      %unit is sample
window_length       = fft_length;               %unit is sample
shift_size          = 64;                       %unit is sample
beta                = 0.5;                     %smoothing factor
reverbration_time   = 250;                      %unit is ms
windows             = hanning(fft_length);      %hanning window
expect_sample_rate  = 8000;                     %8kHz sample rate
yita                = 1.4;                      %gridant desent coeff
start_simulate_point= 120000;                   %point start used to simulate
simulation_length   = 60000;                    %length used to simulate, unit is ms
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
contrast_plot(observation1,observation2,expect_sample_rate,'obeservation');


%//5 initialize demixing matrix
G = zeros(2,2,fft_length);
for i=1:fft_length
    G(1,1,i)=1;G(2,2,i)=1;
end
ksi                 = ones(fft_length,1);
phi                 = ones(fft_length,2);
R                   = zeros(2,2,fft_length);
audio_estimate      = zeros(fft_length,2);
number_of_overlap   = floor(fft_length/shift_size)+1;
estimate_out1       = zeros(length(observation1),1);
estimate_out2       = zeros(length(observation2),1);
norm_coeff          = zeros(fft_length,1);
tic;
%//6 iterator
for n = 1:(floor((length(observation1)-window_length)/shift_size))       % at Nth times
    audio_cut1 = observation1((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows;
    audio_cut2 = observation2((n-1)*shift_size+1:(n-1)*shift_size+window_length).*windows;
    
    audio_cut1_fft = fft(audio_cut1);
    audio_cut2_fft = fft(audio_cut2);
    audio_cut_fft  = [audio_cut1_fft, audio_cut2_fft];
    for k = 1:fft_length
        audio_estimate(k,:) = (G(:,:,k)* (audio_cut_fft(k,:)).').';
        norm_coeff(k)       = norm(G(:,:,k));
    end
    
    phi = audio_estimate ./ (sqrt(diag((audio_estimate)'*(audio_estimate)))');
    for k = 1:fft_length
       R(:,:,k)   = (audio_estimate(k,:)'*phi(k,:)).';
       gradient_k = (diag(diag(R(:,:,k)))-R(:,:,k))*G(:,:,k);
       G(:,:,k)   = G(:,:,k) + yita/sqrt(ksi(k))*gradient_k;
       ksi(k)     = beta * ksi(k) + (1-beta)  *(audio_cut_fft(k,:)*audio_cut_fft(k,:)')/2;
       audio_estimate(k,:) = audio_estimate(k,:) / norm_coeff(k);
    end
    estimate_out1((n-1)*shift_size+1:(n-1)*shift_size+window_length)=...
                  ifft(audio_estimate(:,1))+estimate_out1((n-1)*shift_size...
                    +1:(n-1)*shift_size+window_length);
    estimate_out2((n-1)*shift_size+1:(n-1)*shift_size+window_length)=...
                  ifft(audio_estimate(:,2))+estimate_out2((n-1)*shift_size...
                    +1:(n-1)*shift_size+window_length);
end

toc
contrast_plot(estimate_out1,estimate_out2,expect_sample_rate,'sperated');
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





                             