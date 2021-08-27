function [h] = rtdx_ropen(name);

% [h] = rtdx_ropen(name);
%
% Opens an RTDX channel for reading from the target board.
%
% Inputs:
%	name	Name of the read channel
% Outputs:
%	h	Handle to the read channel

h = rtdx(10, name);
