% MAGNETOMETER LAB 7 PLOTS

clc

% LOAD DATA FILES
uncal = readtable('uncalibrated_log.csv'); % Change names of files for actual logging files. 
cal = readtable('calibrated_log.csv');

% CHANGE THESE COLUMN NAMES TO MATCH THE ACTUAL FILE
mx_uncal = uncal.mx; % could be uncal.____ fill in blank from actual file column 
my_uncal = uncal.my;
t_uncal = uncal.time;
mx_cal = cal.mx;
my_cal = cal.my;
t_cal = cal.time;

declination = -10.52;

% MX VS MY SCATTER PLOT
figure()
plot(mx_uncal, my_uncal, 'bo')
hold on
plot(mx_cal, my_cal, 'r.')
xlabel('m_x [\muT]')
ylabel('m_y [\muT]')
title('m_x vs. m_y')
legend('Uncalibrated', 'Calibrated')
axis equal
grid on
hold off

% HEADING VS TIME
heading_uncal = atan2d(my_uncal, mx_uncal) + declination; %adding in declination and arctan stuff
heading_cal = atan2d(my_cal, mx_cal) + declination;

for i = 1:length(heading_uncal)
    if heading_uncal(i) < 0
        heading_uncal(i) = heading_uncal(i) + 360;   %making sure no negatives. 
    end
end

for i = 1:length(heading_cal)
    if heading_cal(i) < 0
        heading_cal(i) = heading_cal(i) + 360;
    end
end

figure()
plot(t_uncal, heading_uncal, 'b')
hold on
plot(t_cal, heading_cal, 'r')
xlabel('Time [s]')
ylabel('Heading [deg]')
title('Heading vs. Time')
legend('Uncalibrated', 'Calibrated')
grid on
hold off