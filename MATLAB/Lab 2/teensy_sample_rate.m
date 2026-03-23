%Sample Rate: number of samples per second --> Hz
%We get time per a sample and multiply it by number of samples (5000)

%find sample frequency by finding number of samples in one cycle
%number of samples over multiple periods for more accuracy
%measure using cursor

%get number of samples and number of periods
num_samp = GETFROMGRAPH; %number of samples in a period REPLACE GETFROMGRAPH
total_time = (1/200)*num_periods; %REPLACE NUM_PERIODS from graph

samp_rate = num_samp/total_time;  %num of samps per time



%----------------PROBABLY CAN IGNORE THIS IS THE GRAPH STUFF--------
time_per_samp = 1/samp_rate;

%Time vector
time = 1:time_per_samp:5000 * time_per_samp;

%for 200Hz about 25 seconds long total
%for 175kHz about 0.0286 seconds long total (yikes!)

%Get vector of analog values 16bit
analog = matlablogging(5000);

%Plotting data sample from Teensy
%Analog signals [Teensy Unit] vs. Time [s]
figure
plot(time, analog);
xlabel('Time [s]')
ylabel('Teensy Output [?]')
title('Teensy Output vs. Time')

%use cursor to find peaks then sample over many periods

