function [Y, f] = psd(y, N);

% [Y, f] = psd(y, N);
%
% Computes a power spectral density of the signal y, assuming a correlation
% window size of y.

M = length(y);

Y = zeros(1, N);
for ii=1:M-N,
  Y1 = fft(y(ii:ii+N-1));
  Y = Y + abs(Y1).^2;
end
Y = Y/(M-N);

f = [0:N-1]/N;
