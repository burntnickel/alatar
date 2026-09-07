function [a] = ambient(nVec)

% nVec - Normal vector (rr x 3) or (rr x cc x 3)
% It is assumed all input vectors are normalized

s = size(nVec);

if length(s) == 2
  a = ones(s(1), 1);
else
  a = ones(s(1:2));
end

endfunction

