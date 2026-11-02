% Reconstructed MATLAB example for the Athlete Reaction Time Training System.
% This is not the original project code and may differ from the prototype software.
% Starts a session on the Arduino and plots reaction time vs time live.
% Requires MATLAB R2019b or newer (uses serialport).

clear; clc; close all;

port = "COM3";        % change to your port (e.g. "/dev/ttyACM0" or "/dev/cu.usbmodem1101")
baud = 115200;

s = serialport(port, baud);
configureTerminator(s, "LF");
pause(2);             % Arduino resets when the port opens
flush(s);

% Set up the plot
fig = figure("Name", "Reaction Time");
h = plot(nan, nan, "o-", "LineWidth", 1.5, "MarkerFaceColor", "b");
grid on;
xlabel("Time since start (s)");
ylabel("Reaction time (ms)");
title("Athlete Reaction Time");

times = [];
reactions = [];
buttons = [];

write(s, "S", "char");   % tell the Arduino to begin

while ishandle(fig)
    line = strtrim(readline(s));
    if line == "DONE"
        break;
    end

    vals = sscanf(line, "%f,%f,%f");
    if numel(vals) ~= 3
        continue;     % ignore malformed lines
    end

    times(end+1)     = vals(1) / 1000;   %#ok<SAGROW>
    buttons(end+1)   = vals(2);          %#ok<SAGROW>
    reactions(end+1) = vals(3);          %#ok<SAGROW>

    set(h, "XData", times, "YData", reactions);
    drawnow;
end

if ~isempty(reactions)
    title(sprintf("Athlete Reaction Time  |  mean = %.1f ms, best = %.1f ms", ...
        mean(reactions), min(reactions)));

    % Save results
    T = table(times', buttons', reactions', ...
        "VariableNames", ["Time_s", "Button", "Reaction_ms"]);
    writetable(T, "reaction_results.csv");
end

clear s;   % release the serial port
