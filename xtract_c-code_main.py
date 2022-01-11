import argparse
import time


def execute_extractor(filename):
    t0 = time.time()
    if not filename:
        return None
    metadata = extract_metadata(filename=filename)
    t1 = time.time()
    metadata.update({"extract time": (t1 - t0)})
    return metadata


# creates a dictionary containing information about a given .c file.
# TODO: intake file properly instead of just using a local one
def extract_metadata(filename):
    my_dict = dict(
        comments='',  # string of all comments
        function_list=[],  # list of lists. Each list contains func name, output type, and list of parameters
        lib_list=[],  # list of libraries include-ed
        struct_list=[]  # list of structs defined
    )

    # open the file for reading
    try:
        filehandle = open(filename, 'r')
    except Exception as e:
        return {'error': str(e)}

    # 1 if currently working through multiline comment, 0 otherwise.
    is_multiline_comment = 0
    # when a function is defined, this number is 0 because functions are defined outside of all brackets.
    num_open_brackets = 0

    # iterate through each line of the file
    num_lines = 0
    while True:
        # Case that will likely never be reached? A 20k-line c file? 
        if num_lines > 20000:
            break

        # read a single line, if there is one
        try:
            line = filehandle.readline()
        except Exception as e:
            return {'error': str(e)}
        if not line:
            break

        # if multiline comment starts in this line
        if line.find('/*') > -1:
            comment_start = line.find('/*')
            is_multiline_comment = 1
            if line.find('*/', comment_start) > -1:  # if that multiline comment ends in this line
                my_dict['comments'] += line[(comment_start + 2):(line.find('*/', comment_start) - 1)]  # add comment
                is_multiline_comment = 0
            else:  # if that comment ends in some future line (whole line after /* is comment)
                my_dict['comments'] += line[(line.find('/*') + 2):-1]  # add start of comment thru EOL to comments
                line = line[:line.find('/*') - 1]  # ignore comment when looking at rest of line

        # if multiline comment started in a previous line
        if is_multiline_comment == 1:
            if line.find('*/') > -1:  # if multiline comment ends in this line
                my_dict['comments'] += line[:line.find('*/') - 1]  # add start of line thru */ to comments
                line = line[line.find('*/') + 2:]  # ignore comment when looking at rest of line
                is_multiline_comment = 0
            else:  # if whole line is part of multiline comment
                my_dict['comments'] += line[:-1]  # add whole line to comments, minus \n
                continue

        # single line comment
        if line.find('//') > -1:
            x = line.find('//')
            my_dict['comments'] += line[x + 2:-1]  # add comment part to dict, exclude \n
            line = line[:x]  # ignore comment when looking at rest of line

        # add include-ed libraries to dictionary
        if line.startswith('#include '):
            my_dict['lib_list'].append(line[10:-2])

        # adds struct names
        if line.find('struct ') > -1:
            x = line.find('{')
            if x > line.find('struct '):  # if there's a { after the struct name
                my_dict['struct_list'].append(line[(line.find('struct ') + 7):x - 1])  # stop before '{'
            else:
                my_dict['struct_list'].append(line[(line.find('struct ') + 7):-1])  # ignore '\n'

        # add function and its output type to dictionary if current line is outside brackets
        if num_open_brackets == 0:
            # we know that functions are defined by having the type followed by ' ', then func name, then '('
            start = line.find(' ') + 1
            end = line.find('(', start)
            if start > 0 and end > -1:  # if there's a space followed at some point by a '('
                func_name = line[start:end]  # between ' ' and '('
                func_type = line[:start - 1]  # between beginning of line and ' '
                func_params = line[end + 1:line.find(')')].split(', ')  # does not work if params are split btwn 2 lines
                # note: func_params doesnt work if params are split btwn 2 lines
                # or if parameters are split by ',' instead of ', '

                # append a list containing func name, type, and params to the list of functions info.
                my_dict['function_list'].append([func_name, func_type, func_params])

        # increment/decrement num_open_brackets; when it is 0, we know we are not inside brackets.
        if line.find("{") > -1:
            num_open_brackets += 1
        if line.find("}") > -1:
            num_open_brackets -= 1

    # close the pointer to that file
    try:
        filehandle.close()
    except Exception as e:
        return {'error': str(e)}

    return my_dict


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='c-code extractor: main function')
    parser.add_argument('--path', type=str, default=None, 
                        help='name of file to xtract', required=True)
    argv = parser.parse_args()
    filename = argv.path

    meta = execute_extractor(filename)
    print(meta)
