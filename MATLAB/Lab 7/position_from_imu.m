%get the acceleration values
%and corresponding times
conversion = 0.0093;
looptime = 99e-3;
accelerationX = conversion.*accelX;
accelerationY = conversion.*accelY;
t = looptime.*[0:length(accelerationX)-1];

%integrate to get velocity
velocityX = cumtrapz(t, accelerationX);
velocityY = cumtrapz(t, accelerationY);

%integrate to get position
positionX = cumtrapz(t, velocityX);
positionY = cumtrapz(t, velocityY);

%plotting xy coords
figure()
plot([0 0.5 0], [0 0 0], 'r')
hold on
plot(positionX, positionY, 'b')
hold on
xlabel('X Position [m]')
ylabel('Y Position [m]')
title('2D Graph for XY')
legend('Expected Path', 'Actual Path')
%xlim([-1 1])
%ylim([-1 1])
hold off

%% ACCELERATION WITH NOISE
dt = 1/looptime; % The sampling rate
a = accelerationY; % The modeled acceleration
la = length(a);
la2 = round(length(a)/5);
a([la2:end]) = 0; % We only want one cycle of the sine wave.
sigma = .2; % The standard deviation of the noise in the accel.
confLev = 0.95; % The confidence level for bounds
preie = sqrt(2)*erfinv(confLev)*sigma*sqrt(dt); % the prefix to the sqrt(t)
preiie = 2/3*preie; % The prefix to t^3/2a = 1 + sin( pi*t - pi/2);
plusie=preie*t'.^0.5; % The positive noise bound for one integration
plusiie = preiie*t'.^1.5; % The positive noise bound for double integration
en = sigma*randn(size(a)); % Generate the noise
v = cumtrapz(t,a); % Integrate the true acceleration to get the true velocity
r = cumtrapz(t,v); % Integrate the true velocity to get the true position.
an = a + en; % Generate the noisy measured acceleration
vn = cumtrapz(t,an); % Integrate the measured acceleration to get the velocity
vnp = vn + plusie; % Velocity plus confidence bound
vnm = vn - plusie; % Velocity minus confidence bound
rn = cumtrapz(t,vn); % Integrate the velocity to get the position
rnp = rn + plusiie; % Position plus confidence bound
rnm = rn - plusiie; % Position minus confidence bound

%plotting
figure()
plot(t, r, t, rn, t, rnp,'-.', t, rnm,'-.')
xlabel('Time [s]')
ylabel('Position [m]')
title('Calculated Position from Measured Acceleration')
legend('True Velocity','Calculated Velocity','Upper Confidence Bound',...
    'Lower Confidence Bound','location','southeast')
