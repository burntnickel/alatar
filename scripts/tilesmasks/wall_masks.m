function [masksCell, namesCell, constCell] = wall_masks(xSize)

ySize = xSize;
hSize = xSize / 2;
qSize = hSize / 2;

fadeWidth = xSize / 4;

masksCell = cell(1,0);
namesCell = cell(1,0);
constCell = cell(1,0);

index = 1;

%---------------------------------------------------------------------------
% blank
%---------------------------------------------------------------------------
masksCell{index} = zeros(ySize, xSize, 'uint8');
namesCell{index} = 'Blank';
constCell{index} = 'Blank';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (base/000)
%---------------------------------------------------------------------------
kWallFloorBase = index;
masksCell{index} = 255 * ones(ySize, xSize, 'uint8');
namesCell{index} = 'Wall/Floor (base)';
constCell{index} = 'WallBase';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (001)
%---------------------------------------------------------------------------
kWallFloorRight = index;
rightWallFloorFade = ones(ySize, xSize);

for y = 2:ySize
  x0 = xSize - y + 1;
  x1 = min(x0 + fadeWidth, xSize);

  if (x0 >= 1) & (x0 <= xSize) & (x1 >= 1) & (x1 <= xSize)
    n = x1 - x0 + 1;
    rightWallFloorFade(y, x0:x1) = linspace(1, 0, n);
    rightWallFloorFade(y, (x1 + 1):end) = 0;
  end
end

tmp = double(masksCell{kWallFloorBase}) .* rightWallFloorFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (001)';
constCell{index} = 'Wall001';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (010)
%---------------------------------------------------------------------------
middleWallFloorFade = ones(ySize, xSize);

y0 = hSize + 1;
y1 = y0 + fadeWidth;
n = y1 - y0 + 1;
w = linspace(1, 0, n);

