function alignedSignalsOut = alignOiData(lineIn)

alignedSignalsOut = zeros(size(lineIn));
L = size(lineIn,2);

line0 = squeeze(lineIn(1,:));
alignedSignalsOut(1,:) = line0;

for ii = 2:1:size(lineIn,1)
    
    
    D = finddelay(line0,squeeze(lineIn(ii,:)),300);
    
    if D==0
        alignedSignalsOut(ii,:) = lineIn(ii,:);
        
    elseif D>0
        alignedSignalsOut(ii,:) = [squeeze(lineIn(ii,D+1:L)) zeros(1,D)];
    else
        D = abs(D);
        alignedSignalsOut(ii,:) = [zeros(1,D) squeeze(lineIn(ii,1:L-D))];
    end
end