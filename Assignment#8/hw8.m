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
R(isnan(R)) = 0;
R(R>2) = 0;
R(R<184) = 0;

%figure(3);
%plot(IR,'k');
%hold on;
%plot(RED,'r');
%plot(locsHR,IR_max,'r*');
%plot(locsHR,IR_min,'g*');
%plot(locsHR,RED_max,'r*');
%plot(locsHR,RED_min,'g*');

SPO2 = -45.060.*R.*R + 30.354.*R + 94.845; %calculate SPO2
SPO2(SPO2<0) = 0;
%SPO2(isnan(SPO2)) = 0;

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

X=[SMV Xmovmean Ymovmean Zmovmean Xmovvar Ymovvar Zmovvar];

classOrder=unique(Y);
rng(1); % For reproducibility
t=templateSVM('Standardize',1,'KernelFunction','gaussian');
CVMdl=fitcecoc(X,Y,'Holdout',0.50,'CrossVal','on','Learners',t,'ClassNames',classOrder);
CMdl=CVMdl.Trained{1}; % Extract trained, compact classifier
testInds=test(CVMdl.Partition);  % Extract the test indices
XTest=X(testInds,:);
YTest=Y(testInds,:);
YTestPredict=predict(CMdl,XTest); % Predict the test-sample labels
YTestResult=YTest==YTestPredict;

ErrorRate = 100 - sum(YTestResult)/length(YTestResult)*100;

output = [IR RED Xdata Ydata Zdata HRnew' RRnew' SPO2new' ones(L,1).*ErrorRate];

T0 = array2table(output(1:7500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T0,'kyungwoh/sleeping/kyuseony_assignment8_sleeping.csv','Delimiter',',','QuoteStrings',true);

T1 = array2table(output(7501:15000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T1,'kyungwoh/sitting/kyuseony_assignment8_sitting.csv','Delimiter',',','QuoteStrings',true);

T2 = array2table(output(15001:22500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T2,'kyungwoh/standing/kyuseony_assignment8_standing.csv','Delimiter',',','QuoteStrings',true);

T3 = array2table(output(22501:30000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T3,'kyungwoh/walking/kyuseony_assignment8_walking.csv','Delimiter',',','QuoteStrings',true);


T10 = array2table(output(30001:37500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T10,'kyungwoh/sleeping/kyungwoh_assignment8_sleeping.csv','Delimiter',',','QuoteStrings',true);

T11 = array2table(output(37501:45000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T11,'kyungwoh/sitting/kyungwoh_assignment8_sitting.csv','Delimiter',',','QuoteStrings',true);

T12 = array2table(output(45001:52500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T12,'kyungwoh/standing/kyungwoh_assignment8_standing.csv','Delimiter',',','QuoteStrings',true);

T13 = array2table(output(52501:60000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T13,'kyungwoh/walking/kyungwoh_assignment8_walking.csv','Delimiter',',','QuoteStrings',true);


T20 = array2table(output(60001:67500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T20,'kyungwoh/sleeping/heejh_assignment8_sleeping.csv','Delimiter',',','QuoteStrings',true);

T21 = array2table(output(67501:75000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T21,'kyungwoh/sitting/heejh_assignment8_sitting.csv','Delimiter',',','QuoteStrings',true);

T22 = array2table(output(75001:82500,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T22,'kyungwoh/standing/heejh_assignment8_standing.csv','Delimiter',',','QuoteStrings',true);

T23 = array2table(output(82501:90000,:),'VariableNames',{'IR','RED','X','Y','Z','HR','RR','SP02','ERROR_RATE'});
writetable(T23,'kyungwoh/walking/heejh_assignment8_walking.csv','Delimiter',',','QuoteStrings',true);