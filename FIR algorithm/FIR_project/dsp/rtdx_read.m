function [data] = rtdx_read(h, size, type, timeout);

% [data] = rtdx_read(h, size, type, timeout);
%
% Reads data from an RTDX channel.
%
% Inputs:
%	h	Handle
%	size	Number of elements
%	type	Single element of class wanted. (e.g. int32(1))
%	timeout	Time to wait for completion (ms)

cl = rtdx(100, type);
data = rtdx(11, h, cl, size, timeout);


