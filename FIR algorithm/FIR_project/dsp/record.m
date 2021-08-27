function [d] = record(N, rate, stereo);

% [d] = record(N, rate, stereo);
%
% Records from the audio channel and returns real data.
%
% Inputs:
%	N	Number of samples
%	rate	Sample rate
%	stereo	0=mono record, 1=stereo record
%
% Output:
%	d	Real data

if (nargin < 3),
  stereo = 0;
end

di = audio_record(N, rate);

if ~stereo,
  di = di(1,:);
end

d = double(di)/32768;
