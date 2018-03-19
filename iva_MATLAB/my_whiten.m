%initialization
%it spends aboud 0.36 seconds to initialize
%//1 matlab clear
clear;
close all;
%//2 initialize some coeffs according to the paper 
eta                 = 0.00001;                      % study velocity
expect_sample_rate  = 8000;                     % 8kHz sample rate
start_simulate_point= 200000;                   % point start used to simulate
simulation_length   = 80000;                    % length used to simulate, unit is ms
I                   = [1,0;0,1];                % unit matrix
%//3 read audio from files and resample to expected sample rate
[source1,fs1] = audioread('female_record.wav');
[source2,fs2] = audioread('male_record.wav');
source1 = resample(source1,expect_sample_rate,fs1);
source2 = resample(source2,expect_sample_rate,fs2);
source1 = source1(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
source2 = source2(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);

source1_fft = fft(source1);
source2_fft = fft(source2);

source  = [source1_fft, source2_fft].';
source  = source - mean(source')';
OutEstimated  = zeros(size(source));
V       = I;
%//4 RealTime Whiten
for i=1:length(source1)
    OutEstimated(:,i) = V * source(:,i);
    V = V + eta * (I - OutEstimated(:,i)*OutEstimated(:,i)');
end
