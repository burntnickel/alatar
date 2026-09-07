function [a] = diffuse(nVec, lVec, vVec)

% nVec - Normal vector (rr x 3) or (rr x cc x 3)
% lVec - Light vector (1 x 3)
% vVec - Camera vector (1 x 3)
% It is assumed all input vectors are normalized

s = size(nVec);

if length(s) == 2
  a = zeros(s(1), 1);

  for rr = 1:s(1)
    a(rr) = sum(nVec(rr, :) .* lVec);
  endfor
else
  a = zeros(s(1:2));

  for rr = 1:s(1)
    for cc = 1:s(2)
      a(rr, cc) = sum(nVec(rr, cc, :) .* lVec);
    endfor
  endfor
end

endfunction

