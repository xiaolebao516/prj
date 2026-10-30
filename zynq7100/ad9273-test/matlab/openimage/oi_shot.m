% OI_SHOT  Object containing settings for one waveform out / recording in set.
classdef oi_shot
	properties (GetAccess=public,SetAccess=private)
		% The transmit (waveform) side.
		tx
		
		% The receive (recording) side.
		rx
	end
	
	methods (Access=public)
		function this = oi_shot()
			this.tx = oi_tx;
			this.rx = oi_rx;
		end
	end
	
	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		
		function fill(this, bb)
			% Fill the byte buffer with configuration data for this shot.
			this.tx.fill(bb);
			this.rx.fill(bb);
		end

	end % hidden methods
	
		
end

