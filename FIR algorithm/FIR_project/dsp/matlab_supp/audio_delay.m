function [d] = audio_delay(ops);

% [d] = audio_delay(ops);
%
% Estimates the bulk group delay of a system.
%
% Inputs:
%	ops	Options
%		.dmax	Maximum delay to consider
%		.Tn	Noise pulse width
%		.rate	Sample rate to use
% Output:
%	d	Estimated delay

scale = 16384;

% Check inputs
if ~isfield(ops, 'dmax'),
  ops.dmax = 1;
end

if ~isfield(ops, 'Tn'),
  ops.Tn = 0.1;
end

if ~isfield(ops, 'rate'),
  ops.rate = 44100;
end

dmax = ops.dmax;
Tn = ops.Tn;
rate = ops.rate;

% Create a noise pulse
Nn = floor(Tn*rate);
xn = randn(1, Nn)/sqrt(3);

Nmax = floor(dmax*rate);
z = zeros(1, Nmax);
x = [z xn z];
w = [z ones(1,Nn) z];

% Play and record
x_in_ = [1;1]*x;
xi = int16(x_in_*scale);
[y_out_] = audio_recplay(xi, rate);
y = double(y_out_(1,:))/scale;

% Find the shift with max correlation using FFT
corr = ifft(fft(w).*fft(abs(y)));

[val idx] = max(abs(corr));
idx = idx-1;
if (idx >= floor(length(x)/2)),
  idx = idx - (length(x)-1);
end

d = idx/rate;

