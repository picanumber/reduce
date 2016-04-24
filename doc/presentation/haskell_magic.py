import os
import IPython.core.magic as ipym


@ipym.magics_class
class HaskellMagics(ipym.Magics):
    @ipym.cell_magic
    def ghc(self, line, cell=None):
        """Compile, execute Haskell code with ghc, and return the standard output."""
        prep = os.path.abspath(os.getcwd())
        # Define the source and executable filenames.
        source_filename  = prep + '/temp.hs'
        program_filename = prep + '/temp'
        # Write the code contained in the cell to the C++ file.
        with open(source_filename, 'w') as f:
            f.write(cell)
        # Compile the C++ code into an executable.
        compile = self.shell.getoutput(
                "ghc {0:s}".format(source_filename))
        # Execute the executable and return the output.
        output = self.shell.getoutput(program_filename)
        return output


if __name__ == '__main__':
    from IPython import get_ipython
    get_ipython().register_magics(HaskellMagics)

