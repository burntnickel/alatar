function [y] = downsample(x, k);

[r0, c0] = size(x);

r1 = ceil(r0 / k);
c1 = ceil(c0 / k);

y = zeros(r1, c1);

for rr = 1:r1
  for cc = 1:c1
    rv0 = (rr - 1) * k + 1;
    rv1 = min(rv0 + k - 1, r0);
    cv0 = (cc - 1) * k + 1;
    cv1 = min(cv0 + k - 1, c0);
    y(rr, cc) = mean(mean(x(rv0:rv1, cv0:cv1)));
  endfor
endfor

endfunction

