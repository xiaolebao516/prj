% OI_FRAME  Holds settings for a continuous ultrasound imager recording.
%
classdef oi_frame < handle
	properties (GetAccess=public, SetAccess=private)
		% Array of oi_shot objects representing waveform / recording sessions.
		%  The length of this array is managed with the nShots parameter.
		shots = oi_shot.empty
	end
	
	properties (Access=public)
		% Length control of the 'shots' array.
		nShots = 0
	end
	
	%%%%%  Accessors  %%%%%
	methods
		function h = get.handle(this)
			% Return the handle for the frame.
			h = this.handle;
		end
		
		function set.nShots(this, val)
			val = uint32(val);
			
			if val > oi_frame.MAX_N_SHOTS,
				error(sprintf( ...
						'The number of shots cannot exceed %d.', ...
						oi_frame.MAX_N_SHOTS ...
				));
			end
			
			if val > length(this.shots),
				% Extend.  Note separate constructors must be called, or they
				%  will all end up with the same object.
				for iShot = length(this.shots)+1:val,
					this.shots(iShot) = oi_shot();
				end
			else,
				% contract
				this.shots = this.shots(1:val);
			end
			this.nShots = length(this.shots);
		end
	end

	%%%%%  Hidden Methods  %%%%%		
	methods (Access=public, Hidden=true)
		function resetHandle(this)
			% Generates a new handle.  Called when the frame is queued.
			rng('shuffle');
			this.handle = randi(2^32,'uint32');
		end
		
		function fill(this, bb)
			% Fill the byte buffer with configuration data from this frame.
			bb.putInt(this.handle);
			bb.putInt(this.nShots);
			for iShot=1:this.nShots,
				this.shots(iShot).fill(bb);
			end
		end

	end % hidden methods
	
	%%%%%  Private Constants  %%%%%
	properties (Access=private, Constant=true)
		MAX_N_SHOTS = 100
	end
	
	%%%%%  Hidden Properties  %%%%%
	properties (GetAccess=public, SetAccess=private, Hidden=true)
		% Randomly generated integer used to identify the frame to the imager.
		handle
	end
end % oi_frame

