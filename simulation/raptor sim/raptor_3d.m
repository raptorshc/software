%% raptor sim (3d)
%
% ==== ASSUMPTIONS MADE ====
% We are a point mass system
% We have no payload other than ourself
% Lift of the parafoil is negliable
% Drag is the only resistive force acting on us in a significant way
% We are stable and oriented after parafoil unfurls
% We naturally tend to orient so the x axis (our forward flying face) is
% oriented with the wind
% Our vertical velocity and acceleration is constant 
%
%% Variables
%
% general things
g = -9.8;     % gravity (m/s^2)
m = 1.58;     % weight of RAPTOR (kg)
t = 0:0.5:30;     % flight time (seconds)

% wind variables
w_v = 5*sin(0.02*pi*t);     % wind speed (m/s)
w_theta = -pi/4;            % wind angle (radians)

% starting positions
p_x = 0;      % starting coordinate in x (meters)
p_y = 0;      % starting coordinate in y (meters)
p_z = 400;    % starting coordinate in z (meters)

% drag stuff (????)
% might be wrong, idk
c_d_box = 1.05;         % drag coefficient of the box
c_d_parafoil = 0.09;    % drag coefficient of the parafoil
rho = 1.225;            % density of air

% drag equations
d_x = 1/2 * rho * w_v * cos(w_theta) * c_d_box * 0.25;         % x-axis drag force
d_y = 1/2 * rho * w_v * sin(w_theta) * c_d_box * 0.25;         % y-axis drag force
d_z = 1/2 * rho * (m*g) * c_d_box * 3;                         % z-axis drag force

%% Functions
% Assumes east, north, up (ENU) coordinates
%
% unit velocity functions
v_x = w_v .* cos(w_theta) - d_x;
v_y = w_v .* sin(w_theta) - d_y;
v_z = (m*g) - d_z;

% unit position functions
p_x = p_x - w_v .* cos(w_theta) .* t - d_x.*t;
p_y = p_y - w_v .* sin(w_theta) .* t - d_y.*t;
p_z = p_z - ((m*g)*t - d_z*t);

% grabs the index of the first 0 value in the z direction
% we want this index for plotting
index = 0;
for n = 1 : length(p_y)
    if p_z(n) < 0
        index = n;
        break
    end 
end

%% Plotting
plot3(0,0,400,'or',p_x(1:index), p_y(1:index), p_z(1:index),'r', p_x(index), p_y(index), p_z(index),'xr')
grid on;
legend({'Origin', 'Trajectory', 'Landing Location'},'Location','southwest')
title('RAPTOR Freefall Model','FontWeight','Normal')
xlabel('x (m)') % x-axis label
ylabel('y (m)') % y-axis label
zlabel('z (m)') % z-axis label