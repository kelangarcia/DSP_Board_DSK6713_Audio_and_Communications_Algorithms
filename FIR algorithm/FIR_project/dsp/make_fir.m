% make_fir.m
%	Generates the coefficients for an RC filter.

% Generate filter coefficients
p.beta = 0.5;
p.fs = 0.333;	% Make stop freq at 0.25 = 2000Hz
p.root = 0;	% 0=rc 1=root rc
M = 128;
[h f H Hi,wa] = win_method('rc_filt', p, 0.5, 1, M, 0);

% For this lab, make filter have unit gain in passband
scale = abs(H(1));
h = h/scale;
H = H/scale;
Hi = Hi/scale;

% Write the data out to a file that can be used in C code
write_real_array('rc1_taps', 'rc1_taps', 'float', h);

% Write the filter response to a file so it's actual
% response can be tested
save rc1_taps.mat h Hi H f