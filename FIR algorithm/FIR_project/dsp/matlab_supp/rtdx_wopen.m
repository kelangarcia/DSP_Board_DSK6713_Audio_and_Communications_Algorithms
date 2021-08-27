function [h] = rtdx_wopen(name);

% [h] = rtdx_wopen(name);
%
% Opens an RTDX channel for writing to the target board.
%
% Inputs:
%	name	Name of the write channel
% Outputs:
%	h	Handle to the write channel

h = rtdx(1, name);
