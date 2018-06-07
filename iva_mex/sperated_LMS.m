%% MATLAB clear
clear;
close all;

%% hyperparameter
LR = 0.1;               % gradient coefficient
filter_length = 100;
training_epoch = 2;
audio_dir = '../input.wav';
%% data IO
[in, fs] = audioread(audio_dir);

%% parameter initialization
w = zeros(filter_length, size(in, 2));
s = zeros(size(in));
s = [zeros(filter_length - 1, size(in, 2)); s]; % zero padding 
tic;
%% iterations
for epoch = 1 : training_epoch
    for i = 1 : size(in, 1)
        x = s(i : i + filter_length - 1, 2);
        s(i + filter_length - 1, 1) = in(i, 1) - x' * w(:, 1);
        w(:, 1) = w(:, 1) + LR * s(i + filter_length - 1, 1) * x;
        x = s(i : i + filter_length - 1, 1);
        s(i + filter_length - 1, 2) = in(i, 2) - x' * w(:, 2);
        w(:, 2) = w(:, 2) + LR * s(i + filter_length - 1, 2) * x;
    end 
end
out = s(filter_length:end, :);
out = out ./ max(abs(out));
toc

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


