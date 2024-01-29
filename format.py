#!/usr/bin/python

# Returns a list of filenames that need formatted.
def code_files():
    import glob
    src = glob.glob("src/**/*.cpp", recursive=True)
    src += glob.glob("src/**/*.hpp", recursive=True)
    src += glob.glob("src/**/*.h", recursive=True)
    return src

# This can actually be called anything, not just main.  But function names
# should to some extent indicate what they actually do.
def main():
    print ("Formatting Code:")
    for f in code_files():
        print ("  %s" % f)
        # run clang-format on 'f' here
    print ("done!")

# This part, the  "__name__ == '__main__'" is telling Python this is where
# code execution should start.  This is the proper way to to begin
# a program written in Python.
if __name__ == '__main__':
    main()  # call the real program function.