middleWallFloorFade(y0:y1, :) = repmat(w.', [1 xSize]);
middleWallFloorFade((y1 + 1):end, :) = 0;

tmp = double(masksCell{kWallFloorBase}) .* middleWallFloorFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (010)';
constCell{index} = 'Wall010';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (011)
%---------------------------------------------------------------------------
tmp = double(masksCell{kWallFloorBase}) .* min(middleWallFloorFade, rightWallFloorFade);
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (011)';
constCell{index} = 'Wall011';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (100)
%---------------------------------------------------------------------------
leftWallFloorFade = fliplr(rightWallFloorFade);
tmp = double(masksCell{kWallFloorBase}) .* leftWallFloorFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (100)';
constCell{index} = 'Wall100';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (101 / 111)
%---------------------------------------------------------------------------
tmp = double(masksCell{kWallFloorBase}) .* min(leftWallFloorFade, rightWallFloorFade);
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (101/111)';
constCell{index} = 'Wall101';
index = index + 1;

%---------------------------------------------------------------------------
% wall/floor (110)
%---------------------------------------------------------------------------
tmp = double(masksCell{kWallFloorBase}) .* min(leftWallFloorFade, middleWallFloorFade);
masksCell{index} = uint8(tmp);
namesCell{index} = 'Wall/Floor (110)';
constCell{index} = 'Wall110';
index = index + 1;

%---------------------------------------------------------------------------
% steep stairs left (base)
%---------------------------------------------------------------------------
kSteepStairsLeftBase = index;
masksCell{index} = 255 * ones(xSize, ySize, 'uint8');
masksCell{index}(1:hSize, (hSize + 1):end) = 0;
masksCell{index}(1:qSize, (qSize + 1):hSize) = 0;
masksCell{index}((hSize + 1):(hSize + qSize), (hSize + qSize + 1):end) = 0;
namesCell{index} = 'Steep Stairs Left (base)';
constCell{index} = 'SteepStairsLeftBase';
index = index + 1;

%---------------------------------------------------------------------------
% steep stairs left (1)
%---------------------------------------------------------------------------
leftSteepStairsFade = ones(ySize, xSize);

w = linspace(1, 0, fadeWidth);

for d = 1:fadeWidth
  for x = 1:(xSize - d)
    y = x + d;
    leftSteepStairsFade(y, x) = w(d);
  end
end

for x = 1:(xSize - fadeWidth - 1)
  leftSteepStairsFade((fadeWidth + x):end, x) = 0;
end

tmp = double(masksCell{kSteepStairsLeftBase}) .* leftSteepStairsFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Steep Stairs Left (1)';
constCell{index} = 'SteepStairsLeft1';
index = index + 1;

%---------------------------------------------------------------------------
% steep stairs right (base)
%---------------------------------------------------------------------------
kSteepStairsRightBase = index;
masksCell{index} = fliplr(masksCell{kSteepStairsLeftBase});
namesCell{index} = 'Steep Stairs Right (base)';
constCell{index} = 'SteepStairsRightBase';
index = index + 1;

%---------------------------------------------------------------------------
% steep stairs right (1)
%---------------------------------------------------------------------------
rightSteepStairsFade = fliplr(leftSteepStairsFade);
tmp = double(masksCell{kSteepStairsRightBase}) .* rightSteepStairsFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Steep Stairs Right (1)';
constCell{index} = 'SteepStairsRight1';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs left (left part) (base)
%---------------------------------------------------------------------------
kShallowStairsLeftLeft = index;
tmp = kron(masksCell{kSteepStairsLeftBase}, [1 1]);
masksCell{index} = tmp(:, 1:xSize);
namesCell{index} = 'Shallow Stairs Left (left part) (base)';
constCell{index} = 'ShallowStairsLeftLeftPartBase';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs left (left part) (1)
%---------------------------------------------------------------------------
kShallowStairsLeftLeftFade = index;
tmp = kron(leftSteepStairsFade, [1 1]);
masksCell{index} = uint8(double(masksCell{kShallowStairsLeftLeft}) .* tmp(:, 1:xSize));
namesCell{index} = 'Shallow Stairs Left (left part) (1)';
constCell{index} = 'ShallowStairsLeftLeftPart1';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs left (right part) (base)
%---------------------------------------------------------------------------
kShallowStairsLeftRight = index;
tmp = kron(masksCell{kSteepStairsLeftBase}, [1 1]);
masksCell{index} = tmp(:, (xSize + 1):end);
namesCell{index} = 'Shallow Stairs Left (right part) (base)';
constCell{index} = 'ShallowStairsLeftRightPartBase';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs left (right part) (1)
%---------------------------------------------------------------------------
kShallowStairsLeftRightFade = index;
tmp = kron(leftSteepStairsFade, [1 1]);
masksCell{index} = uint8(double(masksCell{kShallowStairsLeftRight}) .* tmp(:, (xSize + 1):end));
namesCell{index} = 'Shallow Stairs Left (right part) (1)';
constCell{index} = 'ShallowStairsLeftRightPart1';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs right (left part) (base)
%---------------------------------------------------------------------------
tmp = kron(masksCell{kSteepStairsRightBase}, [1 1]);
masksCell{index} = tmp(:, 1:xSize);
namesCell{index} = 'Shallow Stairs Right (left part) (base)';
constCell{index} = 'ShallowStairsRightLeftPartBase';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs right (left part) (1)
%---------------------------------------------------------------------------
masksCell{index} = fliplr(masksCell{kShallowStairsLeftRightFade});
namesCell{index} = 'Shallow Stairs Right (left part) (1)';
constCell{index} = 'ShallowStairsRightLeftPart1';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs right (right part) (base)
%---------------------------------------------------------------------------
tmp = kron(masksCell{kSteepStairsRightBase}, [1 1]);
masksCell{index} = tmp(:, (xSize + 1):end);
namesCell{index} = 'Shallow Stairs Right (right part) (base)';
constCell{index} = 'ShallowStairsRightRightPartBase';
index = index + 1;

%---------------------------------------------------------------------------
% shallow stairs right (right part) (1)
%---------------------------------------------------------------------------
masksCell{index} = fliplr(masksCell{kShallowStairsLeftLeftFade});
namesCell{index} = 'Shallow Stairs Right (right part) (1)';
constCell{index} = 'ShallowStairsRightRightPart1';
index = index + 1;

%---------------------------------------------------------------------------
% slide left (base)
%---------------------------------------------------------------------------
kSlideLeftBase = index;
masksCell{index} = zeros(ySize, xSize, 'uint8');

for y = 1:ySize
  masksCell{index}(y, 1:y) = 255;
end

namesCell{index} = 'Side Left (base)';
constCell{index} = 'SlideLeftBase';
index = index + 1;

%---------------------------------------------------------------------------
% slide left (1)
%---------------------------------------------------------------------------
tmp = double(masksCell{kSlideLeftBase}) .* leftSteepStairsFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Side Left (1)';
constCell{index} = 'SlideLeft1';
index = index + 1;

%---------------------------------------------------------------------------
% slide right (base)
%---------------------------------------------------------------------------
kSlideRightBase = index;
masksCell{index} = fliplr(masksCell{kSlideLeftBase});
namesCell{index} = 'Side Right (base)';
constCell{index} = 'SlideRightBase';
index = index + 1;

%---------------------------------------------------------------------------
% slide right (1)
%---------------------------------------------------------------------------
tmp = double(masksCell{kSlideRightBase}) .* rightSteepStairsFade;
masksCell{index} = uint8(tmp);
namesCell{index} = 'Side Right (1)';
constCell{index} = 'SlideRight1';
index = index + 1;

endfunction

