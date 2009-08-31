% 0 - Left
% 1 - Right
function data = fuzzy_output_data(min_value,max_value,level)

x = min_value:0.1:max_value;
mean_1 = min_value;
mean_2 = max_value;
variance = (max_value - min_value) / 3;

if level==0
    data = gaussmf(x,[variance mean_1]);
elseif level==1
    data = gaussmf(x,[variance mean_2]);
end

end
