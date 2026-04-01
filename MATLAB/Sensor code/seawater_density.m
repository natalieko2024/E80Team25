function [rho] = seawater_density(s, t, p)
% Computes the sewater density (kg/m^3)
% Inputs:
% s = salinity in psu
% t = temperture in celcius
% p = pressure in bars
% Outputs: 
% rho = seawater density in kg/m^3

% Reference: 
% Unesco (1981). Background papers and supporting data on the
%   International Equation of State of Seawater 1980. 

% Pure water terms Kw, Aw and Bw of secant bulk modulous
Kw = 19652.21 + 148.4206*t - 2.327105*(t^2) + (1.360477E-2)*(t^3) - (5.155288E-5)*(t^4);
Aw = 3.239908 + (1.43713E-3)*t + (1.16092E-4)*(t^2) - (5.77905E-7)*(t^3);
Bw = 8.50935E-5 - (6.12293E-6)*t + (5.2787E-8)*(t^2);

% Secant Bulk Modulus at one standard atmosphere
Ko = Kw + (54.6746 - 0.603459*t + (1.09987E-2)*(t^2) - (6.1670E-5)*(t^3))*s + ...
    (7.944E-2 + (1.6483E-2)*t - (5.3009E-4)*(t^2))*(s^(3/2));
A = Aw + (2.2838E-3 - (1.0981E-5)*t - (1.6078E-6)*(t^2))*s + (1.91075E-4)*(s^(3/2));
B = Bw + (-9.9348E-7 + (2.0816E-8)*t + (9.1697E-10)*(t^2))*s;

% Secant Bulk Modulus
K = Ko + A*p + B*(p^2);

% Density of the Standard Mean Ocean Water (SMOW)
rho_w = 999.842594 + (6.793952E-2)*t - (9.095290E-3)*(t^2) + (1.001685E-4)*(t^3) - ...
    (1.120083E-6)*(t^4) + (6.536332E-9)*(t^5);

% Density of Seawater at one standard atmosphere
rho_o = rho_w + (8.24493E-1 - (4.0899E-3)*t + (7.6438E-5)*(t^2) - (8.2467E-7)*(t^3) + (5.3875E-9)*(t^4))*s + ...
    (-5.72466E-3 + (1.0227E-4)*t - (1.6546E-6)*(t^2))*(s^(3/2)) + (4.8314E-4)*(s^2);

% Density of Seawater at high pressure (0-1000 bar)
rho = rho_o/(1 - p/K);

end