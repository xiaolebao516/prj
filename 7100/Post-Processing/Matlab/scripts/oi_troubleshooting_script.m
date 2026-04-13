% oi_troubleshooting_script.m
%
% Script to troubleshoot and evaluate new approaches for the Open Image System.
%
% 2021-01-31 JCW Built from oi_example_script.m

%% Housekeeping
clear all
% close all
clc

%% Inputs
collect_new_data = 1; %loads exisiting data if false
save_new_data = 1; %overwrites the existing data file with new data
save_name = '20210206_ATS_0mm.mat'; %filename for new data
depth_cm = 6; %imaging depth, cm
focal_depths_cm = [1 2 3 4 5 6];
n_shots = 6; %number of shots per frame
tx_freq_hz = 5.5e6; % Transmit Waveform Center Frequeny
tx_n_pulses = 1; % Number of Cycles in Transmit Waveform
tx_amp = 2; %[-2, -1, 0, 1, or 2], 2 corresponds to 80Vpk, 1 to 40Vpk
tx_window = 0;

%% Constants
c = 1450; %speed of sound, m/s
pitch_mm = 0.3; %transducer pitch, mm
rx_startup_N = 3000; %approximately number of samples the adc gets before the shot is fired.
%% Calculations
depth_m = depth_cm/100; %m
travel_time = 2*depth_m/c; %s
tx_duration_sec = tx_n_pulses/tx_freq_hz;

%% More Housekeeping
if ~exist('openimage', 'class')
    addpath ../openimage
end

%% Get and Process data
if collect_new_data
    
    % 1) Connect to the imager.
    oi = openimage;
    oi.open;
    oi.get_status
    
    % 1B) Configure the Imaging Line
    for iDepth = 1:length(focal_depths_cm)
        focal_depth_cm = focal_depths_cm(iDepth);
        for iLine = 1:openimage.N_CHAN %will become one line in the image
            apperature_distance_mm = abs([1:16] - iLine).*pitch_mm;
            flight_distance_cm = sqrt(focal_depth_cm.^2 + (0.1*apperature_distance_mm).^2);
            flight_duration_sec = 0.01.*flight_distance_cm./c;
            flight_duration_delta_sec = flight_duration_sec - min(flight_duration_sec);
            flight_duration_delta_cycles = round(flight_duration_delta_sec.*oi_tx.CLOCK_RATE);
            tx_delay_cycles = max(flight_duration_delta_cycles) - flight_duration_delta_cycles;
            
            % 2) Create the frame, and specify the number of shots.
            fr = oi_frame;
            fr.nShots = n_shots;
            
            % 3) Generate the desired pulser waveform.
            tx_fs = oi_tx.CLOCK_RATE;
            nWave = floor(tx_duration_sec * tx_fs);
            t = (0:nWave)./tx_fs;
            if tx_window
                win = hamming(length(t));
                win = win';
            else
                win = ones(size(t));
            end
            % The pulser wavetrain has 5 states, from -2 to 2.
            wave = round(win .* tx_amp .* sin(2*pi*tx_freq_hz .* t));
            
            
            % 4) Apply the waveform to all channels and all shots.
            
            for iShot = 1:fr.nShots
                for iCh = 1:openimage.N_CHAN
                    channel_wave = [zeros(1,tx_delay_cycles(iCh)) wave zeros(1,flight_duration_delta_cycles(iCh))];
                    channel_wave(end+1) = 0;  % Be sure to end with zero.
                    fr.shots(iShot).tx.channels(iCh).enable = true; % NYI, but set anyway
                    fr.shots(iShot).tx.channels(iCh).levelSequence = channel_wave;
