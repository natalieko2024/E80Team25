


for i = 1:1075
    yaw(i) = yaw(i)*pi/180
    yaw_des(i) = yaw_des(i)*pi/180
    
    if yaw(i) < (-pi)
        yaw(i) = yaw_des(i)+2*pi;
    end
    if yaw(i) > (pi)
        yaw(i) = yaw_des(i)- 2*pi;
    end
    if yaw_des(i) < (-pi)
        yaw_des(i) = yaw_des(i)+2*pi;
    end
    if yaw_des(i) > (pi)
        yaw_des(i) = yaw_des(i)- 2*pi;
    end
end


angleERr = yaw_des-yaw;

time = [1:1075]*0.099;

figure()
plot(time, angleERr);
xlabel("Time [s]")
ylabel("Angle Error [rad]")


figure()
plot(time, u, 'LineWidth', 1.5);
xlabel('Time (s)');
ylabel('Control Effort (u)');
title('Control Effort vs Time');
grid on;