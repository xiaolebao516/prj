% OI_RX  Definition of openimage receive properties for a single shot.
classdef oi_rx < handle
	%%%%%  Public Constants  %%%%%
	properties (Access=public, Constant=true)
		% Number of channels per each receiver chip; they will thus share the
		%  same settings.
		N_CHAN_PER_CHIP = 8
		
		% Number of chips in the device.
		N_CHIPS = openimage.N_CHAN / oi_rx.N_CHAN_PER_CHIP
		
		% Sample rate (MHz).  Fixed.
		fs = 80

		% TGC update rate (MHz).  Fixed.
		TGC_FS = 1		
	end
	
	properties (GetAccess=public, SetAccess=private)
		% See the oi_rx_channel class.
		channels = oi_rx_channel.empty
	end

	%%%%%  Public Properties  %%%%%	
	properties (Access=public)
		% Number of samples to acquire for each of the enabled channels.
		N = 2^16;
		
		% Time Gain Compensation.  This is the amount of attenuation to apply
		%  to recorded samples, and can vary throughout the recording.
		%  The values are in dB, and can range from 0 to 45 dB.
		%
		% Note there is one TGC control per 8 channels.
		%
		% The TGC can be updated at 10 MHz.  If empty, no
		%  attenuation is applied.  If a scalar, the same attenuation is
		%  applied throughout the recording.  If a vector, the values are
		%  assumed to be sampled at 10 MHz.  If a two-by-N matrix, the 
		%  first row or column will be interpreted as the time, and the second
		%  the desired attenuation, and the values will be interpolated
		%  for the fixed sampling interval.
		tgc = cell(1, oi_rx.N_CHIPS);
		
		% Test mode.
		% 0 = normal operation
		% 1 = channel id and ramp
		% 2 = digital sine
		% 3 = analog tones
		% 4 = user IO pattern
		testMode = [0 0];
		
	end % public (independent) properties
		
	properties (Access=public, Dependent=true)
		% Low noise amplifier.  The gain may be one of 15.6, 17.9, or 21.6 dB;
		%  the input will be rounded to the nearest value.
		%
		% Note there is one LNA control per 8 channels.
		lna
		
		% Programmable gain amplifier.  The gain may be one of 21, 24, 27, or 30
		%  dB.  The input will be rounded to the nearest value.
		pga
				
		% High pass filter setting, as a divisor of the low pass setting.
		%  For example, if the low pass filter is set to 8 MHz, and the divisor
		%  is 12, the high pass filter cutoff will be about 670 kHz.
		%
		%  The divisor can be one of 3, 6, 9, or 12.
		hpf_divisor

		% Programmable low pass anti-aliasing filter.  This should be set
		%  after the sample rate; changing the sample rate will also change
		%  this setting.  The filter can vary between 8 and 30 MHz.  Only a few
		%  discrete values are allowed; this software will automatically 
		%  select the closest value.
		%
		% After changing, the filter must be tuned.  This will occur at the
		%  beginning of the frame, before waveform generation.
		lpf
	end % public (dependent) properties
	
	%%%%%  Constructor  %%%%%
	methods (Access=public)
		function this = oi_rx()
			% Must initialize handle objects in constructor, or all
			%  will get identical copies:
			 this.channels(openimage.N_CHAN) = oi_rx_channel();
		end
	end
	
	%%%%%  Accessors  %%%%%
	methods
		%function set.fs(this, val)
		%	assert( ...
		%			val >= 20.5 && val <= 165, ...
		%			['The sampling frequency must lie between' ...
		%			' 20.5 and 165 MHz.'] ...
		%	);
		%	this.fs = val;
		%end

		function set.tgc(this, val)
			assert(                                                        ...
					length(val) == oi_rx.N_CHIPS,                          ...
					'The "tgc" field must have %d entries.',               ...
					oi_rx.N_CHIPS                                          ...
			);
			for iCh=1:oi_rx.N_CHIPS,
				assert( ...
						length(val{iCh}) <= oi_rx.MAX_N_TGC,               ...
						['Each entry of the "tgc" field must have at most' ...
						 ' %d entries.'],                                  ...
						oi_rx.MAX_N_TGC                                    ...
				);
			end
			this.tgc = val;
		end

		function out = get.lna(this)
			out = this.LNA_VALUES(this.lnaReg+1);
		end
		
		function set.lna(this, val)
			assert(                                                        ...
					length(val) == oi_rx.N_CHIPS,                          ...
					'The "lna" field must have %d entries.',               ...
					oi_rx.N_CHIPS                                          ...
			);
			[~, this.lnaReg] = this.round_to(this.LNA_VALUES, val);
		end
		
		
		function out = get.pga(this)
			out = this.PGA_VALUES(this.pgaReg+1);
		end
		
		function set.pga(this, val)
			assert(                                                        ...
					length(val) == oi_rx.N_CHIPS,                          ...
					'The "pga" field must have %d entries.',               ...
					oi_rx.N_CHIPS                                          ...
			);
			[~, this.pgaReg] = this.round_to(this.PGA_VALUES, val);
		end
		
		
		function out = get.hpf_divisor(this)
			out = this.HPF_DIV_VALUES(this.hpfDivReg+1);
		end
		
		function set.hpf_divisor(this, val)
			assert(                                                        ...
					length(val) == oi_rx.N_CHIPS,                          ...
					'The "hpf_divisor" field must have %d entries.',       ...
					oi_rx.N_CHIPS                                          ...
			);
			[~, this.hpfDivReg] = this.round_to(this.HPF_DIV_VALUES, val);
		end
		
		
		function freq = get.lpf(this)
			freq = this.fs ./ this.LPF_DIV_VALUES(this.iLpfDiv+1) ...
					.* this.LPF_MUL_VALUES(this.iLpfMul+1);
			if any(freq < 8) || any(freq > 30),
				warning( ...
						'openimage:oi_rx:lpf_freq', ...
						'LPF frequency is out of tunable range (8-30 MHz)' ...
				);
			end
		end
		
		function set.lpf(this, val)
			assert(                                                        ...
					length(val) == oi_rx.N_CHIPS,                          ...
					'The "lpf" field must have %d entries.',               ...
					oi_rx.N_CHIPS                                          ...
			);
			assert( ...
					all(val >= 8 & val <= 30), ...
					'The LPF frequencies must lie between 8 and 30.' ...
			);
			for iVal=1:length(val),
				des = val(iVal); % desired frequency
				iTry = 0;
				for iDiv = 1:length(this.LPF_DIV_VALUES),
					for iMul = 1:length(this.LPF_MUL_VALUES),
						iTry = iTry+1;
						s(iTry).iDiv = iDiv;
						s(iTry).iMul = iMul;
						s(iTry).freq = this.fs ./ this.LPF_DIV_VALUES(iDiv) ...
								.* this.LPF_MUL_VALUES(iMul);
						if s(iTry).freq < 8 || s(iTry).freq > 30,
							s(iTry).freq = 1e99; % replace with unreasonable
						end
					end
				end
				% Select the best value:
				[~,i] = min(abs([s(:).freq]-des));
				this.iLpfDiv(iVal) = s(i).iDiv - 1; % convert 1 based to 0 based
				this.iLpfMul(iVal) = s(i).iMul - 1; % convert 1 based to 0 based
			end
		end % set.lpf
	end % public accessors
	
	%%%%%  Private Class Methods  %%%%%
	methods (Static=true, Access=private)
		function [out, iOut] = round_to(set, val)
			out = val;
			iOut = zeros(size(val));
			for iv=1:length(val),
				[~,i] = min(abs(set-val(iv)));
				out(iv) = set(i);
				iOut(iv) = i-1;  % zero based
			end
		end
		
		function fillTgc(tgc, bb)
			x = uint8(zeros(1,oi_rx.MAX_N_TGC));
			% Convert.  From page 20 of the AD9670 datasheet:
			% VGAatt(dB) = -14 dB/V * (1.6 - Vgain)
			% The TGC DAC is 8-bit.
			x(1:length(tgc)) = uint8(255 * tgc / 14.0 / 3.2);
			
			bb.put(x);
		end

	end % private class methods
	
	%%%%%  Private Methods  %%%%%
	methods (Access=private)
	end
	
	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		
		function fill(this, bb)
			% Fill the byte buffer with RX configuration data for this shot.
			for iChan=1:openimage.N_CHAN,
				this.channels(iChan).fill(bb);
			end
			
			bb.putInt(this.N);  % nSamples
			bb.putInt(length(this.tgc{1}));
			bb.putInt(length(this.tgc{2}));
			
			bb.put(int8(this.iLpfMul));
			bb.put(int8(this.iLpfDiv));
			
			this.fillTgc(this.tgc{1}, bb);
			this.fillTgc(this.tgc{2}, bb);
			
			bb.put(int8(this.lnaReg));
			bb.put(int8(this.pgaReg));
			bb.put(int8(this.hpfDivReg));
			bb.put(int8(this.testMode));
		end

	end % hidden methods	
	
	%%%%%  Private Constants  %%%%%
	properties (Constant=true, Access=private)
		MAX_N_TGC = 300
	
		% Note all these arrays should be ordered as to match the register
		%  layouts.
		LNA_VALUES = [15.6, 17.9, 21.6];
		PGA_VALUES = [21, 24, 27, 30];
		HPF_DIV_VALUES = [12, 9, 6, 3];
		LPF_MUL_VALUES = [1.45 1.25 1.13 1.0 0.9 0.8 0.75];
		LPF_DIV_VALUES = [3 4.5 6];
	end

	%%%%%  Private Data  %%%%%
	properties (Access=private)
		% Selection of LPF multiplier (bits 5:3 of register 0x00F):
		iLpfMul = [3 3]	
		
		% Selection of LPF divisor (bits 7:6 or register 0x00F):
		iLpfDiv = [0 0]
		
		% LNA register (one for each chip).
		lnaReg = [2 2]
		
		% PGA register (one for each chip).
		pgaReg = [3 3]
		
		% HPF filter register (one for each chip).
		hpfDivReg = [0 0]
	end
end
