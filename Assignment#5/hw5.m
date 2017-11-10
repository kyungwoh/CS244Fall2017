%load fisheriris

% Read CSV file
data=csvread('sampleXYZ.csv',1);
Xdata=cat(1, data(:,2), data(:,6), data(:,10), data(:,14), data(:,18));
Ydata=cat(1, data(:,3), data(:,7), data(:,11), data(:,15), data(:,19));
Zdata=cat(1, data(:,4), data(:,8), data(:,12), data(:,16), data(:,20));

window=12;
fs=50;

SMV=sqrt(Xdata.^2+Ydata.^2+Zdata.^2);
Xmovmean=movmean(Xdata,fs*window);
Ymovmean=movmean(Ydata,fs*window);
Zmovmean=movmean(Zdata,fs*window);
Xmovvar=movvar(Xdata,fs*window);
Ymovvar=movvar(Ydata,fs*window);
Zmovvar=movvar(Zdata,fs*window);

%X=[Xdata Ydata Zdata];
%X=[Xdata Ydata Zdata SMV];
%X=[Xdata Ydata Zdata SMV Xmovmean Ymovmean Zmovmean];
X=[Xdata Ydata Zdata SMV Xmovmean Ymovmean Zmovmean Xmovvar Ymovvar Zmovvar];
Y=cat(1, data(:,5), data(:,9), data(:,13), data(:,17), data(:,21)); % Activity

classOrder=unique(Y);
rng(1); % For reproducibility
t=templateSVM('Standardize',1);
%Mdl = fitcecoc(X,Y,'Learners',t,'ClassNames',classOrder);
%L = resubLoss(Mdl);
% |CVMdl| is a |ClassificationPartitionedECOC| model. It contains the
% property |Trained|, which is a 1-by-1 cell array holding a
% |CompactClassificationECOC| model that the software trained using the
% training set.
CVMdl=fitcecoc(X,Y,'Holdout',0.30,'Learners',t,'ClassNames',classOrder);
CMdl=CVMdl.Trained{1}; % Extract trained, compact classifier
testInds=test(CVMdl.Partition);  % Extract the test indices
XTest=X(testInds,:);
YTest=Y(testInds,:);
YTestPredict=predict(CMdl,XTest); % Predict the test-sample labels
YTestResult=YTest==YTestPredict;

PredictRatio=sum(YTestResult)/length(YTestResult)*100
