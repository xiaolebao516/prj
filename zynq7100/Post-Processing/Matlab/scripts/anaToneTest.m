% anaToneTest.m
%
% Script to test the ADC and its analog tone generation capability.
%
% 2020-08-07  WHF  Created.
%

if ~exist('openimage', 'class'),
	addpath ../openimage
end

% 1) Connect to the imager.
oi = openimage;
oi.open;
oi.get_status

% 2) Create the frame, and specify the number of shots.
fr = oi_frame;
fr.nShots = 10;

% 3) Generate the desired pulser waveform.  We won't see it, because we're using
%  test tones, but we can look at it on the scope.
tx_fs = oi_tx.CLOCK_RATE;
duration = 0.5e-6;   % half a microsecond
nWave = floor(duration * tx_fs);
t = (0:nWave)./tx_fs;
freq = 4e6; % 4 MHz
% The pulser wavetrain has 5 states, from -2 to 2.
wave = round(2 * sin(2*pi*freq .* t));
% Be sure to end with zero.
wave(end+1) = 0;

% 4) Apply the waveform to all channels and all shots.
for iShot = 1:fr.nShots,
	for iCh = 1:openimage.N_CHAN,
		fr.shots(iShot).tx.channels(iCh).enable = true; % NYI, but set anyway
		fr.shots(iShot).tx.channels(iCh).levelSequence = wave;
	end
end

% 5) Setup recording.
for iShot = 1:fr.nShots,
	% 5a) Number of samples of this shot:
	fr.shots(iShot).rx.N = 2^16;
	% 5b) Desired test mode:
	fr.shots(iShot).rx.testMode = [3 3];  % 3 => analog tones
	% 5c) Gains/filtering:
	odd = bitand(iShot,1);
	gain = [100 100]*odd;  % alternate min max
	fr.shots(iShot).rx.lna = gain;  % will round to nearest choice
	fr.shots(iShot).rx.pga = gain;  % ditto
	fr.shots(iShot).rx.lpf = [8 8];
	fr.shots(iShot).rx.hpf_divisor = [12 12];
	% 5d) TGC:
	fr.shots(iShot).rx.tgc = { 2:2:200, 1:100 };
	% 5e) Enable the channels:	
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

% 10) Plot.
figure; plot(fd(1,:)); title('Channel 1.  2.5 MHz Analog Tone')
figure; plot(fd(9,:)); title('Channel 9.  2.5 MHz Analog Tone')

