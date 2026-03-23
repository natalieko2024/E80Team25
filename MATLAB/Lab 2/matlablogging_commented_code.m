% matlablogging
% reads from Teensy data stream
% returns a vector teensyanalog that represents the analog signal

function teensyanalog=matlablogging(length)
    length = 5000;  % 5000 is hardcoded buffer size on Teensy --> # of samples
    s = serial('COM7','BaudRate',115200); %rate at which bits transmitted from COM7
    set(s,'InputBufferSize',2*length) %how many bytes that can be stored during data read operation
    fopen(s);                       %opens connection so data can be sent and recieved
    fprintf(s,'%d',2*length)         % Send # of bytes expected to Teensy
    dat = fread(s,2*length,'uint8');     % read data into MATLAB workspace
    fclose(s);                            %stop connection
    teensyanalog = uint8(dat);          % integer values from 0 to 255
    teensyanalog = typecast(teensyanalog,'uint16'); %combines two bytes into one integer that reps voltage
end


%str = fscanf(s);
%teensyanalog = str2num(str);

%[teensyanalog, count] = fscanf(s,['%d']);