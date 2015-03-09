function [] = LinePlot(models, data, paramShape, paramLegend)

figure('Color', [1, 1, 1]);
hold on;
    % Draw models
    minData = min(data);
    maxData = max(data);
    for i = 1:size(models,1)

        if models(i,2) == 0
            lineX = [-models(i,3)/models(i,1), -models(i,3)/models(i,1)];
            lineY = [minData(2), maxData(2)];
        else
            lineX = [minData(1), maxData(1)];
            lineY = -(models(i,1) * lineX + models(i,3)) ./ models(i,2);
        end
        if nargin >= 3
            plot(lineX, lineY, paramShape{i}, 'LineWidth', 2);
        else
            plot(lineX, lineY, 'Color', rand(1,3), 'LineWidth', 2);
        end
    end

    % Draw data
    plot(data(:,1), data(:,2), 'r.');

    % Draw misc.
    axis([min(data(:,1)) max(data(:,1)) min(data(:,2)) max(data(:,2))]);
    axis equal;
    box on;
    grid on;
    xlabel('X');
    ylabel('Y');
    if nargin >=4
        legend(paramLegend);
    end
hold off;
