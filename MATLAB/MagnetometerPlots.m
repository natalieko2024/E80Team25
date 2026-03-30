% MAGNETOMETER LAB 7 PLOTS

%clc

% LOAD DATA FILES
%uncal = readtable('uncalibrated_log.csv'); % Change names of files for actual logging files. 
%cal = readtable('calibrated_log.csv');

% CHANGE THESE COLUMN NAMES TO MATCH THE ACTUAL FILE
mx_uncal = uncalX; % could be uncal.____ fill in blank from actual file column 
my_uncal = uncalY;
t_uncal = [1:474]*0.099;
mx_cal = calX;
my_cal = calY;
t_cal = [1:627]*0.099;

declination = deg2rad(-10.52);

% MX VS MY SCATTER PLOT
figure()
plot(mx_uncal, my_uncal, 'b.')
hold on
plot(mx_cal, my_cal, 'r.')
xlabel('m_x [mG]')
ylabel('m_y [mG]')
title('m_x vs. m_y')
legend('Uncalibrated', 'Calibrated')
axis equal
grid on
hold off

% HEADING VS TIME
%heading_uncal = atan2(my_uncal, mx_uncal) + declination; %adding in declination and arctan stuff
%heading_cal = atan2(my_cal, mx_cal) + declination;

rad_heading_uncal = deg2rad(heading_uncal);
rad_heading_cal = deg2rad(heading_cal);

% for i = 1:length(heading_uncal)
%     if rad_heading_uncal(i) < 0
%         rad_heading_uncal(i) = rad_heading_uncal(i) + 2*pi;   %making sure no negatives. 
%     end
% end
% 
% for i = 1:length(heading_cal)
%     if rad_heading_cal(i) < 0
%         rad_heading_cal(i) = rad_heading_cal(i) + 2*pi;
%     end
% end

figure()
plot(t_uncal, rad_heading_uncal, 'b')
hold on
plot(t_cal, rad_heading_cal, 'r')
xlabel('Time [s]')
ylabel('Heading [rad]')
title('Heading vs. Time')
legend('Uncalibrated', 'Calibrated')
grid on
hold off