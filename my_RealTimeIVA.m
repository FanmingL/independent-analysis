% initialization
%it spends aboud 3.6 seconds to initialize
%matlab clear
clear;
close all;
%initialize some coeffs according to the paper 
fft_length          = 256;                      %unit is sample
shift_size          = 64;                       %unit is sample
beta                = 0.5;                      %smoothing factor
reverbration_time   = 200;                      %unit is ms
windows             = hanning(fft_length);      %hanning window
expect_sample_rate  = 8000;                     %8kHz sample rate
yita                = 1.0;                      %desent gridant coeff
start_simulate_point= 40000;                    %point start used to simulate
simulation_length   = 20000;                    %length used to simulate, unit is ms
c                   = 340;                      % Sound velocity(m/s)
reciver_position1   = [ 2 2 2 ];                % Receiver position[ x y z ](m)
source_position1    = [ 2 2.5 1 ];              % Source position[ x y z ](m)
reciver_position2   = [ 2 2.1 2 ];              % Receiver position[ x y z ](m)
source_position2    = [ 3 2.5 3 ];              % Source position[ x y z ](m)
room_size           = [ 5 4 6 ];                % Room dimensions[ x y z ](m)
number_of_samples   = reverbration_time/1000 ...
                        * expect_sample_rate;   % Number of samples

%read audio from files and resample to expect sample rate
[source1,fs1] = audioread('female_record.mp3');
[source2,fs2] = audioread('male_record.mp3');
source1 = resample(source1,expect_sample_rate,fs1);
source2 = resample(source2,expect_sample_rate,fs2);
source1 = source1(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
source2 = source2(start_simulate_point:floor(start_simulate_point+...
                                       simulation_length/1000*expect_sample_rate),1);
%generate the reverbration audio

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

%implement stft
                             