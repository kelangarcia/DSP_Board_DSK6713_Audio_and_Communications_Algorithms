function [] = rtdx_write(h, data);

% [] = rtdx_write(h, data);
%
% Writes data to an RTDX channel.
%
% Inputs:
%	h	Handle
%	data	Data to be written

rtdx(2, h, data);



