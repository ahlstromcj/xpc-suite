function [O_x, O_y] = OCTread_FIG(fname)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%%   OCTreadFIG.m
%%
%%   This function loads a MATLab .fig file, strips the data
%%   and axes info, then plots it in Octave.
%%
%%   fname = string containing filename
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

a_file = load('-mat',fname);
f_file = fieldnames(a_file);					%% extract struct name

file_handle = f_file{1};

G_object = getfield(a_file,file_handle);			%% this is the graphics object

for jj = 1:length(G_object.children)

	if strcmp(G_object.children(jj).type,'axes') == 1			%% struct containing data

		F_object = G_object.children(jj).children;			%% axes, data, labels			
		cc = 1;

		for ii = 1:length(F_object)
	
			L_object = F_object(ii);			%% dive into the object, into properties
		
			if isfield(L_object.properties,'XData') | isfield(L_object.properties,'String')

			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			%% Look for XData and YData
			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			if strcmp(L_object.type,'graph2d.lineseries') == 1
				O_x{cc} = L_object.properties.XData;
				O_y{cc} = L_object.properties.YData;
				cc = cc + 1;
			end
 
			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			%% Look for axes labels
			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			if strcmp(L_object.type,'text') == 1
				if  strcmp(L_object.properties.String,'Frequency (MHz)')
					if iscell(L_object.properties.String)
						F_xlabel = L_object.properties.String{1};
					else
						F_xlabel = L_object.properties.String;
					end
				elseif isfield(L_object.properties,'Rotation')
					if L_object.properties.Rotation == 90
						if iscell(L_object.properties.String)
							F_ylabel = L_object.properties.String{1};
						else
							F_ylabel = L_object.properties.String;
						end
					end
				end
			end

			end %% another if

		end %% ii
	end %% if
end %% jj

col = 'rgbcymrgbcymrgbcymrgbcym';					%% colour vector for plotting
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Check if we stripped the axes labels
%%
%% if not, set them to 'Missing'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if ~exist('F_xlabel')
	F_xlabel = 'Missing';
end

if ~exist('F_ylabel')
	F_ylabel = 'Missing';
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if length(O_x) == 0
	disp('Error - no data found!');
else

	figure;
	hold on
	xlabel(F_xlabel);
	ylabel(F_ylabel);

	for ii = 1:length(O_x)
		plot(O_x{ii},O_y{ii},col(ii));
	end
end