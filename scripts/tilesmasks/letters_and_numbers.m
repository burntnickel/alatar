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

function [tileCell, nameCell, constCell, seqVec] = letters_and_numbers(xSize)

oversample = 2;
xSize = xSize * oversample;

load_path = '../fonts';
base_name = 'main_font_raw_';

listing = dir(fullfile(load_path, [base_name '*.png']));

N = length(listing);

tileCell = cell(1, N);
nameCell = cell(1, N);
constCell = cell(1, N);
seqVec = zeros(1, N);


for n = 1:N
  filename = listing(n).name;
  [~, name, ~] = fileparts(filename);
  [seq, tf] = str2num(name((end - 2):end));

  if ~tf
    error('Error converting number from ', filename)
  endif

  tmp = zeros(xSize, xSize, 'uint8');

  A = imread(fullfile(load_path, filename));

  [rr, cc] = size(A);

  rPad = round((xSize - rr) / 2);
  cPad = round((xSize - cc) / 2);

  tmp(rPad - 1 + (1:rr), cPad - 1 + (1:cc)) = A;

  tmp(1:4, :) = 255;
  tmp((end-3):end, :) = 255;
  tmp(:, 1:4) = 255;
  tmp(:, (end-3):end) = 255;

  a = uint8(downsample(double(tmp), oversample));

  tileCell{n}.gray = uint8((a > 0) * 255);
  tileCell{n}.alpha = a;
  nameCell{n} = [];
  constCell{n} = [];
  seqVec(n) = seq;
endfor

endfunction

