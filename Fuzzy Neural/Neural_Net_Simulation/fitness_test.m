function error_average = fitness_test (network)
    total = 0;
    number = 0;
    
%     for i=0:50:255
%         for j = 0:50:255
%             for k = 0:50:255
%                 for l = 0:50:255
%                     input = [i j k l];
%                     fuzzy_output = fuzzy_control_output(input);
%                     scaled_input = lin_mapping(input,0,255,0,1);
%                     scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
%                     neural_output = network.forward_propagation(scaled_input);
%                     number = number + 1;
%                     total = total + abs(scaled_output(1) - neural_output(1));
%                 end
%             end
%         end
%     end
    
    for i=150:50:255
        for j=150:50:255
            %for k = 0:10:255
                %for l = 0:10:255
                    input = [i j 40 60]
                    fuzzy_output = fuzzy_control_output(input);
                    scaled_input = lin_mapping(input,0,255,0,1);
                    scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
                    neural_output = network.forward_propagation(scaled_input);
                    number = number + 1;
                    total = total + abs(scaled_output(1) - neural_output(1));
            %end
        end
    end
        
    
%     %for i=0:10:50
%      %   for j = 0:10:50
%             for k = 200:10:250
%       %          for l = 0:10:50
%                     input = [10 10 k 10];
%                     fuzzy_output = fuzzy_control_output(input);
%                     scaled_input = lin_mapping(input,0,255,0,1);
%                     scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
%                     neural_output = network.forward_propagation(scaled_input);
%                     number = number + 1;
%                     total = total + abs(scaled_output(1) - neural_output(1));
%           %      end
%             end
%         %end
%     %end
% 
%     %for i=0:10:50
%         for j = 200:10:250
%       %      for k = 0:10:50
%        %         for l = 0:10:50
%                     input = [10 j 10 10];
%                     fuzzy_output = fuzzy_control_output(input);
%                     scaled_input = lin_mapping(input,0,255,0,1);
%                     scaled_output = [lin_mapping(fuzzy_output,900,1800,0,1) 0.5]; %Constant speed and fuzzy steering trainer
%                     neural_output = network.forward_propagation(scaled_input);
%                     number = number + 1;
%                     total = total + abs(scaled_output(1) - neural_output(1));
%                 %end
%             %end
%         end
%     %end

    
    error_average = total/number;
end
