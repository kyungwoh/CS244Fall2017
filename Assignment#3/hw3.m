% Read CSV file
data=csvread('input.csv',1);
time=data(:,1);
IR=data(:,2);
RED=data(:,3);

Fs=50; % Sampling frequency
Fn=Fs/2; % Nyquist frequency
T=1/Fs; % Sampling period
L=length(time); % Length of signal
t=(0:L-1)*T; % Time vector

[b,a] = butter(2,[0.8 1.5]/Fn); % Bandpass Filter (Butterworth)
[SOS_HR,G_HR] = tf2sos(b,a); % Convert to Second-Order-Section For Stability
S_HR = filtfilt(SOS_HR,G_HR,IR); % Filter IR To Recover S_HR
[pksHR,locsHR] = findpeaks(S_HR); % Find peaks
peakIntervalHR = diff(locsHR);
HR = 60.*Fs./(peakIntervalHR); % Find Heart Rate

figure(1);
freqz(IR);
hold on;
freqz(SOS_HR);
freqz(HR);

[b,a] = butter(2,[0.16 0.33]/Fn); % Bandpass Filter (Butterworth)
[SOS_RR,G_RR] = tf2sos(b,a); % Convert to Second-Order-Section For Stability
S_RR = filtfilt(SOS_RR,G_RR,IR); % Filter IR To Recover S_RR
[pksRR,locsRR] = findpeaks(S_RR); % Find peaks
peakIntervalRR = diff(locsRR);
RR = 60.*Fs./(peakIntervalRR); % Calculate Respiration Rate

figure(2);
freqz(IR);
hold on;
freqz(SOS_RR);
freqz(RR);

S_HRi = S_HR.* (-1); %find min peaks from IR
[pksHRi,locsHRi] = findpeaks(S_HRi);
xq = 0:(L-1);
IR_min_int = interp1(locsHRi,IR(locsHRi),xq,'spline'); %interpolation

[b,a] = butter(2,[0.8 1.5]/Fn); %filter RED
[SOS,G] = tf2sos(b,a); % Convert to Second-Order-Section For Stability
S_RED = filtfilt(SOS,G,RED); % Filter RED To Recover S_RED
[pksRED,locsRED] = findpeaks(S_RED); %max peaks of RED

S_REDi = S_RED.*(-1);
[pksREDi,locsREDi] = findpeaks(S_REDi); %min peaks of RED
RED_min_int = interp1(locsREDi,RED(locsREDi),xq,'spline');

IR_max = IR(locsHR);
IR_min = IR_min_int(locsHR)';
RED_max = RED(locsHR);
RED_min = RED_min_int(locsHR)';

AC_IR = IR_max - IR_min;
AC_RED = RED_max - RED_min;
DC_IR = IR_min;
DC_RED = RED_min;

R = AC_RED .* DC_IR ./ AC_IR ./ DC_RED;

figure(3);
plot(IR,'k');
hold on;
plot(RED,'r');
plot(locsHR,IR_max,'r*');
plot(locsHR,IR_min,'g*');
plot(locsHR,RED_max,'r*');
plot(locsHR,RED_min,'g*');

SPO2 = -45.060.*R.*R + 30.354.*R + 94.845; %calculate SPO2

HRnew(1:L) = HR(1);
SPO2new(1:L) = SPO2(1);
for i = 2:length(locsHR)
    HRnew(locsHR(i-1):locsHR(i)) = HR(i-1);
    SPO2new(locsHR(i-1):locsHR(i)) = SPO2(i);
end

RRnew(1:L) = RR(1);
for i = 2:length(locsRR)
    RRnew(locsRR(i-1):locsRR(i)) = RR(i-1);
end

T = array2table([data HRnew' RRnew' SPO2new'],'VariableNames',{'time','IR','RED','HR','RR','SP02'});
writetable(T,'output.csv','Delimiter',',','QuoteStrings',true);
