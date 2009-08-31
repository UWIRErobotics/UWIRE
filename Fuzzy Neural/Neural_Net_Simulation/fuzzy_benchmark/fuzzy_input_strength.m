%0 - Low, 1 - High
function fuzzy_strength = fuzzy_input_strength(input,high_or_low) 
if high_or_low == 1
    fuzzy_strength = gaussmf(input,[75 255]);
elseif high_or_low == 0
    fuzzy_strength = gaussmf(input,[75 0]);
end
end