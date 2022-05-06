window_length = 0.05;
time_step = 0.01;

stream = pcread("../output/scene_events.pcd");
scatter3(stream.Location(:,1), stream.Location(:,2), stream.Location(:,3), 30, stream.Color, '.');

for i = 0 : time_step : stream.YLimits(2)
    pause(0.2);
    axis([-inf inf i i+window_length -inf inf]);
end