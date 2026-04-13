% OI_TX  Object to hold transmit settings for a shot.
classdef oi_tx < handle
	%%%%%  Public Constants  %%%%%
	properties (Access=public, Constant=true)
		% Clock rate of the waveform.  Currently fixed.
		CLOCK_RATE = 220e6
	end
	
	properties (GetAccess=public, SetAccess=private)
		channels = oi_tx_channel.empty
	end
	
	% Constructor:
	methods (Access=public)
		function this = oi_tx()
			% Must initialize handle objects in constructor, or all
			%  will get identical copies:
			this.channels(openimage.N_CHAN) = oi_tx_channel;
		end
	end
	
	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		
		function fill(this, bb)
			% Fill the byte buffer with TX configuration data for this shot.
			for iChan=1:openimage.N_CHAN,
				% Delegate to channel:
				this.channels(iChan).fill(bb);
			end
		end

	end % hidden methods
	
end


