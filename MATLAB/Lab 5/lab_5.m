%% Simulation
% Sample different sine waves
fs = 50e6;                      % oscilliscope sampling freq
T1 = 1/11e3;
T2 = 1/12e3;
t1 = 0 : 1/fs : 50*T1 - 1/fs;           % 50 cycles
t2 = 0 : 1/fs : 50*T2 - 1/fs;
t3 = t1;

N1 = length(t1);
N2 = length(t2);
N3 = N1;

f1 = (0 : N1-1)*(fs/N1);
f2 = (0 : N2-1)*(fs/N2);
f3 = (0 : N3-1)*(fs/N3);

x1 = 1.5*sin(2*pi*11e3*t1);      % 11kHz, sine
x2 = 1.5*sin(2*pi*12e3*t2);      % 12kHz, sine
x3 = 1.5*square(2*pi*11e3*t3);   % 11kHz, square

% hanning window
h = hann(N3)';

% fft calculation
X1 = fft(x1)/N1;
X2 = fft(x2)/N2;
X3 = fft(x3)/N3;
X4 = fft(x3 .* h)/N3;

% plotting
figure()
subplot(4, 1, 1);
plot(f1, abs(X1));
xlabel('Frequency [Hz]');
xlim([0 100e3])
title('11kHz Sine Wave')

subplot(4, 1, 2);
plot(f2, abs(X2));
xlabel('Frequency [Hz]');
xlim([0 100e3])
title('12kHz Sine Wave')

subplot(4, 1, 3);
plot(f3, abs(X3));
xlabel('Frequency [Hz]');
xlim([0 100e3])
title('11kHz Square Wave')

subplot(4, 1, 4);
figure()
plot(f3, abs(X4));
xlabel('Frequency [Hz]');
xlim([0 100e3])
title('11kHz Square Wave Hanning')

%% FFT using real data
% data from the CSV file
data = readtable('ncope_44.csv', 'NumHeaderLines', 1);
x = data(:, 2);
x = x(2:end, :).Volt;
N = length(x');
%f = (0 : N-1)*(fs/N);

%finding fs of oscope
%t = data(:, 1)
%t = t(2:end, :).second;
%fs = 1/(t(2)-t(1))

f = (-N/2 :N/2-1)*(1.25e7/N);
h_window = hann(N);
X = fft(x .* h_window);

% plotting
figure()
plot(f, fftshift(abs(X)));
xlabel('Frequency [Hz]');
ylabel('Magnitude of Voltage [V]')
xlim([0 100e3])
title('Data with Hanning Window')

%% Analytical Expression describing Power
% I(r) = P_tx/4*pi*r^2
% Power = V^2/R
% V = sqrt(P_tx*R/4*pi*r^2)
% V is proportional to 1/r

%P_tx = 0;   
%c = k*sqrt(P_tx/4*pi);
r = (0:2:20);
V_analytic = 1./r;

beacon1 = [];
beacon2 = [];
beacon3 = [];

% plotting
figure()
plot(r, V_analytic, 'k', r, beacon1, 'bo')
xlabel('Distance [cm]')
ylabel('dBV')
title('Beacon 1')

figure()
plot(r, V_analytic, 'k', r, beacon2, 'bo')
xlabel('Distance [cm]')
ylabel('dBV')
title('Beacon 2')

figure()
plot(r, V_analytic, 'k', r, beacon3, 'bo')
xlabel('Distance [cm]')
ylabel('dBV')
title('Beacon 3')


%% Teensy Matlab Plotting
% grab data from the other teensy file
% DONT FORGET TO CHANGE AND COMMENT OUT
samp_freq1 = 10e3;
samp_freq2 = 100e3;

samp1 = micSignal;
%samp2 = micSignal;

[FFT1, fft_freq1] = fdomain(samp1, 1/samp_freq1);
[FFT2, fft_freq2] = fdomain(samp2, 1/samp_freq2);

fftN1 = length(FFT1);
fftN2 = length(FFT2);

[fft1, t1] = tdomain(FFT1, 1/samp_freq1);
[fft2, t2] = tdomain(FFT2, 1/samp_freq2);

% plotting
figure()
plot(fft_freq1, abs(FFT1), 'k.')
xlabel('Frequency [Hz]'); 
ylabel('Magnitude'); 
title('FFT Magnitude for 10 kS/s Data')

figure()
plot(fft_freq2, abs(FFT2), 'k.')
xlabel('Frequency [Hz]'); 
ylabel('Magnitude'); 
title('FFT Magnitude for 100 kS/s Data')

figure()
plot(t1(1:100), samp1(1:100), 'k.')
xlabel('Time [s]'); 
ylabel('Voltages [V]'); 
title('Time Data for 10 kS/s')

figure()
plot(t2(1:100), samp2(1:100), 'k.')
xlabel('Time [s]'); 
ylabel('Voltages [V]'); 
title('Time Data for 100 kS/s')

