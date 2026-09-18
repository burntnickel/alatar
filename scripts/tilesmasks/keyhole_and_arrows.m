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

function [tileCell, nameCell, constCell, seqVec] = keyhole_and_arrows(xSize)

oversample = 3;
xSize = xSize * oversample;
ySize = xSize;

ySize = xSize;

aspect = 0.75;

tileCell = cell(1, 0);
nameCell = cell(1, 0);
constCell = cell(1,0);
seqVec = zeros(1, 0);

index = 1;

xVec = linspace(-1, 1, xSize).';
yVec = linspace(-1, 1, ySize).';

%---------------------------------------------------------------------------
% keyhole
%---------------------------------------------------------------------------
[XX, YY] = meshgrid(xVec, yVec);

xc = 0;
yc = -0.425;
r = 0.3;
tTop = yc;
tBottom = 0.825;
tSlope = 80 * oversample;

mask_circle = ((XX - xc).^2 + ((YY - yc) / aspect).^2) > r;

mask_triangle = true(ySize, xSize);

[~, idx0] = min(abs(yVec - tTop));
[~, idx1] = min(abs(yVec - tBottom));

xAcc = 0;

for yy = idx0:idx1
  [~, x0] = min(abs(xVec + xAcc));
  [~, x1] = min(abs(xVec - xAcc));
  mask_triangle(yy, x0:x1) = false;
  xAcc = xAcc + 1 / tSlope;
endfor

tmp = uint8(downsample(double(mask_circle & mask_triangle) * 255, oversample));

tileCell{index}.gray = tmp;
tileCell{index}.alpha = ones(size(tmp)) * 255;
nameCell{index} = 'Keyhole';
constCell{index} = 'Keyhole';
seqVec(index) = 64;
index = index + 1;

%---------------------------------------------------------------------------
% right arrow
%---------------------------------------------------------------------------
kRightArrow = index;
mask = false(ySize, xSize);

yRect = 1/4;
xTri0 = -1/4;
xTri1 = 3/4;
yTri = 3/4;
dy = 0.75;

x0 = 1;
[~, x1] = min(abs(xVec));
[~, y0] = min(abs(yVec + yRect));
[~, y1] = min(abs(yVec - yRect));
mask(y0:y1, x0:x1) = true;

[~, x0] = min(abs(xVec - xTri0));
[~, x1] = min(abs(xVec - xTri1));
[~, y0] = min(abs(yVec + yTri));
[~, y1] = min(abs(yVec - yTri));

for xx = x0:x1
  mask(round(y0):round(y1), xx) = true;
  y0 = y0 + dy;
  y1 = y1 - dy;
endfor

tmp = uint8(downsample(double(mask) * 255, oversample));

tileCell{index}.gray = tmp;
tileCell{index}.alpha = tmp;
nameCell{index} = 'Right Arrow';
constCell{index} = 'RightArrow';
seqVec(index) = 110;
index = index + 1;

%---------------------------------------------------------------------------
% up arrow
%---------------------------------------------------------------------------
kUpArrow = index;

tileCell{index}.gray = rot90(tileCell{kRightArrow}.gray);
tileCell{index}.alpha = rot90(tileCell{kRightArrow}.alpha);
nameCell{index} = 'Up Arrow';
constCell{index} = 'UpArrow';
seqVec(index) = 111;
index = index + 1;

%---------------------------------------------------------------------------
% left arrow
%---------------------------------------------------------------------------
kLeftArrow = index;

tileCell{index}.gray = rot90(tileCell{kUpArrow}.gray);
tileCell{index}.alpha = rot90(tileCell{kUpArrow}.alpha);
nameCell{index} = 'Left Arrow';
constCell{index} = 'LeftArrow';
seqVec(index) = 112;
index = index + 1;

%---------------------------------------------------------------------------
% down arrow
%---------------------------------------------------------------------------
tileCell{index}.gray = rot90(tileCell{kLeftArrow}.gray);
tileCell{index}.alpha = rot90(tileCell{kLeftArrow}.alpha);
nameCell{index} = 'Down Arrow';
constCell{index} = 'DownArrow';
seqVec(index) = 113;
index = index + 1;


endfunction

