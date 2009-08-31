function [centroid_x,centroid_y] = centroid(x,y)

centroid_num = 0;
centroid_den = 0;
%Finding centroid of resultant fuzzy area
for i=1:numel(x)
    centroid_num = centroid_num + x(i)*y(i);
    centroid_den = centroid_den + y(i);
end
centroid_x = centroid_num/centroid_den;
centroid_y = centroid_den/numel(y);

end