% raptor sim (2d)

%% Variables
g = -9.8;     % m/s^2
m = 1.58;    % kg
t = 0:15;     % seconds
v_w = 10;     % m/s
p_y = 400;    % starting height in meters

% drag stuff (????)
c_d_box = 1.05;
c_d_parafoil = 0.09;
rho = 1.225;

d_h =  1/2 * rho * v_w * c_d_box * 0.25;        % horizontal drag force
d_v = 1/2 * rho * v_w * c_d_box * 3;            % vertical drag force

%% Functions
% Assumes x is horizontal axis and y is vertical axis
%
% velocities
v_x = v_w - d_h;
v_y = (m*g) - d_v;

% positions
p_x = v_w * t - d_h*t;
p_y = p_y + ((m*g)*t - d_v*t);

for n = 1 : length(p_y)
    if p_y(n) < 0
        p_y(n) = 0;
    end 
end

%% Plotting
plot(t, p_x, t, p_y)
grid on;
legend('p_x', 'p_y')
title('Distance traveled from origin','FontWeight','Normal')
xlabel('Time (s)') % x-axis label
ylabel('Distance (m)') % y-axis label