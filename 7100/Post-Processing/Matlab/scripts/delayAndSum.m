function lineOut = delayAndSum(dataIn,channel,c,fs_Hz,pitch_mm)

dt = 1/fs_Hz;
dz = c*dt;
L = size(dataIn,2);
N = size(dataIn,1);
z = 0:dz:(L-1)*dz;

elements = abs([1:N] - channel);

halfWinSize = max(channel,N-channel);

win = blackmanharris(2*halfWinSize+1);

win = win(halfWinSize +2 - channel:end);
win = win(1:16);

x = (pitch_mm/1000).*elements; %distance from centerline along transducer face

[X,Z] = meshgrid(x,z);

dr = sqrt(X.^2 + Z.^2);

dSamples = round((dr./c)./dt)+1;
dSamples = min(dSamples, L);

for zz=1:1:length(z) 
    theseDelays = dSamples(zz,:);
  
    thisSignal = 0;
    for iChan = 1:N
        thisDelay = theseDelays(iChan);
        thisWindow = win(iChan);
        thisSignal = thisSignal + thisWindow.*dataIn(iChan,thisDelay);
    end
    lineOut(zz) = thisSignal;
end
    
    
