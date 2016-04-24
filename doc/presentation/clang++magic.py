import os
import IPython.core.magic as ipym


@ipym.magics_class
class ClangppMagics(ipym.Magics):
    @ipym.cell_magic
    def clang(self, line, cell=None):
        """Compile, execute C++ code with clang, and return the standard output."""
        prep = os.path.abspath(os.getcwd())
        # Define the source and executable filenames.
        source_filename  = prep + '/temp.cpp'
        program_filename = prep + '/temp.exe'
        # Write the code contained in the cell to the C++ file.
        with open(source_filename, 'w') as f:
            f.write(cell)
        # Compile the C++ code into an executable.
        compile = self.shell.getoutput(
                "clang++ -std=c++1z -O2 -Wall -pedantic -pthread {0:s} -o {1:s}".format(
                source_filename, program_filename))
        # Execute the executable and return the output.
        output = self.shell.getoutput(program_filename)
        return output


if __name__ == '__main__':
    from IPython import get_ipython
    get_ipython().register_magics(ClangppMagics)

