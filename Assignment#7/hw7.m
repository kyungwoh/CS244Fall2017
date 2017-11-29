% Read CSV file
data = csvread('hw7_input.csv',1);
Y = data(:,1); % Activity
SEQ = data(:,2);
IR = data(:,3);
RED = data(:,4);
Xdata = data(:,5);
Ydata = data(:,6);
Zdata = data(:,7);

Fs=12.5; % Sampling frequency
Fn=Fs/2; % Nyquist frequency
T=1/Fs; % Sampling period
L=length(SEQ); % Length of signal

[b,a] = butter(5,[75/60 100/60]/Fn); % Bandpass Filter (Butterworth)
[SOS_HR,G_HR] = tf2sos(b,a); % Convert to Second-Order-Section For Stability
S_HR = filtfilt(SOS_HR,G_HR,IR); % Filter IR To Recover S_HR
[pksHR,locsHR] = findpeaks(S_HR); % Find peaks
peakIntervalHR = diff(locsHR);
HR = 60.*Fs./(peakIntervalHR); % Find Heart Rate

[b,a] = butter(5,[15/60 25/60]/Fn); % Bandpass Filter (Butterworth)
[SOS_RR,G_RR] = tf2sos(b,a); % Convert to Second-Order-Section For Stability
S_RR = filtfilt(SOS_RR,G_RR,IR); % Filter IR To Recover S_RR
[pksRR,locsRR] = findpeaks(S_RR); % Find peaks
peakIntervalRR = diff(locsRR);
RR = 60.*Fs./(peakIntervalRR); % Calculate Respiration Rate

S_HRi = S_HR.* (-1); %find min peaks from IR
[pksHRi,locsHRi] = findpeaks(S_HRi);
xq = 0:(L-1);
IR_min_int = interp1(locsHRi,IR(locsHRi),xq,'spline'); %interpolation

[b,a] = butter(5,[75/60 100/60]/Fn); %filter RED
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
SPO2(SPO2<0) = 0;

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


window = 10;

SMV = sqrt(Xdata.^2 + Ydata.^2 + Zdata.^2);
Xmovmean = movmean(Xdata, Fs*window);
Ymovmean = movmean(Ydata, Fs*window);
Zmovmean = movmean(Zdata, Fs*window);
Xmovvar = movvar(Xdata, Fs*window);
Ymovvar = movvar(Ydata, Fs*window);
Zmovvar = movvar(Zdata, Fs*window);

X=[IR RED HRnew' RRnew' SPO2new' Xdata Ydata Zdata SMV Xmovmean Ymovmean Zmovmean Xmovvar Ymovvar Zmovvar];

classOrder=unique(Y);
rng(1); % For reproducibility
t=templateSVM('Standardize',1);
CVMdl=fitcecoc(X,Y,'Holdout',0.20,'Learners',t,'ClassNames',classOrder);
CMdl=CVMdl.Trained{1}; % Extract trained, compact classifier
testInds=test(CVMdl.Partition);  % Extract the test indices
XTest=X(testInds,:);
YTest=Y(testInds,:);
YTestPredict=predict(CMdl,XTest); % Predict the test-sample labels
YTestResult=YTest==YTestPredict;

ErrorRate = 100 - sum(YTestResult)/length(YTestResult)*100;

T = array2table([IR RED Xdata Ydata Zdata HRnew' RRnew' SPO2new' ones(L,1).*ErrorRate],'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T,'team11_assignment7.csv','Delimiter',',','QuoteStrings',true);