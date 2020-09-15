MAC_Feq = 0.7;

data1 = textread('withCE.txt');
hold on;
data2 = textread('withoutCE.txt');
hold on;
data = [data1,data2];

bar(data);

% title('comparison of required bandwidth for RU in left with DRAM');
% saveas(gca,'RU_DRAM_BW.jpg');
% close
