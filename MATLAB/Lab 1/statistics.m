% Script for analysis of data. It assumes the data set is stationary and
% does not have a functional dependence on other variables. For example, a
% set of readings from a voltmeter or mass readings from a scale would
% qualify.
% The inputs consist of the data set and the desired confidence level.
% The script calculates the following:
%    1. The mean or average of the data
%    2. The sample standard deviation of the data
%    3. The count of the data
%    4. The estimated standard error of the data
%    5. The Student-t value
%    6. The confidence interval

% Grab data
[accelX, accelY, accelZ] = logreader('004')

% Replace these with your data set
dataxy = accelX-accelY; 
datayz = accelY-accelZ;
dataxz = accelX-accelZ;

% Arithmetic mean
barxy = mean(dataxy) 
baryz = mean(datayz)
barxz = mean(dataxz)

% Standard Deviation
Sxy = std(dataxy); 
Syz = std(datayz);
Sxz = std(dataxz);

% Variance values
Varxy = Sxy^2;
Varyz = Syz^2;
Varxz = Sxz^2;

% Count [THIS MIGHT NEED FIXING CAUSE DATA DIFF LENGTH]
Nxy = length(dataxy); 
Nyz = length(datayz);
Nxz = length(dataxz);

% Residuals
Resxy = barxy*ones(Nxy) - dataxy;
Resyz = baryz*ones(Nyz) - datayz;
Resxz = barxz*ones(Nxz) - dataxz;

% Estimated Standard Error
ESExy = Sxy/sqrt(Nxy); 
ESEyz = Syz/sqrt(Nyz);
ESExz = Sxz/sqrt(Nxz);

% tinv is for 1-tailed, for 2-tailed we need to halve the range
StdTxy = tinv((1-0.5*(1-confLev)),Nxy-1); % The Student t value
StdTyz = tinv((1-0.5*(1-confLev)),Nyz-1);
StdTxz = tinv((1-0.5*(1-confLev)),Nxz-1);

% 1/2 of the confidence interval ąlambda
lambdaxy = StdTxy*ESExy; 
lambdayz = StdTyz*ESEyz;
lambdaxz = StdTxz*ESExz;

% Confidence intervals
cixy = [barxy-lambdaxy, barxy+lambdaxy]
ciyz = [baryz-lambdayz, baryz+lambdayz]
cixz = [barxz-lambdaxz, barxz+lambdaxz]