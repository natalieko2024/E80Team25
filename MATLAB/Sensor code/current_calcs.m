function [u_vel, v_vel] = current_calcs(rho, p, x, y, phi)
    % function that gets the current velocitions along the x and y axis
    % rho = seawater density in kg/m^3
    % p = pressure in (bars?)
    % x = x-positions from gps
    % y = y-positions from gps 
    % phi = latitude reading in radians

    dpdx = diff(p) ./ diff(x);
    dpdy = diff(p) ./ diff(y);
    % dpdx = gradient(p, x);
    % dpdy = gradient(p, y);

    % Coriolis parameter
    omega = 7.29211E-5;         % in 1/s
    f = 2*omega*sin(phi);

    % Current velocity calculations
    u_vel = -dpdy ./ (rho .* f);
    v_vel = dpdx ./ (rho .* f);

end