% all_acceleration_over_time_data.m
% Written by Rebecca Kong (rkong@hmc.edu) on 27 Jan 2026
% Modified by Natalie Ko (nko@hmc.edu) on 30 Jan 2026

% Get the acceleration values and corresponding sample numbers
data = accelX;
sampnum = [1:length(data)];
% Set the conversion value from accelerometer units to meters/sec^2
conversion = 9.81/(995.2089+64.6489);

% Plot the data, remember to convert each value in the acceleration arrays
% to meters/sec^2 using .* to multiply each element
plot(sampnum, conversion.*data, 'r') % zero x acceleration
hold on
plot(sampnum, conversion.*accelY, 'b') % zero y acceleration
hold on
plot(sampnum, conversion.*accelZ, 'g') % zero z acceleration
xl = xlabel('Sample Number [N]')
set(xl,'FontSize', 20)
yl = ylabel('Acceleration [m/s^2]')
set(yl,'FontSize', 20)
ldg = legend('X Acceleration', 'Y Acceleration', 'Z Acceleration')
xlim([370 720])
ldg.FontSize = 10;
