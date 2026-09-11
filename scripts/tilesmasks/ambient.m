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

