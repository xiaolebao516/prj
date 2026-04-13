% deadbeef.m
%
% Script to test ADC-FPGA interface.
%
% 2021-02-08  WHF  Created.
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
fr.nShots = 1;

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
	fr.shots(iShot).rx.N = 2^8;
	% 5b) Desired test mode:
	fr.shots(iShot).rx.testMode = [4 4];  % 4 => user data
	% 5c) Gains/filtering:
	% Leave at defaults.
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

% 10) Verify the 'deadbeef'ness of the data.
for i=1:16,
	fprintf(' %2d  ', i);
end
fprintf('\n');
for r=1:size(fd,2),
	for i=1:16,
		fprintf('%04x ', typecast(fd(i, r),'uint16'));
	end
	fprintf('\n');
end


