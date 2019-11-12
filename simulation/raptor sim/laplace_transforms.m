%% Laplace transforms

% x-unit drag force
%d_x = 1/2 * rho * w_v * cos(w_theta) * c_d_box * 0.25;         % x-axis drag force

% Assumes east, north, up (ENU) coordinates
% x-unit velocity functions
%v_x = w_v .* cos(w_theta) - d_x;

% x-unit position functions
%p_x = p_x - w_v .* cos(w_theta) .* t - d_x.*t;

s = tf('s');
a = 5 * cos(w_theta);
b = 1/2 * rho * 5 * cos(w_theta) * c_d_box * 0.25; 

P_x_num = [a+b];
P_x_dem = [1, 0, 0];

sys = tf(P_x_num,P_x_dem)
[y,t] = step(sys);

plot(t,y);
title('Step Response')
xlabel('Time (seconds)') % x-axis label
ylabel('Amplitude') % y-axis label