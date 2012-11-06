%%
%% MATLAB script for loading and plotting features extracted with openSMILE
%%

% vis.m
clear X
Np=1;

%export:
filename='tmp.bmp';  winh= 200;

%input:
%file = 'dat/xyz.dat'; normdisp = 0;    % cepstrum
%file = 'mspec.dat'; normdisp = 2; Np=4;
%file = 'tonespec.dat'; normdisp = 1;
file = '../output/pitch.dat'; normdisp = 2; Np=1;

%file = 'dat/fft.dat'; normdisp = 1;
%file = 'mfcc.dat'; normdisp = 1;
%file = 'energy.dat';  normdisp = 2;
%%file = 'dat/lpc.dat';  normdisp = 1;


fid = fopen(file,'r','a');
N = fread(fid,1,'float');   % read vector size from file
nVec = fread(fid,1,'float');   % read number of vectors from file
nVec

start = 0;
len = 5000; %nVec
%for i=1:(len)
i=0;
while (i<len+start)
i=i+1;
c = fread(fid,N,'float');
if (i>start) 
if (length(c) < N) break; end
X(i,:) = c;  %c(1:end-1);
end
end
fclose(fid);
Xorig=X;
%X=X/32700*32700;
x=sum(X,2);  
if (normdisp == 1)
    X=X(:,13:end);
X=abs(X.^0.4);
X=X./max(max(abs(X)));
end

%X=X(1:2:end,:);
%plot(X(220,:));

% energy from fft or melspec: 
%figure
%plot(x);


size(X);
h=figure('Position',[1 950-winh 1280 winh]);
if (normdisp==3)
    plot(X(:,2:4))
elseif (normdisp==2) 
%plot(log(X(:,1)));
%X(:,Np)=X(:,Np)./max(abs(X(:,Np)));
%X(:,Np)=2.^X(:,Np);
plot((X(:,Np))); 
else
imshow((abs(X')));
end

saveas(h,filename);
