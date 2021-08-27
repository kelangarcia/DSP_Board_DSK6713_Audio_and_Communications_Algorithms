function [symb] = ascii_to_symb(bps, ascii);

symb_per_char = 8/bps;
symb1 = zeros(symb_per_char, length(ascii));
bytes = uint8(ascii);
for ii=1:symb_per_char,
  symb2 = bitand(bytes, 2^bps-1);
  symb1(ii,:) = symb2;
  bytes = bitshift(bytes, -2);
end
symb = symb1(:);
  
% [symb] = ascii_to_qpsk_frame(sync, N, ascii);

