from os.path import splitext
from pdbx.reader.PdbxReader import PdbxReader
from pdbx.reader.PdbxContainers import *
from sys import argv, exit

def prepareOutFile(file, name) :
    file.write("windowsize 500 500\n") # Set the window size to 500 x 500px
    file.write("open %s\n" % name) # Open the CIF file
    file.write("preset apply pub 4\n") # Apply publication preset #4
    file.write("color white\n") # Color the molecule white
    file.write("set bg_color gray\n") # Color the background gray
    file.write("disp; repr bs; set silhouette\n") # Represent the atoms in ball-and-stick format
    file.write("savepos fullview\n") # Save this position (i.e., the full view of molecule)

def writeConnection(selection, file) :
    file.write("color green ligand\n")
    file.write("sel %s\n" % " | ".join(selection)) # Select the site members
    file.write("color blue sel\n") # Color them blue
    file.write("sel sel | ligand; wait 20\n") # Further select the ligand
    file.write("focus sel; wait 25; ~disp ~sel; wait 100\n") # Focus in on the selection and hide non-selected atoms
    file.write("disp ~sel; reset fullview 20\n") # Reset to the full molecule view
    file.write("color white sel; ~sel; wait 20\n") # Drop and uncolor the selection

# Check for improper usage
if len(argv) != 2 :
    exit("Usage: python Structures.py /path/to/file.cif")

# Open the CIF file
cif = open(argv[1])

# A list to be propagated with data blocks
data = []

# Create a PdbxReader object
pRd = PdbxReader(cif)

# Read the CIF file, propagating the data list
pRd.read(data)

# Close the CIF file, as it is no longer needed
cif.close()

# Retrieve the first data block
block = data[0]

# Retrieve the struct_site_gen category table, which delineates members of structurally relevant sites
struct_site_gen = block.getObj("struct_site_gen")

# Use the CIF file pathname to generate the Chimera command file (.COM) pathname
(file, ext) = splitext(argv[1])
comFileName = file + ".com"

# Open the COM command file for writing
comFile = open(comFileName, 'w');

# Write out some basic Chimera initialization commands
prepareOutFile(comFile, argv[1])

# Store the site_id of the first row
currentSite = struct_site_gen["site_id"]

# A Chimera selection string list for the site
selection = []

# Iterate over the rows in struct_site_gen, where each row delineates a member of a structurally relevant site
for index in range(struct_site_gen.getRowCount()) :

    # Retrieve the site_id of the current row
    site = struct_site_gen.getValue("site_id", index)

    # Check for a new site
    if currentSite != site :

        # Write out commands for Chimera to customize the display of this site
        writeConnection(selection, comFile)

        # Set the current site_id to this site_id
        currentSite = site

        # Clear the selection string list
        selection = []

    # Append the latest site member
    selection.append(":%s,%s.%s" % (struct_site_gen.getValue("auth_seq_id", index),
                                    struct_site_gen.getValue("auth_comp_id", index),
                                    struct_site_gen.getValue("auth_asym_id", index)))

# Write out commands for Chimera to customize the display of the last site
writeConnection(selection, comFile)

# Write out the Chimera close command, as all connections have been processed
comFile.write("stop\n")

# Close the COM file
comFile.close()
