import sys

# takes a list file (https://github.com/wowdev/wow-listfile/releases) and outputs just the db paths for faster loading.

def filter_lines(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            stripped_line = line.strip()  # Remove leading and trailing whitespace
            # Check if the line ends with 'db', 'dbc', or 'db2'
            if stripped_line.endswith(('db', 'dbc', 'db2')):
                outfile.write(line)  # Write the original line to the output file

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python filter-listfile.py <src> <dest>")
        sys.exit(1)

    src = sys.argv[1]
    dest = sys.argv[2]
    filter_lines(src, dest)