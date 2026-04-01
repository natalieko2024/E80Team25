% From the GPS
x_position = [];
y_position = [];

% Earth magentic field noise (might be fixed from calibration?)
x_earth = 0;
y_earth = 0;
z_earth = 0;

% Magentic field measurements from sensor
x_mag = [] - x_earth;
y_mag = [] - y_earth;
z_mag = [] - z_earth;

mag = sqrt(x_mag.^2 + y_mag.^2 + z_mag.^2);

% Transform 1D coordinates into 2D coordinate matrices (grid)
num_grid = 200;
% change min and max based on our values
xi = linspace(0, 5, num_grid);
yi = linspace(0, 5, num_grid);
[Xi, Yi] = meshgrid(xi, yi);

% Interpolate mag onto grid (predicts points where data isnt taken)
Bi = griddata(x_position, y_position, mag, Xi, Yi, 'cubic');

% Contour Plot
figure()
contourf(Xi, Yi, Bi, 30);
colormap(jet);
c = colorbar;
c.Label.String = 'Magnetic Anomaly (µT)';
hold on;
xlabel('X Position (m)');
ylabel('Y Position (m)');
title('Magnetometer Measurement Points');

% Measured Points
scatter(x_position, y_position, 'w.')
hold on;

% Get Peaks
threshold = mean(mag) + 2*std(mag);     % significantly different
i = find(mag > threshold);
plot(x_position(i), y_position(i), 'ro', 'MarkerSize', 20, 'LineWidth', 2);
%legend('Measurement points', 'Object Located');
