clear all
% close all
clc

dists = 4.*[0 1 2 3 4 5 6 7 9 10 11]; %roungh "distance" that the translational stage was at, used for filenames. Note we intentionally skip one where the stage stuttered.
ti=[]; %initialize an image vector
nOverlap = 2;

reAlignData = false;

for xx = 1:1:length(dists) %loop through files
    
    if reAlignData
        load(['phantom_focused_' num2str(dists(xx)) 'mm.mat']);  % The "data" vairable is data(tx_focus_depth, image_line, rx_channel, rx_waveform_data);
        
        % Build vectors for image display
        fs_Hz = fr.shots(1,1).rx.fs.*1e6; %sample rate, Hz
        fs_MHz = fs_Hz/1e6; %sample rate, MHz
        t = 0:1/fs_Hz:(fr.shots(1).rx.N-1)/fs_Hz; %time vector
        z = c.*t./2.*100; %depth axis (cm)
        x = pitch_mm/2:pitch_mm:16.5*pitch_mm; %transverse axis (mm)
        tx_freq_MHz = tx_freq_hz./1e6; %center frequency of hte transmit waveform
    end
    
    % form the image from the raw data
    for iLine = 1:16 %loop through lines. A "line" becomes one vertical line in the image
        
        if reAlignData
            for iDepth = 1:size(data,1) %loop through focusing depths
                
                fd = squeeze(data(iDepth,iLine,:,:)); %Grab the data for this depth and this line
                fd2 = double(reshape(fd,16,[],fr.nShots)); %We took "nShots" measurements each time, we want to average these to improve SNR
                clear fd;
                
                aveData(:,:,iDepth) = alignAndAverage(fd2); %aligns data via cross-correlation to account for variation in the ADC start time relative to the pulser start time
                clear fd2
            end
            
            aveDataLine = alignAndAverage(aveData); %"aveData" has one entry for each tx_focus_depth, we want to align the data in time (ADC-pulser inconsistencies again) for all depths and sum together; the same function works here
            
            save(['Data\prebeamformed_data_' num2str(xx) '_' num2str(iLine) '.mat'], 'aveDataLine', 'fs_MHz','pitch_mm','c','fs_Hz','tx_freq_MHz');
        else
            load(['Data\prebeamformed_data_' num2str(xx) '_' num2str(iLine) '.mat']) %data live: \\Olympus\Projects\1010260-OPEN-IMAGE\Technical Work\Testing\Data\20210131 Phantom
        end
        
        [b,a] = butter(3,[tx_freq_MHz-0.5 tx_freq_MHz+0.5]./(fs_MHz/2)); %BPF filter coefficients
        aveDataFiltered = filtfilt(b,a,double(aveDataLine'));  aveDataFiltered = aveDataFiltered'; %BPF around the TX center frequency
        
        lineOut(iLine,:) = delayAndSum(aveDataFiltered,iLine,c,fs_Hz,pitch_mm); %This is a delay, window, and sum beamformer to complete the formation of this image "line"
    end
    
    lineOutAligned = alignOiData(lineOut); %align each "line" in time (ADC trigger inconsistencies again!)
    
    lineOutAligned = lineOutAligned(1:end-1,:); %get rid of channel 16.
    
    if xx>1
        L = size(lineOutAligned,2);
        N = size(lineOutAligned,1);
        D = finddelay(ti(end,:),lineOutAligned(1,:),300);
        
        if D==0
            mergeMe = lineOutAligned;
        elseif D>0
            mergeMe = [lineOutAligned(:,D+1:L) zeros(N,D)];
        else
            D = abs(D);
            mergeMe = [zeros(N,D) lineOutAligned(:,1:L-D)];
        end
        
        % blend the two together
        ti(end-1,:) = (ti(end-1,:).*.75 + mergeMe(1,:).*.25);
        ti(end,:) = ti(end,:).*.25 + mergeMe(2,:).*.75;
        
        %balance the levels
        levelInds = 3400:3800;
        targLevel = rms(ti(end-2,levelInds));
        ti(end-1,:) = ti(end-1,:).*(targLevel./rms(ti(end-1,levelInds)));
        ti(end,:) = ti(end,:).*(targLevel./rms(ti(end,levelInds)));
        
        %merge
        ti = [ti; mergeMe(3:end,:)];
    else
        ti = lineOutAligned;
    end
end

ti = ti(:,3400:9000);

fake_tgc = logspace(0,2.6,size(ti,2)); %build a fake time-gain-compensation gain matrix (TGC was not enabled in hardware for this test)
ti = fake_tgc.*ti; %apply TGC

ti = sqrt(abs(hilbert(ti'))); %This is a standard way of turning ultrasound waveform data into images


%% Plot it!

figure;
imagesc(ti)
caxis([30 220])
% colormap(gray)
% % ylim([0 depth_cm])
% xlabel('cm')
% ylabel('cm')