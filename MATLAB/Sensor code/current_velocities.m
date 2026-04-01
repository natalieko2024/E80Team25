% From the GPS
x_position = [];
y_position = [];
depth = [];

% Get Current Velocities
u_vel = [];             % x-direction
v_vel = [];             % y-direction
w_vel = zeros(u_vel);   % gives each xy-plane

% Plot at one depth
figure()
quiver(x_position, y_position, u_vel, v_vel);
%quiver3(x_position, y_position, depth, u_vel, v_vel, w_vel);
xlabel('X Position [m]')
ylabel('Y Position [m]')
%zlable('Depth [m]')
title('Current Velocity Along XY-Plane');