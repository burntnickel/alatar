function [tileCell, nameCell, constCell, seqVec] = ladders(xSize)

oversample = 3;

xSize = xSize * oversample;
ySize = xSize;

r = 25 * oversample / xSize;

lVec = [1 -1 2];
lVec = lVec / norm(lVec);

vVec = [0  0  1];

amb = 0.2;
dif = 0.8;
spec = 1.0;
k = 16.0;

tileCell = cell(1, 0);
nameCell = cell(1, 0);
constCell = cell(1,0);
seqVec = zeros(1, 0);

index = 1;

xVec = linspace(-1, 1, xSize).';
yVec = linspace(-1, 1, ySize).';

%---------------------------------------------------------------------------
% blank
%---------------------------------------------------------------------------
%tileCell{index}.gray = zeros(ySize / oversample, xSize / oversample, 'uint8');
%tileCell{index}.alpha = zeros(ySize / oversample, xSize / oversample, 'uint8');
%nameCell{index} = 'Blank';
%constCell{index} = 'LadderBlank';
%index = index + 1;
%
%---------------------------------------------------------------------------
% vertical only
%---------------------------------------------------------------------------
zVec = sqrt(r^2 - xVec.^2);
mask = abs(xVec) > r;
nMat = [xVec, zeros(ySize, 1), zVec] / r;
nMat(mask, :) = 0;
nMat = real(nMat);

tmp = amb * ambient(nMat) ...
  + dif * diffuse(nMat, lVec, vVec) ...
  + spec * specular(nMat, lVec, vVec, k);
tmp(mask) = 0;
tmp = min(tmp, 1);
tmp = repmat(tmp.', [xSize 1]);

tmpD = zVec.';
tmpD(mask) = -Inf;
depthV = repmat(tmpD, [xSize 1]);

rawV = tmp;
tileCell{index}.gray = uint8(downsample(255 * tmp, oversample));
tileCell{index}.alpha = uint8(downsample(255 * double(tmp > 0), oversample));
nameCell{index} = 'Ladder Vertical Only';
constCell{index} = 'Vertical';
seqVec(index) = -1;
index = index + 1;

%---------------------------------------------------------------------------
% horizontal only
%---------------------------------------------------------------------------
zVec = sqrt(r^2 - yVec.^2);
mask = abs(yVec) > r;
nMat = [zeros(ySize, 1), yVec, zVec] / r;
nMat(mask, :) = 0;
nMat = real(nMat);

tmp = amb * ambient(nMat) ...
  + dif * diffuse(nMat, lVec, vVec) ...
  + spec * specular(nMat, lVec, vVec, k);
tmp(mask) = 0;
tmp = min(tmp, 1);

tmp = repmat(tmp, [1 ySize]);

tmpD = zVec;
tmpD(mask) = -Inf;
depthH = repmat(tmpD, [1 ySize]);

rawH = tmp;
tileCell{index}.gray = uint8(downsample(255 * tmp, oversample));
tileCell{index}.alpha = uint8(downsample(255 * double(tmp > 0), oversample));
nameCell{index} = 'Ladder Horizontal Only';
constCell{index} = 'Horizontal';
seqVec(index) = 102;
index = index + 1;

%---------------------------------------------------------------------------
% left combo
%---------------------------------------------------------------------------
tmp = rawV;
dMask = depthH > depthV;
hMask = repmat(xVec.' > 0, [xSize 1]);
mask = dMask & hMask;
tmp(mask) = rawH(mask);

tileCell{index}.gray = uint8(downsample(255 * tmp, oversample));
tileCell{index}.alpha = uint8(downsample(255 * double(tmp > 0), oversample));
nameCell{index} = 'Ladder Combo Left';
constCell{index} = 'ComboLeft';
seqVec(index) = 101;
index = index + 1;

%---------------------------------------------------------------------------
% right combo
%---------------------------------------------------------------------------
tmp = rawV;
dMask = depthH > depthV;
hMask = repmat(xVec.' < 0, [xSize 1]);
mask = dMask & hMask;
tmp(mask) = rawH(mask);

tileCell{index}.gray = uint8(downsample(255 * tmp, oversample));
tileCell{index}.alpha = uint8(downsample(255 * double(tmp > 0), oversample));
nameCell{index} = 'Ladder Combo Left';
constCell{index} = 'ComboRight';
seqVec(index) = 103;
index = index + 1;

endfunction

