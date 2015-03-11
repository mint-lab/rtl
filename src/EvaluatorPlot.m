function [result] = EvaluatorPlot(filename, varIndex, varName, algoName, algoShape)

if nargin < 3
    varName = 'Variable';
end
if nargin < 4
    algoName = {};
end

% Load the evaluation record
% - Format: dataNum, noiseLevel, inlierRate, expTrial, algoIndex, compTime, NSSE, TP, FP, FN, (precision), (recall)
index.dataNum    = 1;
index.noiseLevel = 2;
index.inlierRate = 3;
index.expTrial   = 4;
index.algoIndex  = 5;
index.compTime   = 6;
index.NSSE       = 7;
index.TP         = 8;
index.FP         = 9;
index.FN         = 10;
index.precision  = 11;
index.recall     = 12;
record = load(filename);
record(:,index.precision) = record(:,index.TP) ./ (record(:,index.TP) + record(:,index.FP)); % precision
record(:,index.recall)    = record(:,index.TP) ./ (record(:,index.TP) + record(:,index.FN)); % recall

varValue = unique(record(:,varIndex));
algoID = unique(record(:,index.algoIndex));
if nargin < 5
    algoShape = rand(length(algoID), 3);
end

% Calculate representative values
criteria = { 'Computing Time [msec]', 'NSSE', 'Precision', 'Recall' };
result{1} = zeros(length(algoID), length(varValue));
result{2} = zeros(length(algoID), length(varValue));
result{3} = zeros(length(algoID), length(varValue));
result{4} = zeros(length(algoID), length(varValue));
for i = 1:length(algoID)
    for j = 1:length(varValue)
        mask = record(:,index.algoIndex) == algoID(i) & record(:,varIndex) == varValue(j);
        result{1}(i,j) = mean(record(mask,index.compTime)) * 1000; % [sec] to [msec]
        result{2}(i,j) = mean(record(mask,index.NSSE));
        result{3}(i,j) = mean(record(mask,index.precision));
        result{4}(i,j) = mean(record(mask,index.recall));
    end
end

% Draw graphs
for cr = 1:length(criteria)
    figure('Color', [1, 1, 1]);
    hold on;
        for i = 1:length(algoID)
            if nargin < 5
                plot(varValue, result{cr}(i,:), 'Color', algoShape(i,:), 'LineWidth', 2);
            else
                plot(varValue, result{cr}(i,:), algoShape{i}, 'LineWidth', 2);
            end
        end
        if cr == 2
            axis([min(varValue), max(varValue), 0.8, 1.2]);
        elseif cr == 3
            axis([min(varValue), max(varValue), 0, 1]);
        elseif cr == 4
            axis([min(varValue), max(varValue), 0, 1]);
        end
        box on;
        grid on;
        xlabel(varName);
        ylabel(criteria{cr});
        legend(algoName);
    hold off;
end
