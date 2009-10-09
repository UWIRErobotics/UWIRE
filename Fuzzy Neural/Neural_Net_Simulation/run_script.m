clear all
clear classes

%%
network = neural_network(4);
network_output = network.calculate_output([0 0 0 0]);
network_output
%%
clear
close all

x=0:0.1:255;
y1=gaussmf(x,[75 0]);

plot(x,y1,'b')
axis([0 255 0 1]);
hold on
plot(x,y2,'r')
xlabel('gaussmf, P=[2 5]')

%%
clear all
close all

x = 900:0.1:2200;
y1 = fuzzy_output_data(900,2200,0);
y2 = fuzzy_output_data(900,2200,1);
plot(x,y1)
hold on
plot(x,y2)

%%

close all
clear all
a = [30 240 230 125]; % LE, L, R, RE

%Input Strength Calculations
LE_low = fuzzy_input_strength(a(1),0);
LE_high = fuzzy_input_strength(a(1),1);

L_low = fuzzy_input_strength(a(2),0);
L_high = fuzzy_input_strength(a(2),1);

R_low = fuzzy_input_strength(a(3),0);
R_high = fuzzy_input_strength(a(3),1);

RE_low = fuzzy_input_strength(a(4),0);
RE_high = fuzzy_input_strength(a(4),1);

% Vector Combining Rules Deductions
x = 0:0.1:255;

L_Combine_High = min(L_high,fuzzy_LR_strengths(2));
L_Combine_Med = min(min(L_low,LE_high),fuzzy_LR_strengths(1));
L_Combine_Low = min(min(L_low,LE_low),fuzzy_LR_strengths(0));
L_Combine = max(L_Combine_High,L_Combine_Med);
L_Combine = max(L_Combine, L_Combine_Low);

centroid_num = 0;
centroid_den = 0;
%Finding centroid of resultant fuzzy area
for i=1:numel(L_Combine)
    centroid_num = centroid_num + x(i)*L_Combine(i);
    centroid_den = centroid_den + L_Combine(i);
end
centroid = centroid_num/centroid_den;
centroid_point = centroid_den/numel(L_Combine);


R_Combine_High = min(R_high,fuzzy_LR_strengths(2));
R_Combine_Med = min(min(R_low,RE_high),fuzzy_LR_strengths(1));
R_Combine_Low = min(min(R_low,RE_low),fuzzy_LR_strengths(0));
R_Combine = max(R_Combine_High,R_Combine_Med);
R_Combine = max(R_Combine, R_Combine_Low);

x = 0:0.1:255;
 figure(1)
 %plot(x,max(L_Combine,R_Combine));
 plot(x,L_Combine);
 hold on;
 %plot(centroid,centroid_point,'x');
% 
% figure(2)
% plot(x,R_Combine);
 
 
% plot(x,fuzzy_LR_strengths(0),'b');
% hold on;
% plot(x,fuzzy_LR_strengths(1),'r');
% plot(x,fuzzy_LR_strengths(2),'g');

%Rule Deductions

% R = max(LE_high,L_high);
% L = max(RE_high,R_high);
% 
% min_value = 900;
% max_value = 1800;
% x = min_value:0.1:max_value;
% y1 = min(L,fuzzy_output_data(min_value,max_value,0));
% y2 = min(R,fuzzy_output_data(min_value,max_value,1));
% 
% g=max(y1,y2);
% plot(x,g)
% 
% plot(x,y1,'b')
% hold on
% plot (x,y2,'g')
% 
%%
clear all
close all

fuzzy_control_output([0 255 255 0]);

%% combine inputs
close all
%
plot(0:0.1:255,fuzzy_LR_strengths(0),'r');
hold on
plot(0:0.1:255,fuzzy_LR_strengths(1),'b');
plot(0:0.1:255,fuzzy_LR_strengths(2),'g');
axis([0 255 0 1])
legend('Combine LOW', 'Combine MED','Combine HIGH');

