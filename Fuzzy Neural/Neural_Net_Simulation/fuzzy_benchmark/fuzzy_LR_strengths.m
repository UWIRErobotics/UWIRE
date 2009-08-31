%0 - Low, 1 - Medium, 2 - High
function fuzzy_strength = fuzzy_LR_strengths(high_med_low) 

input = 0:0.1:255;
variance = 50;
if high_med_low == 0
    fuzzy_strength = gaussmf(input,[variance 0]);
elseif high_med_low == 1
    fuzzy_strength = gaussmf(input,[variance 255/2]);
elseif high_med_low == 2
    fuzzy_strength = gaussmf(input,[variance,255]);
end

end