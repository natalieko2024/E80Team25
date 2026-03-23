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

data = accelZ; % CHOOSE WHICH ACCELERATION
confLev = 0.95;
xbar = mean(data) % Arithmetic mean
S = std(data) % Standard Deviation
V = S^2 % Variance
N = length(data); % Count
R = xbar*ones(N) - data; % Residuals
ESE = S/sqrt(N) % Estimated Standard Error
% tinv is for 1-tailed, for 2-tailed we need to halve the range
StdT = tinv((1-0.5*(1-confLev)),N-1); % The Student t value
lambda = StdT*ESE; % 1/2 of the confidence interval ąlambda
ci = [xbar-lambda, xbar+lambda] % Confidence interval