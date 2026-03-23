%% Data values
T = []; %INPUT DATA (in Kelvins)
% if need to convert celcius to kelvin
T = T + 273.15;
R = []; %INPUT DATA (resistor values)
confLev = 0.95;

% %% Test values
% % The resistances.
% R = [473200, 441800, 412600, 385600, 360500, 337200, 315500, 295400, ...
%     276700, 259300, 243100, 228000, 213900, 200800, 188600, 177200, ...
%     166500, 156600, 147300, 138600, 130500, 122900, 115800, 109200, ...
%     103000, 97130, 91660, 86540, 81730, 77220, 72980, 69000, 65260, ...
%     61750, 58450, 55340, 52420, 49670, 47080, 44640, 42340, 40170, ...
%     38120, 36200, 34380, 32660, 31040, 29510, 28060, 26690, 25400, ...
%     24180, 23020, 21920, 20890, 19900, 18970, 18090, 17260, 16470, ...
%     15710, 15000, 14320, 13680, 13070, 12490, 11940, 11420, 10920, ...
%     10450, 10000, 9572, 9165, 8777, 8408, 8056, 7721, 7402, 7098, ...
%     6808, 6531, 6267, 6015, 5774, 5545, 5326, 5116, 4916, 4725, 4543, ...
%     4368, 4201, 4041, 3888, 3742, 3602];
% 
% % The noisy tem[peratures.
% T = [237.89, 222.29, 220.44, 229.68, 223.80, 240.13, 224.09, 236.63, ...
%     239.87, 236.67, 241.33, 230.53, 244.87, 243.76, 253.48, 248.04, ...
%     245.11, 241.58, 250.99, 251.27, 254.79, 247.07, 244.51, 240.73, ...
%     259.03, 247.82, 253.96, 246.75, 249.61, 260.59, 245.99, 270.61, ...
%     262.16, 251.45, 256.72, 250.46, 259.38, 277.45, 267.59, 270.90, ...
%     258.47, 257.87, 268.08, 267.12, 266.16, 283.41, 268.84, 273.66, ...
%     263.59, 274.50, 291.04, 281.83, 267.99, 277.94, 278.47, 275.93, ...
%     297.04, 273.29, 296.58, 283.65, 275.19, 284.87, 290.91, 302.05, ...
%     283.67, 290.39, 299.32, 308.30, 300.72, 295.69, 298.54, 289.95, ...
%     289.15, 313.87, 307.66, 316.51, 304.86, 308.64, 292.66, 293.79, ...
%     312.59, 299.79, 322.10, 298.11, 317.17, 306.48, 326.64, 303.52, ...
%     320.89, 308.95, 325.71, 315.08, 317.71, 317.80, 333.34, 337.38];

%% Plot Original Data
% Plot the resistance vs temperature.
figure() 
plot(R,T)
xlabel('Resistance (\Omega)')
ylabel('Temperature (K)')
title('Temperature vs. Resistance')

%% Nonlinear Fit
% To compare this transformed linear fit of a polynomial with non-linear, let's
% do a non-linear fit using the Steinhart-Hart equation (but we'll include
% the 2nd-order term.
range = max(R) - min(R); % Get range for our xplot data
xplot = min(R):range/30:max(R); % Generate x data for some of our plots.

% First we have to define the function we will fit.
% Things work better if we have starting points for a, b, c, and d. We'll
% use our values from above and 'fitoptions'
fo = fitoptions('Method','NonlinearLeastSquares',...
    'StartPoint',[-0.002894 0.001339 -9.963e-05 3.053e-06]);
ft = fittype('1/(a+b*log(R)+c*(log(R)^2)+d*(log(R)^3))','independent',...
    'R','options',fo);

% Next, we have to get our data into the correct format for 'fit'.
[Xout,Yout] = prepareCurveData(R, T);

% Now we'll do our fit.
% gives us the coefficient values as well as statistics
[f1,stat1] = fit(Xout,Yout,ft)

p11 = predint(f1,xplot,confLev,'observation','off'); % Gen conf bounds
p21 = predint(f1,xplot,confLev,'functional','off'); % Gen conf bounds

%plotting the data
figure()
plot(f1,Xout,Yout) % Notice that the fit doesn't need both x and y.
hold on
plot(xplot, p21, '-.b') % Upper and lower functional confidence limits
plot(xplot, p11, '--m') % Upper and lower observational confidence limits
xlabel('Resistance (\Omega)')
ylabel('Temperature (K)')
title('Steinhart-Hart Fit')
legend('Data Points','Best Fit Line','Upper Func. Bound',...
    'Lower Func. Bound', 'Upper Obs. Bound', 'Lower Obs. Bound',...
    'Location', 'northeast')
hold off

%% Nonlinear Residuals
% Plots of the residuals in the nonlinear fitted data
figure()
plot(f1,Xout,Yout,'residuals')
xlabel('Resistance (\Omega)')
ylabel('Residuals (K)')
title('Steinhart-Hart Fit')