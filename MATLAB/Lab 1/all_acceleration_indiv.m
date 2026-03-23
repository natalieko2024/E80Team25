%get the acceleration values in the X direction 
%and corresponding sample numbers
datax = accelX;
sampnumx = [1:length(datax)];

%CHANGE TO START AND END TIME
startx = min(sampnumx);
endx = max(sampnumx);

%plot the data
plot(sampnumx, datax, 'k') % zero x acceleration (line)
hold on
plot(sampnumx, datax, 'ok') % zero x acceleration

%label axes and title
xl = xlabel('Sample Number')
set(xl,'FontSize', 20)
yl = ylabel('Acceleration [mg]')
set(yl,'FontSize', 20)
ttl = title('Zero Acceleration X Data vs. Sample Time')
set(ttl,'FontSize', 30)
%boundaries
xlim([startx endx])


%Y ACCELERATION PLOT
figure
%get the acceleration values in the Y direction 
%and corresponding sample numbers
datay = accelY;
sampnumy = [1:length(datay)];

%CHANGE TO START AND END TIME
startx_y = min(sampnumy);
endx_y = max(sampnumy);

%plot the data
plot(sampnumy, datay, 'k') % zero y acceleration
hold on
plot(sampnumy, datay, 'ok') % zero y acceleration
%label axes and title
x2 = xlabel('Sample Number')
set(x2,'FontSize', 20)
y2 = ylabel('Acceleration [mg]')
set(y2,'FontSize', 20)
ttl2 = title('Zero Acceleration Y Data vs. Sample Time')
set(ttl2,'FontSize', 30)
%set boundaries
xlim([startx_y endx_y])


%Z ACCELERATION PLOT
figure
%get the acceleration values in the Z direction 
%and corresponding sample numbers
dataz = accelZ;
sampnumz = [1:length(dataz)];

%CHANGE TO START AND END TIME
startx_z = min(sampnumz);
endx_z = max(sampnumz);

%plot the data
plot(sampnumz, dataz, 'k') % zero y acceleration
hold on
plot(sampnumz, dataz, 'ok') % zero y acceleration
%label axes and title
x3 = xlabel('Sample Number')
set(x3,'FontSize', 20)
y3 = ylabel('Acceleration [mg]')
set(y3,'FontSize', 20)
ttl3 = title('Zero Acceleration Z Data vs. Sample Time')
set(ttl3,'FontSize', 30)
%set boundaries
xlim([startx_z endx_z])