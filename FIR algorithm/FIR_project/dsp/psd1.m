function [Y, f] = psd(y, N);

% [Y, f] = psd(y, N);
%
% Computes a power spectral density of the signal y, assuming a correlation
% window size of N.

y = y(:).';
M = length(y);

win = .5*(1 - cos(2*pi*[1:N]/(N+1)));
Y = zeros(1, N);
for ii=1:M-N,
  Y1 = fft(win.*y(ii:ii+N-1))/N;
  Y = Y + abs(Y1).^2;
end
Y = Y/(M-N);

f = [0:N-1]/N;
