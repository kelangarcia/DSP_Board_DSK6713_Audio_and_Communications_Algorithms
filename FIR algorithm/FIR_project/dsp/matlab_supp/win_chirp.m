function [y, Y, fY] = win_chirp(N, f0, f1, win_len, pad);

% [y, Y, fY] = win_chirp(N, f0, f1, win_len, pad);
%
% Creates a simple windowed chirp waveform.  The window is a raised cosine
% shape.  Realize for testing filters that the chirp has a nearly rectangular
% shape centered at f1 with support extending from f0 to f0+2*(f1-f0).
%
% Inputs:
%	N	Number of total samples
%	f0,f1	Start and stop frequency
%	win_len	Length of the on/off window in samples
%	pad	Optional zero padding on two sides
%
% Outputs:
%	y	Waveform	
%	Y	Frequency domain representation of signal
%	

% Number of samples with chirp signal
Nc = N-win_len*2;

% Get the window
% Raised cos window
w1 = (1-cos(pi*[0:win_len-1]/(win_len-1)))/2;

% Gaussian window
%sig = win_len/4;
%w1 = exp(-[0:win_len-1].^2/(2*sig^2)); w1 = w1(end:-1:1);

w = [w1 ones(1,Nc) w1(end:-1:1)];

% Get the frequency at each sample
f = [0:N-1]/(N-1)*(f1-f0) + f0;
n = [0:N-1];
y = cos(2*pi*f.*n).*w + j*sin(2*pi*f.*n);

% Get the frequency domain rep.
if (nargout >= 3),
  y1 = [y zeros(1, length(y))];
  fY = [0:length(y1)-1]/length(y1);
  Y = fft(y1)/length(y1);
end

if (nargin >= 5),
  z = zeros(1, pad);
  y = [z y z];
end

