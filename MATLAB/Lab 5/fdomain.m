function [X,f]=fdomain(x,Fs)
% FDOMAIN Function to compute the Fourier coefficients from vector x
% and the corresponding frequencies (two-sided)
% usage:
% [X,f]=fdomain(x,Fs)
% x=vector of time domain samples
% Fs=sampling rate (in Hz)
% X=vector of complex Fourier coefficients
% f=vector of corresponding frequencies (two-sided)
N=length(x);
if mod(N,2)==0
k=-N/2:N/2-1; % N even
else
k=-(N-1)/2:(N-1)/2; % N odd
end
T0=N/Fs; % Duration of signal
f=k/T0; % wavenumbers (k) divided by T0 = frequencies
X=fft(x)/N ; % Matlab's FFT uses a different convention without the 1/N so we put it in here.
X=fftshift(X);
end

%% COMMAND WINDOW
% [x1, Fs] = audioread('Shook.wav');
% [x2, Fs] = audioread('ShookEarHurts.wav');
% [X1,f] = fdomain(x1, Fs);
% [X2,f] = fdomain(x2, Fs);
% clf;
% subplot(2,1,1); stem(f, abs(X1)); xlabel('Hz'); ylabel('magnitude'); title('good version')
% subplot(2,1,2); stem(f, abs(X2), 'r'); xlabel('Hz'); ylabel('magnitude'); title('bad version');
% temp[f', abs(X2)];  %look through and remove the data manually

% bandstop = (abs(f) < 2498 | abs(f) > 2502);
% X = X.*bandstop';             % reason for ' is to make dimensions match
% [xfixed, t] = tdomain(X, Fs); % return signal to the time domain
% sound(xfixed, Fs)

% low pass filter
% X2lowpassed = X2.*(abs(f)<2400)';     %conditional gives 0s and 1s
% [x2lowpassed, t] = tdomain(X2lowpassed, Fs);
% sound(xfixed, Fs)