%                     figure(1)
%                     plot(channel_wave); hold on;
                end
            end
            
            
            % 5) Setup recording. Note many of these parameters require two-element
            % arrays (one for each ultrasound AFE/ADC IC).
            for iShot = 1:fr.nShots
                % 5a) Number of samples of this shot:
                nPoints = travel_time*(oi_rx.fs*1e6) + rx_startup_N; %samples
                fr.shots(iShot).rx.N = round(nPoints);
                % 5b) Desired test mode:
                fr.shots(iShot).rx.testMode = [0 0];  % % 0 = normal operation; 1 = channel id and ramp; 2 = digital sine; 3 = analog tones; 4 = user IO pattern
                % 5c) Gains/filtering:
                fr.shots(iShot).rx.lna = [21.6 21.6];  %LNA_VALUES = [15.6, 17.9, 21.6];
                fr.shots(iShot).rx.pga = [27 27];  %PGA_VALUES = [21, 24, 27, 30];
                fr.shots(iShot).rx.lpf = [10 10]; %MHz
                fr.shots(iShot).rx.hpf_divisor = [12 12]; %divisor on HPF value
                tgc_N = floor(fr.shots(iShot).rx.N./oi_rx.fs); %TGC is at 1 MHz
                tgc_startup_N = floor(rx_startup_N./oi_rx.fs); %TGC is at 1 MHz
                tgc_levels = [45.*ones(1,tgc_startup_N) linspace(45,0,tgc_N-tgc_startup_N)];
                fr.shots(iShot).rx.tgc = { tgc_levels , tgc_levels }; % Each field of the cell array should be a vector of the desired *attenuation* in dB, from 0 to 45 dB, sampled at 1 MHz, for up to 300 entries.
                % 5d) Enable the channels:
                for iCh = 1:openimage.N_CHAN
                    fr.shots(iShot).rx.channels(iCh).enable = true; % NYI, but set anyway
                end
            end
            
            % 6) Queue the frame.
            oi.queue_frame(fr);
            
            % 7) Request status, until the state is ready.
            oi.get_status
            pause(1)  % TODO.  We just wait a bit.
            oi.get_status
            
            % 8) Get the frame data.
            fd = oi.get_frame(fr);
            
            % 8B) Plot.
            if 0
                for ii=1:1:16
                    figure; plot(fd(ii,:)); title(num2str(ii))
                end
            end
            
            data(iDepth,iLine,:,:,:) = fd;
            
            if iLine ~=16
                clear fr
            end
        end %Looping through lines
    end %Looping through focal depths
    
    % 9) Close the connection.
    oi.close;
 
    if save_new_data
        save(save_name)
    end
    
else %collect_new_data == false
    load(save_name);
end

%% Build vectors for image display
fs_Hz = fr.shots(1,1).rx.fs.*1e6;
fs_MHz = fs_Hz/1e6;
t = 0:1/fs_Hz:(fr.shots(1).rx.N-1)/fs_Hz;
z = c.*t./2.*100; %depth axis (cm)
x = pitch_mm/2:pitch_mm:16.5*pitch_mm; %transverse axis (mm)
tx_freq_MHz = tx_freq_hz./1e6;
[b,a] = butter(3,[tx_freq_MHz-1 tx_freq_MHz+1]./(fs_MHz/2)); %filter coefficients

%% form the image from the raw data

for iLine = 1:size(data,2) %loop through lines
    
    for iDepth = 1:size(data,1) %loop through focusing depths

        fd = squeeze(data(iDepth,iLine,:,:,:));
        fd2 = double(reshape(fd,16,[],fr.nShots));
        
        aveData(:,:,iDepth) = alignAndAverage(fd2); %aligns data via cross-correlation 
    end
    
    if size(data,1)>1
        aveDataLine = alignAndAverage(aveData);
    else
        aveDataLine = aveData;
    end
    aveDataFiltered = filtfilt(b,a,double(aveDataLine'));  aveDataFiltered = aveDataFiltered';
    
    lineOut(iLine,:) = delayAndSum(aveDataFiltered,iLine,c,fs_Hz,pitch_mm);
end

lineOut = lineOut';
             
fi = sqrt(abs(hilbert(lineOut(rx_startup_N:end,:))));

fi = fi./max(max(fi));

figure;
imagesc(x./10,z,fi)
% colormap(gray)
ylim([0 depth_cm])
xlabel('cm')
ylabel('cm')
