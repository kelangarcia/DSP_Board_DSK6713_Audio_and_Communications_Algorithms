function [] = play(x, rate);

% [] = play(w, rate);
%
% Plays waveform w at rate samples per second.

r = size(x,1);
c = size(x,2);

if (c<=2),
  x = x.';
  a = r; r = c; c = a;
end

if (r == 1),
  x = [1;1]*x;
end

xi = int16(x*32768);

audio_play(xi, rate);

 