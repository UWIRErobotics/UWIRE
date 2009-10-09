classdef neural_network < handle
    properties
        num_input = 4
        learning_rate = 1
        momentum = 0.8
        network_input
        network_output
        first_layer_output
        error_value_1
        error_value_2
        first_layer = neuron(1)
        output_layer = neuron(1)
        del_weights1
        del_weights2
        del_weights3
        del_weights4
        del_weights5
    end
    
    methods
        function obj = neural_network(num_network_inputs)
            obj.num_input = num_network_inputs;
            obj.network_input = ones(4,1);
            obj.first_layer_output = ones(3,1);
            obj.network_output = ones(2,1);
            obj.error_value_1 = 0;
            obj.error_value_2 = 0;
            for i=1:obj.num_input-1
                obj.first_layer(i,1) = neuron(obj.num_input);
            end
            obj.output_layer(1,1) = neuron(obj.num_input-1); %speed
            obj.output_layer(2,1) = neuron(obj.num_input-1); %steering angle
            obj.del_weights1=zeros(1,3);
            obj.del_weights2=zeros(1,3);
            obj.del_weights3=zeros(1,4);
            obj.del_weights4=zeros(1,4);
            obj.del_weights5=zeros(1,4);
        end
        
        function network_output = forward_propagation(obj,network_inputs)
            obj.network_input = network_inputs;
            %Process input to generate output for first layer
            for i=1:obj.num_input-1
                first_layer_output(i,1) = obj.first_layer(i,1).process_output(obj.network_input);
            end
            obj.first_layer_output = first_layer_output;
            
            %Process output from first layer to generation network output
            for i=1:2 %2 outputs
                network_output(i,1) = obj.output_layer(i,1).process_output(first_layer_output);
            end 
            obj.network_output = network_output;
        end
        
        function error_backpropagation(obj,required_output)
           %Compute Error Value
           obj.error_value_1 = 0.5*(required_output(1) - obj.network_output(1)) + obj.error_value_1;
           obj.error_value_2 = 0.5*(required_output(2) - obj.network_output(2)) + obj.error_value_2;
           
           %Compute Output Layer Error Signal
           signal_output_1 = obj.network_output(1)*(1 - obj.network_output(1))*(required_output(1) - obj.network_output(1));
           signal_output_2 = obj.network_output(2)*(1 - obj.network_output(2))*(required_output(2) - obj.network_output(2));
           
           %Compute and Apply Change in Weights for Output 1
%           output1_delweight = obj.learning_rate * signal_output_1 * obj.first_layer_output;
           del_weight_1 = obj.learning_rate * signal_output_1 * obj.first_layer_output(1);
           del_weight_2 = obj.learning_rate * signal_output_1 * obj.first_layer_output(2);
           del_weight_3 = obj.learning_rate * signal_output_1 * obj.first_layer_output(3);
           obj.del_weights1 = [obj.del_weights1; del_weight_1 del_weight_2 del_weight_3];

%           obj.output_layer(1).weights = obj.output_layer(1).weights + output1_delweight;
           obj.output_layer(1).weights(1) = obj.output_layer(1).weights(1) + del_weight_1;
           obj.output_layer(1).weights(2) = obj.output_layer(1).weights(2) + del_weight_2;
           obj.output_layer(1).weights(3) = obj.output_layer(1).weights(3) + del_weight_3;
           
           %Compute and Apply Change in Weights for Output 2
%           output2_delweight = obj.learning_rate *signal_output_2 *obj.first_layer_output;
           del_weight_1 = obj.learning_rate * signal_output_2 * obj.first_layer_output(1);
           del_weight_2 = obj.learning_rate * signal_output_2 * obj.first_layer_output(2);
           del_weight_3 = obj.learning_rate * signal_output_2 * obj.first_layer_output(3);
           obj.del_weights2 = [obj.del_weights2; del_weight_1 del_weight_2 del_weight_3];
           
