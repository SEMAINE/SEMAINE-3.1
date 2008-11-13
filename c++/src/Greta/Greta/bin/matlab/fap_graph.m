
function f = fap_graph(filename)

fid = fopen(filename);
if (fid==-1)
   fprintf('\nFile %s non trovato!\n',filename);
   return;
end
maxframes=fscanf(fid,'%d',1);

[r, count] = fscanf(fid,'%d',maxframes); % leggi i tempi di inizio degli stati
FAPS = [];
FAPS = [FAPS; r'];
FRAMES=[];
FRAMES=[0:maxframes-1]; % intervallo dei frames speaker
fclose(fid);

figure('MenuBar','figure','Position',[10,535,400,400]);   
hold on
title('Andamento FAP');
plot(FRAMES,FAPS, 'r')
