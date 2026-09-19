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

close all; clear variables; clc

xSize = 64;
pngFilename = 'tiles_and_masks.png';
txtFilename = 'maskstiles.txt';
headerFilename = 'maskstiles.h';

ySize = xSize;

masksTilesCellRaw = cell(1, 0);
namesCellRaw = cell(1, 0);
constCellRaw = cell(1, 0);
seqVec = zeros(1, 0);

index = 0;

%---------------------------------------------------------------------------
% Wall/Floor Masks
%---------------------------------------------------------------------------
[masksTilesCell0, namesCell0, constEndCell, seqVec0] = wall_masks(xSize);
constBase = 'kMaskTile';

N = length(constEndCell);

masksTilesCellRaw((1:N) + index) = masksTilesCell0;
namesCellRaw((1:N) + index) = namesCell0;

for iN = 1:N
  tmp = sprintf('%s%s', constBase, constEndCell{iN});
  constCellRaw{index + iN} = tmp;
endfor

seqVec((1:N) + index) = seqVec0;

index = index + N;

%---------------------------------------------------------------------------
% Ladder Tiles
%---------------------------------------------------------------------------
[masksTilesCell0, namesCell0, constEndCell, seqVec0] = ladders(xSize);
constBase = 'kLadderTile';

N = length(constEndCell);

masksTilesCellRaw((1:N) + index) = masksTilesCell0;
namesCellRaw((1:N) + index) = namesCell0;

for iN = 1:N
  tmp = sprintf('%s%s', constBase, constEndCell{iN});
  constCellRaw{index + iN} = tmp;
endfor

seqVec((1:N) + index) = seqVec0;

index = index + N;

%---------------------------------------------------------------------------
% Keyhold and Arrows
%---------------------------------------------------------------------------
[masksTilesCell0, namesCell0, constEndCell, seqVec0] = keyhole_and_arrows(xSize);
constBase = 'kMiscTile';

N = length(constEndCell);

masksTilesCellRaw((1:N) + index) = masksTilesCell0;
namesCellRaw((1:N) + index) = namesCell0;

for iN = 1:N
  tmp = sprintf('%s%s', constBase, constEndCell{iN});
  constCellRaw{index + iN} = tmp;
endfor

seqVec((1:N) + index) = seqVec0;

index = index + N;

%---------------------------------------------------------------------------
% Letters and Numbers
% No named constants for these
%---------------------------------------------------------------------------
[masksTilesCell0, ~, ~, seqVec0] = letters_and_numbers(xSize);

N = length(seqVec0);

masksTilesCellRaw((1:N) + index) = masksTilesCell0;

for iN = 1:N
  namesCellRaw{index + iN} = [];
  constCellRaw{index + iN} = [];
endfor

seqVec((1:N) + index) = seqVec0;

index = index + N;


%---------------------------------------------------------------------------
% Repack tiles here
%---------------------------------------------------------------------------
nRawTiles = length(masksTilesCellRaw);

masksTilesCell = cell(1, 0);
namesCell = cell(1, 0);
constCell = cell(1, 0);

newSeq = 128;

for iTile = 1:nRawTiles
  if (seqVec(iTile) == -1)
    idx = newSeq;
    newSeq = newSeq + 1;
  else
    idx = seqVec(iTile) + 1;
  end

  masksTilesCell(idx) = masksTilesCellRaw(iTile);
  namesCell(idx) = namesCellRaw(iTile);
  constCell(idx) = constCellRaw(iTile);
endfor

%---------------------------------------------------------------------------
% Output PNG file
%---------------------------------------------------------------------------
nMasksTiles = length(masksTilesCell);

data = zeros(nMasksTiles * ySize, xSize, 3, 'uint8');
alpha = zeros(nMasksTiles * ySize, xSize, 'uint8');

[xx, yy] = meshgrid(1:xSize, 1:ySize);
dummyTile = 255 * iseven((mod(xx - 1, 32) > 15) + (mod(yy - 1, 32) > 15));

for iMaskTile = 1:nMasksTiles
  s0 = 1 + (iMaskTile - 1) * ySize;
  s1 = s0 + ySize - 1;

  if isempty(masksTilesCell{iMaskTile})
    for iChan = 1:3
      data(s0:s1, :, iChan) = dummyTile;
      alpha(s0:s1, :) = 255 * ones(xSize, ySize);
    endfor
  else
    for iChan = 1:3
      data(s0:s1, :, iChan) = masksTilesCell{iMaskTile}.gray;
    endfor

    alpha(s0:s1, :) = masksTilesCell{iMaskTile}.alpha;

    fprintf(1,'%d\t%s\n', iMaskTile - 1, namesCell{iMaskTile});
  end
end

imwrite(data, pngFilename, 'Alpha', alpha)
imwrite(data, 'data.png')
imwrite(alpha, 'alpha.png')

%---------------------------------------------------------------------------
% Output desription file
%---------------------------------------------------------------------------

fid = fopen(txtFilename, 'w');

for iMaskTile = 1:nMasksTiles
  if ~isempty(namesCell{iMaskTile})
    fprintf(fid, '%d\t%s\n', iMaskTile - 1, namesCell{iMaskTile});
  endif
endfor

fclose(fid);

%---------------------------------------------------------------------------
% Output header file
%---------------------------------------------------------------------------

fid = fopen(headerFilename, 'w');

fprintf(fid, '#ifndef H_ALATAR_MASKSTILES\n');
fprintf(fid, '#define H_ALATAR_MASKSTILES\n\n');

fprintf(fid, '// Automatically generated by script\n');
fprintf(fid, '// May have been reformatted later\n\n');

fprintf(fid, 'namespace alatar_updated {\n\n');

fprintf(fid, 'enum MaskTileIDs {\n');

for iMaskTile = 1:nMasksTiles
  if ~isempty(constCell{iMaskTile})
    fprintf(fid, '%s = %d', constCell{iMaskTile}, iMaskTile - 1);

    if (iMaskTile == nMasksTiles)
      fprintf(fid, ' // %s\n', namesCell{iMaskTile});
    else
      fprintf(fid, ', // %s\n', namesCell{iMaskTile});
    endif
  endif
endfor

fprintf(fid, '};\n\n');

fprintf(fid, '}  // namespace alatar_updated\n\n');

fprintf(fid, '#endif\n');

fclose(fid);

