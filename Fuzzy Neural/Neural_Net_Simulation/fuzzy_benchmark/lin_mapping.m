function [resultant_x] = lin_mapping(x,old_min,old_max,new_min,new_max)
    slope =  (new_max - new_min)/(old_max - old_min);
    intercept = (-1*old_min) * slope;
    resultant_x = x*slope + intercept;
end