%% sector inputs
close all

hold on
plot(0:0.1:255,fuzzy_input_strength(0:0.1:255,0),'r')
plot(0:0.1:255,fuzzy_input_strength(0:0.1:255,1),'b')
axis([0 255 0 1])
legend('Input LOW', 'Input HIGH');

%% outputs
close all
hold on
plot (900:0.1:1800,fuzzy_output_data(900,1800,0),'r');
plot (900:0.1:1800,fuzzy_output_data(900,1800,1),'b');
legend('Steering LOW', 'Steering HIGH');
%%
clear all
close all

neural_driver = neural_network(4);
%error = fitness_test(neural_driver);
%for count=0:50
for count=0:20
    for i=0:50:255
        for j=0:50:255
            for k = 0:50:255
                for l = 0:50:255
                    input = [i j k l]
                    fuzzy_output = fuzzy_control_output(input);
                    scaled_input = lin_mapping(input,0,255,0,1);
                    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
                    neural_output = neural_driver.forward_propagation(scaled_input);
                    neural_driver.error_backpropagation(scaled_output)
                end
            end 
        end
    end
    count
    %error = [error fitness_test(neural_driver)];
end

% for i=150:50:255
%     for j=150:50:255
%         %for k = 0:10:255
%             %for l = 0:10:255
%                 input = [40 60 j i]
%                 fuzzy_output = fuzzy_control_output(input);
%                 scaled_input = lin_mapping(input,0,255,0,1);
%                 scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
%                 neural_output = neural_driver.forward_propagation(scaled_input);
%                 neural_driver.error_backpropagation(scaled_output)
%             %end
%         %end 
%     end
% end


%% quick test

input = [14 63 136 631];
fuzzy_output = fuzzy_control_output(input);
scaled_input = lin_mapping(input,0,255,0,1);
scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5] %Constant speed and fuzzy steering trainer
neural_output = neural_driver.forward_propagation(scaled_input)

%% weight change test
clear all
close all

neural_driver = neural_network(4);

%% fuzzy test
output_data = [];
fuzzy_output_data=[];
neural_output_data = [];
for i=150:10:255
    input = [i 40 40 40];
    fuzzy_output = fuzzy_control_output(input);
    %output_data = [output_data; fuzzy_output];
    scaled_input = lin_mapping(input,0,255,0,1);
    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
    neural_output = neural_driver.forward_propagation(scaled_input);
    fuzzy_output_data = [fuzzy_output_data; scaled_output(:,1)];
    neural_output_data = [neural_output_data; neural_output(1)];
end

for i=150:10:255
    input = [40 i 40 40];
    
    fuzzy_output = fuzzy_control_output(input);
    %output_data = [output_data; fuzzy_output];
    scaled_input = lin_mapping(input,0,255,0,1);
    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
    neural_output = neural_driver.forward_propagation(scaled_input);
    fuzzy_output_data = [fuzzy_output_data; scaled_output(:,1)];
    neural_output_data = [neural_output_data; neural_output(1)];
end


for i=150:10:255
    input = [40 40 i 40];
    
    fuzzy_output = fuzzy_control_output(input);
    %output_data = [output_data; fuzzy_output];
    scaled_input = lin_mapping(input,0,255,0,1);
    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
    neural_output = neural_driver.forward_propagation(scaled_input);
    fuzzy_output_data = [fuzzy_output_data; scaled_output(1)];
    neural_output_data = [neural_output_data; neural_output(1)];
end

for i=150:10:255
    input = [40 40 40 i];
    
    fuzzy_output = fuzzy_control_output(input);
    %output_data = [output_data; fuzzy_output];
    scaled_input = lin_mapping(input,0,255,0,1);
    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
    neural_output = neural_driver.forward_propagation(scaled_input);
    fuzzy_output_data = [fuzzy_output_data; scaled_output(:,1)];
    neural_output_data = [neural_output_data; neural_output(1)];
end


