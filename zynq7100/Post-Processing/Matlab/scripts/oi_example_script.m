% oi_example_script.m
%
% Script to test the Open Image System.
%
% 2020-09-29 JCW/WHF Created from anaToneTest
% 2020-10-01 JCW Parameterization and Engineering Units

%% Housekeeping
clear all
% close all
clc

%% Inputs
collect_new_data = 1; %loads exisiting data if false
save_new_data = 0; %overwrites the existing data file with new data
save_name = 'foo.mat'; %filename for new data
depth_cm = 6; %imaging depth, cm
n_shots = 4; %number of shots per frame
tx_freq_hz = 5.5e6; % Transmit Waveform Center Frequeny
tx_n_pulses = 2.5; % Number of Cycles in Transmit Waveform
tx_amp = 2; %[-2, -1, 0, 1, or 2], 2 corresponds to 80Vpk, 1 to 40Vpk
tx_window = 1;

%% Constants
c = 1500; %speed of sound, m/s
pitch_mm = 0.3; %transducer pitch, mm
rx_startup_N = 3000; %approximately number of samples the adc gets before the shot is fired.

%% Calculations
depth_m = depth_cm/100; %m
travel_time = 2*depth_m/c; %s
tx_duration_sec = tx_n_pulses/tx_freq_hz;

%% More Housekeeping
if ~exist('openimage', 'class'),
    addpath ../openimage
end

%% Get and Process data

if collect_new_data
    
    % 1) Connect to the imager.
    oi = openimage;
    oi.open;
    oi.get_status
    
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
    % Be sure to end with zero.
    wave(end+1) = 0;
    
    % 4) Apply the waveform to all channels and all shots.
    for iShot = 1:fr.nShots,
        for iCh = 1:openimage.N_CHAN,
            fr.shots(iShot).tx.channels(iCh).enable = true; % NYI, but set anyway
            fr.shots(iShot).tx.channels(iCh).levelSequence = wave;
        end
    end
    
    % 5) Setup recording. Note many of these parameters require two-element
    % arrays (one for each ultrasound AFE/ADC IC).
    for iShot = 1:fr.nShots,
        % 5a) Number of samples of this shot:
        nPoints = travel_time*(oi_rx.fs*1e6); %samples
        fr.shots(iShot).rx.N = floor(nPoints) + rx_startup_N; %the ADCs presample by around 2700 samples
        % 5b) Desired test mode:
        fr.shots(iShot).rx.testMode = [0 0];  % % 0 = normal operation; 1 = channel id and ramp; 2 = digital sine; 3 = analog tones; 4 = user IO pattern
        % 5c) Gains/filtering:
        fr.shots(iShot).rx.lna = [15.6 15.6];  %LNA_VALUES = [15.6, 17.9, 21.6];
        fr.shots(iShot).rx.pga = [21 21];  %PGA_VALUES = [21, 24, 27, 30];
        fr.shots(iShot).rx.lpf = [10 10]; %MHz
        fr.shots(iShot).rx.hpf_divisor = [12 12]; %divisor on HPF value
        tgc_N = floor(fr.shots(iShot).rx.N./oi_rx.fs); %TGC is at 1 MHz
        tgc_startup_N = floor(rx_startup_N./oi_rx.fs); %TGC is at 1 MHz
        tgc_levels = [45.*ones(1,tgc_startup_N) linspace(45,0,tgc_N-tgc_startup_N)];
        fr.shots(iShot).rx.tgc = { tgc_levels , tgc_levels }; % Each field of the cell array should be a vector of the desired *attenuation* in dB, from 0 to 45 dB, sampled at 1 MHz, for up to 300 entries.
        % 5d) Enable the channels:
        for iCh = 1:openimage.N_CHAN,
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
    
    % 9) Close the connection.
    oi.close;
    
%     10) Plot.
%     for ii=1:1:16
%     figure; plot(fd(ii,:)); title(num2str(ii))
%     end
%     
    if save_new_data
        save(save_name)
    end
    
else
    load('water_reflection.mat');
end

%% Build vectors for image display
fs = fr.shots(1,1).rx.fs.*1e6;
t = 0:1/fs:(fr.shots(1).rx.N-1)/fs;
z = c.*t./2.*100; %depth axis (cm)
x = pitch_mm/2:pitch_mm:16.5*pitch_mm; %transverse axis (mm)

%% form the image from the raw data
fd2 = reshape(fd,16,[],fr.nShots);
figure; plot(squeeze(fd2(4,:,:)))

fdm = mean(fd2,3); %averaging all shots now to improve SNR
[b,a] = butter(3,1e6/(fs/2),'high'); %high pass filter coefficients (sample rates specified in MHz)
fdm = filtfilt(b,a,fdm'); fdm=fdm'; %high-pass filtering to remove some DC
figure;
plot(fdm(4,:))

fi = sqrt(abs(hilbert(fdm)));
figure;
imagesc(x./10,z,fi')
colormap(gray)
ylim([0 depth_cm])
xlabel('cm')
ylabel('cm')
% axis equal