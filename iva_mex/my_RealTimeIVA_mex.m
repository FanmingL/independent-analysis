iva_dir = './iva_mex';
if ~exist('real_time_iva','file')
    eval(['mex -I',iva_dir,' ',iva_dir,'/real_time_iva.c ',iva_dir,'/fft.c ',iva_dir,'/iva.c ',iva_dir,'/matrix.c ;']);
end
[A, fs] = audioread('input.wav');           % here the wav should be a double channel audio
fft_length      = 512;                      % number of fft points
shift_size      = fft_length / 4;           % window shitf size
beta            = 0.5;                      % smooth factor
eta             = 0.1;                      % learning rate
tic;                                        % record time
%%%%%%%%%start implement real-time IVA algorithm%%%
B = real_time_iva(A, fft_length, shift_size, beta, eta);
%%%%%%%%%%end algorithm%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%judge is the algorithm diverge%%%%%%%%%%%%%%
converge_flag = sum(isnan(B(:)));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



time_period = toc;
str = sprintf('the audio remain %.2f seconds, it is spent %.2f seconds to process it',length(A)/fs, time_period);
if (~converge_flag)
    str = [str, ', and algorithm converged.\n'];
else
    str = [str, ', while algorithm diverge.\n'];
end
fprintf(str);

