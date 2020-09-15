MAC_Feq = 0.7;
Peak_ops = 92;
Peak_ops_sparse_weight = 368;

data = textread('w1_x1_wx1_s1.txt');
plot(data(:,2)./data(:,3) * MAC_Feq,data(:,1));
hold on;
data = textread('w3_x3_wx8_s1.txt');
plot(data(:,2)./data(:,3) * MAC_Feq,data(:,1));
hold on;
data = textread('w8_x8_wx8_s1.txt');
plot(data(:,2)./data(:,3) * MAC_Feq,data(:,1));
plot([1,8] * MAC_Feq,[Peak_ops,Peak_ops]);
legend('w1\_x1\_wx1\_s1',...
       'w3\_x3\_wx8\_s1',...
       'w8\_x8\_wx8\_s1',...
       'peak ops',...
       'location','northwest');
xlabel('RU frequency(GHz)');
ylabel('equivalent speed(Tops)');
% title('comparison of required bandwidth for RU in left with DRAM');
% saveas(gca,'RU_DRAM_BW.jpg');
% close
