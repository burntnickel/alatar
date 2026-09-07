function [a] = specular(nVec, lVec, vVec, k)

% nVec - Normal vector (rr x 3) or (rr x cc x 3)
% lVec - Light vector (1 x 3)
% vVec - Camera vector (1 x 3)
% k - Shininess exponent
% It is assumed all input vectors are normalized

s = size(nVec);

if length(s) == 2
  a = zeros(s(1), 1);

  for rr = 1:s(1)
    rVec = 2 * sum(nVec(rr, :) .* lVec) * nVec(rr, :) - lVec;
%   rVec = rVec / norm(rVec);
    dp = max(sum(rVec .* vVec), 0);
    a(rr) = dp ^ k;
  endfor
else
  a = zeros(s(1:2));

  for rr = 1:s(1)
    for cc = 1:s(2)
      rVec = 2 * sum(nVec(rr, cc, :) .* lVec) * nVec(rr, cc, :) - lVec;
 %     rVec = rVec / norm(rVec);
    dp = max(sum(rVec .* vVec), 0);
    a(rr, cc) = dp ^ k;    endfor
  endfor
end

endfunction

