% --------------------------------------------------------
% Sintassi: gaze_distr('nomefile')
% --------------------------------------------------------
% legge il file 'nomefile' specificato in input e 
% disegna sei grafici:
%
% 1 - Andamento nel tempo dello sguardo Speaker-Listener
% 2 - Distribuzione dello sguardo per lo Speaker
% 3 - Distribuzione dello sguardo per il Listener
% 4 - Distribuzione (1,1)
% 5 - Distribuzione (0,1)-(1,0)
% 6 - Distribuzione (0,0)
%
% Crea un file con il prefisso 'prob_' (ad es: se nomefile
% e' dati.txt allora avremo prob_dati.txt) dove sono 
% riassunti i dati precedenti e le probabilita' di 
% passare da (s1,l1) a (s2,l2) per i 16 casi possibili:
%

% S\L	 00	 01	 10	 11
%
% 00	 p1	 p2	 p3	 p4
%
% 01	 p5	 p6	 p7	 p8
%
% 10	 p9	 p10	 p11	 p12
%
% 11	 p13	 p14	 p15	 p16
%
% --------------------------------------------------------

function f = gaze_distr(filename)

draw_figures=1; 
menu_flag=1;
solo_andamento=1; 

% se solo_andamento e' 0 stampa l'andamento 0/1 sguardo speaker, listener
% distribuzione stati (0,0),(0,1),(1,0),(1,1) e stampa
% sul file prob_***** le probabilita' di passare da (Sx,Lx) a (Sy,Ly)
% se solo_andamento e' 1 stampa l'andamento dei FAPs degli occhi
% sx/dx speaker/listener e la tabella dei parametri usati.


fid = fopen(filename);
if (fid==-1)
   fprintf('\nFile %s non trovato!\n',filename);
   return;
end
max_t_s1_l1=fscanf(fid,'%f',1);
max_t_s1=fscanf(fid,'%f',1);
max_t_l1=fscanf(fid,'%f',1);
max_t_s0=fscanf(fid,'%f',1);
max_t_l0=fscanf(fid,'%f',1);
dt=fscanf(fid,'%f',1);
limit=fscanf(fid,'%f',1);
scale=fscanf(fid,'%f',1);
counter = fscanf(fid,'%d',1); % leggi numero di stati 0/1
TEMPO = [];
LISTENER = [];
SPEAKER = [];
[r, count] = fscanf(fid,'%f',counter); % leggi i tempi di inizio degli stati
TEMPO = [TEMPO; r'];
[r, count] = fscanf(fid,'%f',counter); % leggi gli stati per il listener
LISTENER = [LISTENER; r'];
[r, count] = fscanf(fid,'%f',counter); % leggi gli stati per lo speaker
SPEAKER = [SPEAKER; r'];

maxframes = fscanf(fid,'%d',1); % leggi il numero di frames speaker
S_FRAMES=[];
S_FRAMES=[0:maxframes-1]; % intervallo dei frames speaker
S_L_EYE = [];
S_R_EYE = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio sx h speaker
S_L_EYE = [S_L_EYE; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio dx h speaker
S_R_EYE = [S_R_EYE; r'];

S_L_EYE_V = [];
S_R_EYE_V = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio sx v speaker
S_L_EYE_V = [S_L_EYE_V; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio dx v speaker
S_R_EYE_V = [S_R_EYE_V; r'];

S_HEAD_H = [];
S_HEAD_V = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps testa h speaker
S_HEAD_H = [S_HEAD_H; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps testa v speaker
S_HEAD_V = [S_HEAD_V; r'];

maxframes = fscanf(fid,'%d',1); % leggi il numero di frames listener
L_FRAMES=[];
L_FRAMES=[0:maxframes-1]; %intervallo dei frames listener
L_L_EYE = [];
L_R_EYE = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio sx h listener
L_L_EYE = [L_L_EYE; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio dx h listener
L_R_EYE = [L_R_EYE; r'];

L_L_EYE_V = [];
L_R_EYE_V = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio sx v listener
L_L_EYE_V = [L_L_EYE_V; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps occhio dx v listener
L_R_EYE_V = [L_R_EYE_V; r'];

L_HEAD_H = [];
L_HEAD_V = [];
[r, count] = fscanf(fid,'%f',maxframes); % faps testa h listener
L_HEAD_H = [L_HEAD_H; r'];
[r, count] = fscanf(fid,'%f',maxframes); % faps testa v listener
L_HEAD_V = [L_HEAD_V; r'];


fclose(fid);

if (solo_andamento==0)
   prob_filename=sprintf('prob_%s',filename);
	fid=fopen(prob_filename,'w');
end

if (solo_andamento==1)
if (draw_figures==1)
if (menu_flag==1)
   figure('MenuBar','figure','Position',[10,535,400,400]);   
else
   figure('MenuBar','none','Position',[10,595,400,400]); 
end
hold on
title('Andamento orizzontale occhi speaker');
plot(S_FRAMES,S_L_EYE, 'r')
%plot(S_FRAMES,S_R_EYE, 'r')
plot(S_FRAMES,S_HEAD_H,'g')
plot(S_FRAMES,S_L_EYE+S_HEAD_H,'b')
%legend('SX','DX');
legend('Eyes','Head','Gaze',-1);
end

if (draw_figures==1)
if (menu_flag==1)
   figure('MenuBar','figure','Position',[415,535,400,400]);   
else
   figure('MenuBar','none','Position',[415,595,400,400]); 
end
hold on
title('Andamento verticale occhi speaker');
plot(S_FRAMES,S_L_EYE_V, 'r')
%plot(S_FRAMES,S_R_EYE_V, 'r')
plot(S_FRAMES,S_HEAD_V,'g')
plot(S_FRAMES,S_L_EYE_V+S_HEAD_V,'b')
%legend('SX','DX');
legend('Eyes','Head','Gaze',-1);
end

if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[10,45,400,400]);   
else
   figure('MenuBar','none','Position',[10,165,400,400]);   
end
hold on
title('Andamento orizzontale occhi listener');
plot(L_FRAMES,L_L_EYE, 'r')
%plot(L_FRAMES,L_R_EYE, 'r')
plot(L_FRAMES,L_HEAD_H,'g')
plot(L_FRAMES,L_L_EYE+L_HEAD_H,'b')
%legend('SX','DX');
legend('Eyes','Head','Gaze',-1);
end

if (draw_figures==1)
if (menu_flag==1)
   figure('MenuBar','figure','Position',[415,45,400,400]);   
else
   figure('MenuBar','none','Position',[415,165,400,400]); 
end
hold on
title('Andamento verticale occhi listener');
plot(L_FRAMES,L_L_EYE_V, 'r')
%plot(L_FRAMES,L_R_EYE_V, 'r')
plot(L_FRAMES,L_HEAD_V,'g')
plot(L_FRAMES,L_L_EYE_V+L_HEAD_V,'b')
%legend('SX','DX');
legend('Eyes','Head','Gaze',-1);
end


if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[820,535,400,400]);   
else
   figure('MenuBar','none','Position',[820,595,400,400]);   
end
axis([0 1 0 10]);
title(texlabel(strcat('File di dati: ',filename)));
text(0.1,8,strcat('T_{{S_1}{L_1}}: ',num2str(max_t_s1_l1)));
text(0.1,7,strcat('T_{S_1}: ',num2str(max_t_s1)));
text(0.1,6,strcat('T_{L_1}: ',num2str(max_t_l1)));
text(0.1,5,strcat('T_{S_0}: ',num2str(max_t_s0)));
text(0.1,4,strcat('T_{L_0}: ',num2str(max_t_l0)));
text(0.1,3,strcat('D_T: ',num2str(dt)));
text(0.1,2,strcat('LIMIT: ',num2str(limit)));
text(0.1,1,strcat('SCALE: ',num2str(scale)));
end
end

% Calcolo andamento nel tempo dello sguardo
if (solo_andamento==0)
if (draw_figures==1)
if (menu_flag==1)
   figure('MenuBar','figure','Position',[10,535,400,400]);   
else
   figure('MenuBar','none','Position',[10,595,400,400]); 
end
hold on
axis([0 max(TEMPO) -0.05 1.10]);
%axis image
%title('Andamento sguardo Speaker-Listener');
%T=strcat('Andamento sguardo Speaker-Listener ',num2str(max(TEMPO),2));
%title(strcat(T,' sec'));
plot(TEMPO,SPEAKER, 'g')
plot(TEMPO,LISTENER, 'r:')
%legend('Speaker','Listener');
xlabel( '0 = look away - 1 = look at' );
end
end

if (solo_andamento==0)
% Calcolo distribuzione dello sguardo per lo speaker
x=0:1;
if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[415,535,400,400]);   
else
   figure('MenuBar','none','Position',[415,595,400,400]);   
end
colormap([0,1,0]);
end

[n,xout]=hist(SPEAKER,x);
if (draw_figures==1)
bar(xout,n);
title( 'Distribuzione sguardo Speaker' );
end
perc_look_at=(n(2)/counter)*100;
perc_look_away=(n(1)/counter)*100;
if (draw_figures==1)
T1=strcat(' % look away: ',num2str(perc_look_away,3));
T2=strcat(' - % look at: ',num2str(perc_look_at,3));
xlabel(strcat(T1,T2));
end

fprintf(fid,'Percentuali di look away/look at per lo Speaker su %.2f secondi\r\n\r\n',max(TEMPO));
fprintf(fid,'look away:\t%3.2f%%\r\n',perc_look_away);
fprintf(fid,'look at:\t%3.2f%%\r\n',perc_look_at);

% Calcolo distribuzione dello sguardo per il listener
if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[820,535,400,400]);   
else
   figure('MenuBar','none','Position',[820,595,400,400]);   
end
colormap([1,0,0]);
end
[n,xout]=hist(LISTENER,x);
if (draw_figures==1)
bar(xout,n);
title( 'Distribuzione sguardo Listener' );
end
perc_look_at=(n(2)/counter)*100;
perc_look_away=(n(1)/counter)*100;
if (draw_figures==1)
T1=strcat(' % look away: ',num2str(perc_look_away,3));
T2=strcat(' - % look at: ',num2str(perc_look_at,3));
xlabel(strcat(T1,T2));
end

fprintf(fid,'\r\nPercentuali di look away/look at per il Listener su %.2f secondi\r\n\r\n',max(TEMPO));
fprintf(fid,'look away:\t%3.2f%%\r\n',perc_look_away);
fprintf(fid,'look at:\t%3.2f%%\r\n',perc_look_at);

x=0:1;
% Calcolo distribuzione (1,1)
if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[10,45,400,400]);   
else
   figure('MenuBar','none','Position',[10,165,400,400]);   
end
colormap([0,0,1]);
end
[n,xout]=hist(SPEAKER&LISTENER,x);
if (draw_figures==1)
bar(xout,n);
title( 'Distribuzione (1,1)' );
end
perc_mutual_gaze=(n(2)/counter)*100;
perc_not_mutual_gaze=(n(1)/counter)*100;
if (draw_figures==1)
T1=strcat(' % no (1,1): ',num2str(perc_not_mutual_gaze,3));
T2=strcat(' - % (1,1): ',num2str(perc_mutual_gaze,3));
xlabel(strcat(T1,T2));
end

fprintf(fid,'\r\nPercentuale di mutual gaze su %.2f secondi: %3.2f%%\r\n',max(TEMPO),perc_mutual_gaze);

% Calcolo distribuzione (0,1)-(1,0)
if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[415,45,400,400]);   
else
   figure('MenuBar','none','Position',[415,165,400,400]);   
end
colormap([1,0,1]);
end
[n,xout]=hist(xor(SPEAKER,LISTENER),x);
if (draw_figures==1)
bar(xout,n);
title( 'Distribuzione (0,1)-(1,0)' );
end
perc_01_10_gaze=(n(2)/counter)*100;
perc_not_01_10_gaze=(n(1)/counter)*100;
if (draw_figures==1)
T1=strcat(' % no (0,1)-(1,0): ',num2str(perc_not_01_10_gaze,3));
T2=strcat(' - % (0,1)-(1,0): ',num2str(perc_01_10_gaze,3));
xlabel(strcat(T1,T2));
end

fprintf(fid,'\r\nPercentuale di gaze (0,1)-(1,0) su %.2f secondi: %3.2f%%\r\n',max(TEMPO),perc_01_10_gaze);

% Calcolo distribuzione (0,0)
if (draw_figures==1)
if (menu_flag==1)
	figure('MenuBar','figure','Position',[820,45,400,400]);   
else
   figure('MenuBar','none','Position',[820,165,400,400]);   
end
colormap([0,1,1]);
end
A = not(SPEAKER);
B = not(LISTENER);
[n,xout]=hist(A&B,x);
if (draw_figures==1)
bar(xout,n);
title( 'Distribuzione (0,0)' );
end
perc_00_gaze=(n(2)/counter)*100;
perc_no_00_gaze=(n(1)/counter)*100;
if (draw_figures==1)
T1=strcat(' % no (0,0): ',num2str(perc_no_00_gaze,3));
T2=strcat(' - % (0,0): ',num2str(perc_00_gaze,3));
xlabel(strcat(T1,T2));
end

% Calcolo delle probabilita' di passare da una coppia (s1,l1) ad una coppia (s2,l2)
% sono 16 casi possibili che vengono salvati nel file prob_filename insieme
% ad alcuni dati che fanno riferimento ai precedenti grafici

fprintf(fid,'\r\nPercentuale di gaze (0,0) su %.2f secondi: %3.2f%%\r\n',max(TEMPO),perc_00_gaze);

MT = ([ SPEAKER ; LISTENER ]);
M = MT';

P= [ [ 0 0 0 0] ; [ 0 0 0 0] ; [0 0 0 0] ; [0 0 0 0] ];

for i=1:counter-1,
   v1 = M(i,:);
   v2 = M(i+1,:);
   if (v1==[0 0]) % prob 00 -> ..
      if (v2==[0 0])
          P(1,1)=P(1,1)+1;
      elseif (v2==[0 1])
          P(1,2)=P(1,2)+1;
      elseif (v2==[1 0])
          P(1,3)=P(1,3)+1;
      elseif (v2==[1 1])
          P(1,4)=P(1,4)+1;
      end
   elseif (v1==[0 1]) % prob 01 -> ..
      if (v2==[0 0])
         P(2,1)=P(2,1)+1;
      elseif (v2==[0 1])
         P(2,2)=P(2,2)+1;
      elseif (v2==[1 0])
         P(2,3)=P(2,3)+1;
      elseif (v2==[1 1])
         P(2,4)=P(2,4)+1;
      end
   elseif (v1==[1 0]) % prob 10 -> ..
      if (v2==[0 0])
         P(3,1)=P(3,1)+1;
      elseif (v2==[0 1])
         P(3,2)=P(3,2)+1;
      elseif (v2==[1 0])
         P(3,3)=P(3,3)+1;
      elseif (v2==[1 1])
         P(3,4)=P(3,4)+1;
      end
   elseif (v1==[1 1]) % prob 11 -> ..
      if (v2==[0 0])
         P(4,1)=P(4,1)+1;
      elseif (v2==[0 1])
         P(4,2)=P(4,2)+1;
      elseif (v2==[1 0])
         P(4,3)=P(4,3)+1;
      elseif (v2==[1 1])
         P(4,4)=P(4,4)+1;
      end
   end
end

for i=1:4,
   for j=1:4,
      P(i,j)=(P(i,j)/(counter-1))*100;
   end
end

fprintf(fid,'\r\nProbabilita'' di passare dallo stato (S1,L1) allo stato (S2,L2)\r\n\r\n');
fprintf(fid,'S\\L\t00\t01\t10\t11\r\n\r\n');
fprintf(fid,'00\t%3.2f\t%3.2f\t%3.2f\t%3.2f\r\n\r\n',P(1,:));
fprintf(fid,'01\t%3.2f\t%3.2f\t%3.2f\t%3.2f\r\n\r\n',P(2,:));
fprintf(fid,'10\t%3.2f\t%3.2f\t%3.2f\t%3.2f\r\n\r\n',P(3,:));
fprintf(fid,'11\t%3.2f\t%3.2f\t%3.2f\t%3.2f\r\n\r\n',P(4,:));
fclose(fid);

end % solo_andamento

%open(prob_filename);
%fid=fopen('test.txt','w');
%fprintf(fid,'%d\r\n',counter);
%for i=1:counter,
%   fprintf(fid,'(%d,%d)\r\n',M(i,:));
%end
%fclose(fid);