%           obj.output_layer(2).weights = obj.output_layer(2).weights + output2_delweight;
           obj.output_layer(2).weights(1) = obj.output_layer(2).weights(1) + del_weight_1;
           obj.output_layer(2).weights(2) = obj.output_layer(2).weights(1) + del_weight_2;
           obj.output_layer(2).weights(3) = obj.output_layer(2).weights(1) + del_weight_3;
           
           %Compute second layer error signals
           signal_sec_layer_1 = obj.first_layer_output(1)*(1-obj.first_layer_output(1)) *  ((obj.output_layer(1).weights(1) * signal_output_1) + (obj.output_layer(2).weights(1) * signal_output_2));
           signal_sec_layer_2 = obj.first_layer_output(2)*(1-obj.first_layer_output(2)) *  ((obj.output_layer(1).weights(2) * signal_output_1) + (obj.output_layer(2).weights(2) * signal_output_2));
           signal_sec_layer_3 = obj.first_layer_output(3)*(1-obj.first_layer_output(3)) *  ((obj.output_layer(1).weights(3) * signal_output_1) + (obj.output_layer(2).weights(3) * signal_output_2));
           
           
           %Compute and Apply Change in Weights for First Layer 
           
           %FIRST Neuron
%            n1_delweight = obj.learning_rate * signal_sec_layer_1 * obj.network_input;
%            obj.first_layer(1).weights = obj.first_layer(1).weights + n1_delweight';
           del_weight_1 = obj.learning_rate * signal_sec_layer_1 * obj.network_input(1);
           del_weight_2 = obj.learning_rate * signal_sec_layer_1 * obj.network_input(2);
           del_weight_3 = obj.learning_rate * signal_sec_layer_1 * obj.network_input(3);
           del_weight_4 = obj.learning_rate * signal_sec_layer_1 * obj.network_input(4);
           obj.first_layer(1).weights(1)= obj.first_layer(1).weights(1) + del_weight_1;
           obj.first_layer(1).weights(2)= obj.first_layer(1).weights(2) + del_weight_2;
           obj.first_layer(1).weights(3)= obj.first_layer(1).weights(3) + del_weight_3;
           obj.first_layer(1).weights(4)= obj.first_layer(1).weights(4) + del_weight_4;
           
           obj.del_weights3 = [obj.del_weights3; del_weight_1 del_weight_2 del_weight_3 del_weight_4];
                      
           %SECOND Neuron
%            n2_delweight = obj.learning_rate * signal_sec_layer_2 * obj.network_input;
%            obj.first_layer(2).weights = obj.first_layer(2).weights + n2_delweight';
           
           del_weight_1 = obj.learning_rate * signal_sec_layer_2 * obj.network_input(1);
           del_weight_2 = obj.learning_rate * signal_sec_layer_2 * obj.network_input(2);
           del_weight_3 = obj.learning_rate * signal_sec_layer_2 * obj.network_input(3);
           del_weight_4 = obj.learning_rate * signal_sec_layer_2 * obj.network_input(4);
           obj.first_layer(2).weights(1)= obj.first_layer(2).weights(1) + del_weight_1;
           obj.first_layer(2).weights(2)= obj.first_layer(2).weights(2) + del_weight_2;
           obj.first_layer(2).weights(3)= obj.first_layer(2).weights(3) + del_weight_3;
           obj.first_layer(2).weights(4)= obj.first_layer(2).weights(4) + del_weight_4;
           obj.del_weights4 = [obj.del_weights4; del_weight_1 del_weight_2 del_weight_3 del_weight_4];
           
           %THIRD Neuron
%            n3_delweight = obj.learning_rate * signal_sec_layer_3 * obj.network_input;
%            obj.first_layer(3).weights = obj.first_layer(3).weights + n3_delweight';
           del_weight_1 = obj.learning_rate * signal_sec_layer_3 * obj.network_input(1);
           del_weight_2 = obj.learning_rate * signal_sec_layer_3 * obj.network_input(2);
           del_weight_3 = obj.learning_rate * signal_sec_layer_3 * obj.network_input(3);
           del_weight_4 = obj.learning_rate * signal_sec_layer_3 * obj.network_input(4);
           obj.first_layer(3).weights(1)= obj.first_layer(3).weights(1) + del_weight_1;
           obj.first_layer(3).weights(2)= obj.first_layer(3).weights(2) + del_weight_2;
           obj.first_layer(3).weights(3)= obj.first_layer(3).weights(3) + del_weight_3;
           obj.first_layer(3).weights(4)= obj.first_layer(3).weights(4) + del_weight_4;
           obj.del_weights5 = [obj.del_weights5; del_weight_1 del_weight_2 del_weight_3 del_weight_4];
           
           %All Weight Updates are now done.
        end
    end
end