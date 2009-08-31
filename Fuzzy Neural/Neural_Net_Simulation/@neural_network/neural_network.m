classdef neural_network < handle
    properties
        num_input = 4
        input
        first_layer = neuron(1)
        output_layer = neuron(1)
    end
    
    methods
        function obj = neural_network(num_network_inputs)
            obj.num_input = num_network_inputs;
            obj.input = ones(4,1);
            for i=1:obj.num_input-1
                obj.first_layer(i,1) = neuron(obj.num_input);
            end
            obj.output_layer(1,1) = neuron(obj.num_input-1); %speed
            obj.output_layer(2,1) = neuron(obj.num_input-1); %steering angle            
           
        end
        
        function network_output = calculate_output(obj,network_inputs)
            obj.input = network_inputs;
            for i=1:obj.num_input-1
                first_layer_output(i,1) = obj.first_layer(i,1).process_output(obj.input);
            end
            for i=1:2 %2 outputs
                network_output(i,1) = obj.output_layer(i,1).process_output(first_layer_output);
            end
            
        end  
    end
end