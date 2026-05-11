% OI_TX_CHANNEL  Settings for an individual transmit channel.
classdef oi_tx_channel < handle
	properties (Access=public)
		% Set to true to enable the channel, false to disable (Hi-Z).
		enable = false
		
		% Waveform to generate.  The waveform will be output at
		%   oi_tx.CLOCK_RATE.
		%  2 = Vpp0, 1 = Vpp1, 0 = RTZ, -1 = Vnn1, -2 = Vnn0;
		%  e.g., [2 -2] would be  single-cycle square wave.
		%  Note that the waveform *must* begin and end with RTZ.
		levelSequence = int8([0 0]);
	end
	
	%%%%%  Accessors  %%%%%
	methods
		function set.levelSequence(this, LS)
			LS = int8(LS);
			if length(LS) > oi_tx_channel.MAX_N_LEVEL_SEQUENCE,
				error(sprintf( ...
						'The level sequence is limited to a length of %d.', ...
						oi_tx_channel.MAX_N_LEVEL_SEQUENCE ...
				));
			end
			if any(LS>2 | LS <-2),
				error('Level sequence must lie btw -2 and 2.')
			end
			this.levelSequence = LS;
		end
	end % accessors	
	
	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		
		function fill(this, bb)
			% Fill the byte buffer with configuration data for this TX channel.
			bb.putInt(this.enable);
			bb.putInt(length(this.levelSequence));
			
			LS = this.levelSequence;
			% Pad
			LS = [LS(:)' int8(zeros(1, this.MAX_N_LEVEL_SEQUENCE-length(LS)))];
			bb.put(LS);
		end

	end % hidden methods
	
	% Private Constants
	properties (Access=private, Constant=true)
		MAX_N_LEVEL_SEQUENCE = 252
	end % private constants
	
end
