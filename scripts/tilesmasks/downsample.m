% Copyright 2026 Jude Giampaolo
%
% This file is part of Alatar.
%
% Alatar is free software: you can redistribute it and/or modify it under the
% terms of the GNU General Public License as published by the Free Software Foundation,
% either version 3 of the License, or (at your option) any later version.
%
% Alatar is distributed in the hope that it will be useful, but WITHOUT ANY
% WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
% PARTICULAR PURPOSE. See the GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License along with Alatar.
% If not, see <https://www.gnu.org/licenses/>. 

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

