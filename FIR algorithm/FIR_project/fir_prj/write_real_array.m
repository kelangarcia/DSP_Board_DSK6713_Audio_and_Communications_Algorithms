function [] = write_real_array(fname, var_name, var_type, data);

% [] = write_real_array(fname, var_name, var_type, data);
%
% Writes an array of data in a compilable C format.
%
% Inputs:
%	fname		Base name of the output file
%	var_name	Name of the array variable
%	var_type	Type of the variable
%	data		Values in the array

% Write the .c file
f = fopen(strcat(fname, '.c'), 'wb');
fprintf(f, '/* File automatically generated by write_real_array.m. */\n');
fprintf(f, '\n');
fprintf(f, '#define %s_LEN %d\n', var_name, length(data));
fprintf(f, '\n');
fprintf(f, '%s %s[%s_LEN] = {\n', var_type, var_name, var_name);
fprintf(f, '%g,\n', data(1:end-1));
fprintf(f, '%g};\n', data(end));
fclose(f);

% write the .h file
f = fopen(strcat(fname, '.h'), 'wb');
fprintf(f, '/* File automatically generated by write_real_array.m. */\n');
fprintf(f, '\n');
fprintf(f, '#define %s_LEN %d\n', var_name, length(data));
fprintf(f, '\n');
fprintf(f, 'extern %s %s[%s_LEN];\n', var_type, var_name, var_name);
fclose(f);