


for i = 1:1075
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


plot(angleERr, time);
xlabel("Angle Error [rad]")
ylabel("Time [s]")



plot(time, u, 'LineWidth', 1.5);
xlabel('Time (s)');
ylabel('Control Effort (u)');
title('Control Effort vs Time');
grid on;