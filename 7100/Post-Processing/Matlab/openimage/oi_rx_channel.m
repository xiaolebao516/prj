% OI_RX_CHANNEL  Object to contain settings particular to a receive channel.
classdef oi_rx_channel < handle
	properties (Access=public)
		% Set to true to record on this channel; false otherwise.
		enable = false
	end
	
	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		
		function fill(this, bb)
			% Fill the byte buffer with configuration data for this RX channel.
			bb.putInt(this.enable);
		end

	end % hidden methods
	
end
