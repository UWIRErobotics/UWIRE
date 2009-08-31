classdef neuron < handle   
    properties
        num_inputs=4;
        output;
        weights;
    end
        
    methods
        function obj = neuron(number_inputs)
            obj.num_inputs = number_inputs;
            obj.weights = ones(obj.num_inputs,1);
        end
        
        function n_output = process_output(obj,inputs)
            sum = 0;
            for i=1:obj.num_inputs
                sum = sum + (obj.weights(i) * inputs(i));
            end
            obj.output = activation_function(obj,sum);
            n_output = obj.output;
        end
               
        function a = activation_function(obj,z) 
            %sigmoid
            a = 1 ./ (1 + exp(-z));
        end
    end 
end
