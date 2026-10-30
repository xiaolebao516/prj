% OPENIMAGE  MATLAB Interface to the Open Image open ultrasonic imager.
%
% Enter 'methods openimage' to see available methods.  Then enter
%  'help openimage.X' to see detailed help a particular method X.
%
classdef openimage < handle
	%%%%%  Public Constants  %%%%%
	properties (Access=public, Constant=true)
		N_CHAN = 16
	end

	% Public Methods
	methods
		% Constructor:
		function this = openimage()
		end
		
		function open(this)
			% OPENIMAGE.OPEN  Open a connection to the imager.
			
			% This is the address of the imager.  It is a fixed static address
			%  for now.
			%sa = java.net.InetSocketAddress(10.11.2.60,26000);
			sa = java.net.InetSocketAddress('10.11.34.215', 26000);

			if ~isempty(this.socket),
				error('Already connected.  Use close.');
			end
			
			% Open the connection.			
			this.socket = java.nio.channels.SocketChannel.open(sa)
		end
		
		function close(this)
			% OPENIMAGE.CLOSE  Close the connection to the imager.
			if isempty(this.socket),
				% silently ignore
			else,
				this.socket.close;
				this.socket = [];
			end
		end
		
		function queue_frame(this, fr)
			% OPENIMAGE.QUEUE_FRAME  Send the frame to the imager to fill.
			%
			% The settings stored in the shots stored in the frame will be sent
			%  to the imager, which will begin generating the output waveforms
			%  and recording samples.
			%
			%  This openimage must be connected.
			this.checkConnected;
			
			% Reset the frame handle.
			fr.resetHandle;
			
			% Build up a byte buffer to send.
			bb = java.nio.ByteBuffer.allocate(2^19);
			bb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
			bb.put(openimage.MAGIC);
			bb.put(openimage.CMD_QUEUE_FRAME);
			bb.putShort(0); % fill later with size
			% Delegate to the frame to fill:
			fr.fill(bb);
			% Fill the size:
			bb.putShort(2, uint16(bb.position - 4));
			% Send out:
			bb.flip;
			this.socket.write(bb);
			
			this.getResponse(openimage.RES_ACK);
			
		end
		
		function data = get_frame(this, fr)
			% OPENIMAGE.GET_FRAME  Get data from the previously queued frame.
			%
			% When a queued frame is executed, the imager records data from 
			%  all shots contiguously in RAM.  This method waits until the
			%  recording is complete, and then streams all data to the host.
			%
			% The data will be interleaved, i.e., the first sample of the first
			%  enabled channel, followed by the first sample of the second
			%  enabled channel, etc.  All samples are int16.
			this.checkConnected;
			
			% Compute the number of bytes of data:
			nBytesPerAdc = 0;
			for iShot = 1:fr.nShots,
				nBytesPerAdc = nBytesPerAdc ...
						+ fr.shots(iShot).rx.N * 2 * oi_rx.N_CHAN_PER_CHIP;
			end
			
			bb = java.nio.ByteBuffer.allocate(128);
			bb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
			bb.put(openimage.MAGIC);
			bb.put(openimage.CMD_GET_FRAME);			
			bb.putShort(12); % three u32
			bb.putInt(0);
			bb.putInt(0);
			bb.putInt(0);
			bb.flip;
			
			blob_u8 = uint8(zeros(2, nBytesPerAdc));
			
			for iAdc = 1:oi_rx.N_CHIPS,
				offset = 0;
				nPerReq = 1024;

				bb.putInt(4, iAdc-1);
			
				while offset < nBytesPerAdc,
					nThisReq = min(nBytesPerAdc-offset, nPerReq);
					bb.putInt(8, offset);
					bb.putInt(12, nThisReq);
					bb.rewind;
					this.socket.write(bb);
					
					bytes = this.getResponse(openimage.RES_FRAME);
					if length(bytes) ~= nThisReq,
						error('Wrong number of bytes received.');
					end
					blob_u8(iAdc, offset+(1:length(bytes))) = bytes;
					offset = offset + length(bytes);
				end
			end
			
			% Reinterpret as i16, reshape and aggregate:
			data = [
					reshape(typecast(blob_u8(1,:), 'int16'),8,[]);
					reshape(typecast(blob_u8(2,:), 'int16'),8,[]);
			];
		end
		
		function status = get_status(this)
			% OPENIMAGE.GET_STATUS  Request status from a connected imager.
			this.checkConnected;
				
			this.socket.write(java.nio.ByteBuffer.wrap(uint8([ ...
					openimage.MAGIC openimage.CMD_STATUS 0 0])));
			
			bytes = this.getResponse(openimage.RES_STATUS);
			
			status = struct( ...
				'state', openimage.STATE_STR{bytes(1)}, ...
				'flags', typecast(bytes(5:8), 'uint32'), ...
				'buildDate', char(openimage.zeroTrim(bytes(9:end)')) ...
			);
		end % get_status
	end
	
	% Hidden Class Methods
	methods (Static=true, Hidden=true)
	end
	
	% Private Methods
	methods (Access=private)
		function checkConnected(this)
			if isempty(this.socket), error('Not connected.'); end
		end
		
		function [out, code] = getResponse(this, respCode)
			% Gets the response to a command; compares header against
			%  expected code.
			if nargin < 2,
				respCode = [];
			end
			bb = java.nio.ByteBuffer.allocate(2048);
			bb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
			% Read from the imager:
			this.socket.read(bb);
			% Setup the buffer for reading:
			bb.flip;
			% Get the backing array, and extract the valid bytes:
			ar = bb.array;
			ar = typecast(ar(1:bb.limit), 'uint8');
						
			if ar(1) ~= openimage.MAGIC,
				error('Response did not contain magic.');
			elseif ar(2) == openimage.RES_NACK,
				openimage.handleNack(ar(5));
			elseif ~isempty(respCode) && ar(2) ~= respCode,
				error(sprintf( ...
						['Response code (%02x) did not match' ...
						' expected value (%02x.'], ...
						ar(2), ...
						respCode ...
				) );
			else,
				repLen = typecast(ar(3:4),'uint16');
				len = length(ar)-4;
				if repLen ~= len,
					error(sprintf( ...
							['Reported length (%d) different'  ... 
							  ' from received length (%d).'],  ...
							repLen, ...
							len ...
					))
				else,
					code = ar(2);
					out = ar(5:end);
				end
			end
		end % getResponse
	end % private methods
		
	% Private Class Methods
	methods (Static=true, Access=private)
		function handleNack(err)
			if (err < length(openimage.ERROR_STR)),
				error(openimage.ERROR_STR{err+1});
			else,
				error(sprintf('Unknown error code: 0x%02x.', err));
			end
		end
	
		function zb = zeroTrim(ib)
			i = find(ib==0, 1);
			if isempty(i),
				zb = ib;
			else
				zb = ib(1:i-1);
			end
		end
	end % private methods
	
	% Private Constants
	properties (Access=private, Constant=true)
		MAGIC = 260/2
		%%%  Command Codes  %%%
		CMD_STATUS = hex2dec('01')
		CMD_QUEUE_FRAME = hex2dec('11')
		CMD_GET_FRAME = hex2dec('12')
		
		%%% Response Codes %%%
		RES_ACK = hex2dec('80')
		RES_STATUS = hex2dec('81')
		RES_FRAME = hex2dec('92')
		RES_NACK = hex2dec('FF')
		
		%%% Strings %%%
		STATE_STR = {  % INIT skipped
			'READY',
			'ARMED',
			'RECORDING',
			'FAULT'
		};
		ERROR_STR = {
			'NONE',
			'The command was not recognized by the imager.',
			'The packet the imager received was bad.',
			'The command packet was an incorrect size.',
			'The imager is in the wrong state for that command.',
			'An invalid parameter was passed to the imager.'
		};
	end
	
	% Private Data
	properties (Access=private)
		% A Java SocketChannel used to communicate with the imager.
		socket
	end
end


