function [H, f, d] = audio_xfer(ops);

% [H, f, d] = audio_xfer(ops);
%
% Computes the input/output transfer function of a system
% using audio hardware.
%
% Inputs:
%	ops	Structure of options
%		.rate		Sample rate
%		.f0		Start frequency
%		.f1		Stop frequency
%		.N		Number of frequency points
%		.Tpad		Length of zero padding
%		.Ton		Time of the transition window
%		.Test		Time of estimation window (per point)
%		.delay		Bulk delay of input to output
%
% Outputs:
%	H	Transfer function in frequency domain
%	f	Frequency points

scale = 16384;

% Set default options if necessary
if ~isfield(ops, 'rate'),
  ops.rate = 44100;
end

if ~isfield(ops, 'f0'),
  ops.f0 = 20;
end

if ~isfield(ops, 'f1'),
  ops.f1 = 20000;
end

if ~isfield(ops, 'N'),
  ops.N = 32;
end

if ~isfield(ops, 'Tpad'),
  ops.Tpad = 0.1;
end

if ~isfield(ops, 'Ton'),
  ops.Ton = 0.01;
end

if ~isfield(ops, 'delay'),
  ops.delay = 0;
end

% Get options out of structure
rate = ops.rate;
f0 = ops.f0;
f1 = ops.f1;
N = ops.N;
Tpad = ops.Tpad;
Ton = ops.Ton;
Test = ops.Test;
delay = ops.delay;

% Compute the total length of the chirp
Tall = Test*N + Ton + 2*Tpad;
if (Tall > 30),
  error('Total sweep time > 30s.  Do fewer points or shorter estimation window.');
end

% Get frequencies
f = [0:N-1]/(N-1)*(f1-f0) + f0;

% Get length of things in samples
Non = floor(Ton*rate);
Nest = floor(Test*rate);
Npad = floor(Tpad*rate);

% Get the on/off window.  Use raised cosine
w1 = (1-cos(pi*[0:Non-1]/(Non-1)))/2;
w_on = [w1 ones(1,Nest) w1(end:-1:1)];

% Build stepped tone waveform
y1 = zeros(1, Non + (Nest + Nest)*N);
n1_ = zeros(1, N);
n2_ = zeros(1, N);
for n=1:N,
  % Get starting/ending samples for this tone
  n1 = (n-1)*(Nest+Non)+1;
  n2 = n1+Nest+2*Non-1;
  
  n1_(n) = n1;
  n2_(n) = n2;
  
  t = ([n1:n2]-1)/rate;
  
  % Compute waveform for tone
  x1 = cos(2*pi*f(n)*t).*w_on;
  
  % store it
  y1(n1:n2) = y1(n1:n2)+x1;
end

% Add padding
z = zeros(1, Npad);
y_in = [z y1 z];

% Play and record
y_in_ = [1;1]*y_in;
yi = int16(y_in_*scale);
[y_out_] = audio_recplay(yi, rate);
y_out = double(y_out_(1,:))/scale;

% Cut out proper part using known bulk delay
Ndelay = floor(delay*rate);
idx = [Npad+Ndelay:Npad+Ndelay+length(y1)];

% Cut off padding
y = y_out(idx); %y_out(Npad+1:Npad+length(y1));

% For estimation, use a hamming window half the size of the waveform
Nest2 = floor(Nest/2);
w_est = 0.54 + 0.46*cos(2*pi*[0:Nest2-1]/(Nest2-1));

% Estimate at each tone
H = zeros(1, N);
for n=1:N,
  n1 = n1_(n);
  n2 = n2_(n);
  % y1 = y(n1:n2);
  
  % Cut out the middle part
  n1p = n1+Non+floor(Nest/4);
  n2p = n1p+Nest2-1;
  idx = [n1p:n2p];
  
  % Get the carrier for this tone
  t = (idx-1)/rate;
  x1 = (cos(2*pi*f(n)*t) + j*sin(2*pi*f(n)*t));
  
  % Estimate magnitude and phase
  y2 = y(idx);
  H(n) = sum(y2.*conj(x1).*w_est)/sum(w_est);
end

if (nargout >= 3),
  d.y = y;
  d.x = y1;
end

