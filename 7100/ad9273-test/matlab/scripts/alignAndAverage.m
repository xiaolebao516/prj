function aveDataOut = alignAndAverage(frameData)

%aligns data from multiple shots in the time domain and averages them
%together

nShots = size(frameData,3);
nAdcChannels = 8; %hard-coded as we always use all 8 ADC channels
adcStartInds = [1 9];

for adc = 1:2 %each ADC has its own unique personality
    thisFrameData = frameData(adcStartInds(adc):adcStartInds(adc)+7,:,:);
    
    chanData1 = squeeze(thisFrameData(1,:,:)); %data from first ADC channel
    alignedData = squeeze(thisFrameData(:,:,1)); %data from first shot
    L = size(thisFrameData,2);
    
    for iShot = 1:(nShots-1)
        D = finddelay(alignedData(1,:),chanData1(:,iShot+1),200);
        
        if D==0
            alignedData = alignedData + squeeze(thisFrameData(:,:,iShot+1)); %signals are aligned
        elseif D>0
            alignedData = alignedData + [squeeze(thisFrameData(:,D+1:L,iShot+1)) zeros(nAdcChannels,D)];
        else
            D = abs(D);
            alignedData = alignedData + [zeros(nAdcChannels,D) squeeze(thisFrameData(:,1:L-D,iShot+1))];
        end
        
    end
    
    adcData(adc,:,:) = alignedData./nShots;
    
end

D = finddelay(squeeze(adcData(1,8,:)),squeeze(adcData(2,1,:)),1000);

if D==0
    aveDataOut = [squeeze(adcData(1,:,:));squeeze(adcData(2,:,:))];
    
elseif D>0
    aveDataOut = [squeeze(adcData(1,:,:)); ...
        [squeeze(adcData(2,:,D+1:L)) zeros(nAdcChannels,D)]];
else
    D = abs(D);
    aveDataOut = [squeeze(adcData(1,:,:)); ...
        [zeros(nAdcChannels,D) squeeze(adcData(2,:,1:L-D))]];
end




