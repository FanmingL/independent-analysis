clear; close all;
iva_dir = '.';
audio_dir = '../input.wav';
str = ['mex -I',iva_dir,' ',iva_dir,'/real_time_iva.c ',iva_dir,'/fft.c ',iva_dir,'/iva.c ',iva_dir,'/matrix.c ;'];
if ~exist('real_time_iva','file')
    fprintf(str);
    eval(str);
end

[in, fs]        = audioread(audio_dir);     % here the wav should be a double channel audio
P               = [in;in];                  % train for 2 epoch
fft_length      = 512;                      % number of fft points
shift_size      = fft_length / 4;           % window shitf size
beta            = 0.5;                      % smooth factor according to the paper
eta             = 0.1;                      % learning rate according to the paper
tic;                                        % record time
%%%%%%%%%start implement real-time IVA algorithm%%%
R = real_time_iva(P, fft_length, shift_size, beta, eta);
out = R(end - floor(length(R) / 2):end, :);
%%%%%%%%%%end algorithm%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%judge is the algorithm diverge%%%%%%%%%%%%%%
converge_flag = sum(isnan(out(:)));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



time_period = toc;
str = sprintf('the audio remain %.2f seconds, it is spent %.2f seconds to process it',length(in)/fs, time_period);
if (~converge_flag)
    str = [str, ', and algorithm converged.\n'];
else
    str = [str, ', while algorithm diverge.\n'];
end
fprintf(str);
%% figure
figure
subplot(211)
plot(in(:,1));title('observation1');
subplot(212)
plot(in(:,2));title('observation2');
figure
subplot(211)
plot(out(:,1));title('observation1');
subplot(212)
plot(out(:,2));title('observation2');

