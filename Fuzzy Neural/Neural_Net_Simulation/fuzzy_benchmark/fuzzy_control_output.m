function [str_angle strength] = fuzzy_control_output(sector_input)
    x = 0:0.1:255; %range of inputs
    output_min = 900;
    output_max = 1800;
    output_min = output_min - 500;
    output_max = output_max + 500;
    

    %Input Strength Calculations
    LE_low = fuzzy_input_strength(sector_input(1),0);
    LE_high = fuzzy_input_strength(sector_input(1),1);

    L_low = fuzzy_input_strength(sector_input(2),0);
    L_high = fuzzy_input_strength(sector_input(2),1);

    R_low = fuzzy_input_strength(sector_input(3),0);
    R_high = fuzzy_input_strength(sector_input(3),1);

    RE_low = fuzzy_input_strength(sector_input(4),0);
    RE_high = fuzzy_input_strength(sector_input(4),1);

    %Combine Left Sections into one
    L_Combine_High = min(L_high,fuzzy_LR_strengths(2));
    L_Combine_Med = min(min(L_low,LE_high),fuzzy_LR_strengths(1));
    L_Combine_Low = min(min(L_low,LE_low),fuzzy_LR_strengths(0));
    L_Combine = max(L_Combine_High,L_Combine_Med);
    L_Combine = max(L_Combine, L_Combine_Low);

    [L_centroid_x,L_centroid_y] = centroid(x,L_Combine);
    % figure(1)
    % plot(x,L_Combine);
    % hold on;
    % plot(L_centroid_x,L_centroid_y,'rx');
    left_section_force = lin_mapping(L_centroid_x,40.4872,214.5128,0,255);

    %Combine Right Sections into one
    R_Combine_High = min(R_high,fuzzy_LR_strengths(2));
    R_Combine_Med = min(min(R_low,RE_high),fuzzy_LR_strengths(1));
    R_Combine_Low = min(min(R_low,RE_low),fuzzy_LR_strengths(0));
    R_Combine = max(R_Combine_High,R_Combine_Med);
    R_Combine = max(R_Combine, R_Combine_Low);
    [R_centroid_x,R_centroid_y] = centroid(x,R_Combine);
    right_section_force = lin_mapping(R_centroid_x,40.4872,214.5128,0,255);

    left_high = fuzzy_input_strength(left_section_force,1);
    right_high = fuzzy_input_strength(right_section_force,1);

    right_output= min(left_high,fuzzy_output_data(output_min,output_max,1));
    left_output = min(right_high,fuzzy_output_data(output_min,output_max,0));

    output_x = output_min:0.1:output_max;
    fuzzy_output = max(left_output,right_output);
    [str_angle strength] = centroid(output_x,fuzzy_output);

end