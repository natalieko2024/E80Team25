%% Grab Data
% Voltage values
V_depth = sym(A01) * 0.0032;
V_res = sym(A00) * 0.0032;

% initialize
temperatures = zeros(length(A00):1);

% time in seconds
t = 0:0.1:0.1*(length(A00)-1);

%% Depth calculation
% Depth calculation
depths = -1.48*V_depth + 4.48;

%% Solve for R

%WHITE THERMISTOR
% Back solve for resistance
% define variables
Rp1 = 154545;
Rg = 100000;
Rf = 299600;
Rn1 = 99600;

V_plus = (Rg/(Rp1 + Rg)) * 5;
Vin = (V_res - ((1 + Rf/Rn1) * V_plus)) * (-Rn1/Rf);

depth_R = (((5*47000) ./ Vin) - 47000);

% Get temperature from resistance
a = 0.02348; 
b = -0.005701; 
c = 0.0005159; 
d = -1.487e-05; 

temp = @(R) 1/sym(a+b*log(R)+c*(log(R).^2)+d*(log(R).^3))
for i = 1:length(depth_R)
    temperatures(i) = temp(depth_R(i));
end

% Convert to Celcius
temperatures = double(temperatures) - 273.15;

%% Graphing
% plotting graphs
figure()
title('Depth vs. Time')
plot(t, depth)
%plot(t, depths)
xlabel('Time [s]')
ylabel('Depth [m]')

figure()
title('Temperature vs. Time')
plot(t, temperatures)
xlabel('Time [s]')
ylabel('Temperature [C]')

% Surface temp water = 17.9