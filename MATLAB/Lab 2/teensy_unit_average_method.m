%Teensy Unit
length = 5000; %change accordingly this will also be sample number
samp_num = 1:5000;
teensy_out = matlablogging(length);

%known input voltage of 1Vpp
%v_in2 = 1; (should be the peak of the graph V = +0.9)
%v_in1 = 0.5; (should be when V = +0.4)
%corresponding teensy values (get from graphs):
%teensy2 = ;
%teensy1 = ;

%Since there is an offset we need to take that into account
%200Hz: +0.4 V          175kHz: +0.6 V

%taking the average should account for any offset since negs just report 0
%teensy_unit = ((v_in2-v_in1)/(teensy2-teensy1)); 
%voltage/teensy unit

%Create plot of teensy output vs. sample number
figure
plot(samp_num, teensy_out)
xlabel('Sample Number')
ylabel('Teensy Output [?]')
title('Teensy Output vs. Sample Number')