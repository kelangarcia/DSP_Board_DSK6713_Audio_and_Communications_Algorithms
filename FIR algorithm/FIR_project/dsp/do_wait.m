function [junk] = do_wait(val, scaling);

if (nargin < 2),
  scaling=1;
end

global do_wait_last_time;
global do_wait_last_clock;

cl = clock;
time1 = cl(4)*3600 + cl(5)*60 + cl(6);
if isempty(do_wait_last_time),
  % On first call just get last glock
  do_wait_last_time = time1;
  do_wait_last_clock = val;
elseif (val < do_wait_last_clock),
  % We restarted.
  do_wait_last_time = time1;
  do_wait_last_clock = val;
else
  % Clock is ahead, wait until time lines up
  while (time1 < do_wait_last_time + scaling*(val-do_wait_last_clock)),
    cl = clock;
    time1 = cl(4)*3600 + cl(5)*60 + cl(6);
  end
  do_wait_last_clock = val;
  do_wait_last_time = time1;
end

junk = 0;


