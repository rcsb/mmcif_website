from os.path import splitext
from pdbx.reader.PdbxReader import PdbxReader
from pdbx.reader.PdbxContainers import *
from sys import argv, exit

# Check for improper usage
if len(argv) != 2 :
    exit("Usage: python Connections3.py /path/to/file.cif");

# Open the CIF file
cif = open(argv[1])

# Create a list to store data blocks
data = []

# Create a PdbxReader object
pRd = PdbxReader(cif)

# Read the CIF file, propagating the data list
pRd.read(data)

# Close the CIF file, as it is no longer needed
cif.close()

# Retrieve the first data block
block = data[0]

# Get the struct_conn category table, which delineates connections
struct_conn = block.getObj("struct_conn")

# Get the atom_site category table, which delineates atomic constituents
atom_site = block.getObj("atom_site")

# Iterate over every rows in struct_conn, where each row delineates an interatomic connection
for index in range(struct_conn.getRowCount()) :

    # Verify that the connection is covalent
    if struct_conn.getValue("conn_type_id", index) == "covale" :

        # Container for coordinates
        coords = []

        # Analyze the current row twice, once per partner
        for partner in ["ptnr1_", "ptnr2_"] :    

            # Retrieve all the information necessary to uniquely identify the atom
            atom = {"auth_seq_id" : struct_conn.getValue(partner + "auth_seq_id", index), 
                "auth_comp_id" : struct_conn.getValue(partner + "auth_comp_id", index), 
                "auth_asym_id" : struct_conn.getValue(partner + "auth_asym_id", index), 
                "label_atom_id" : struct_conn.getValue(partner + "label_atom_id", index),
                "label_alt_id" : struct_conn.getValue("pdbx_" + partner + "label_alt_id", index)}

			# Iterate over every row in the atom_site category table
            for i in range(atom_site.getRowCount()) :
                found = True
				# Look for the row corresponding to this atom
                for key in atom.keys() :
                    if atom_site.getValue(key, i) != atom[key] :
                        found = False
				# Store the coordinates of the atom
                if (found) :
                    coords.append("(%s, %s, %s)" % (atom_site.getValue("Cartn_x", i),
                                                   atom_site.getValue("Cartn_y", i),
                                                   atom_site.getValue("Cartn_z", i)))
                    break
 
        print "Found a covalent bond between atoms located at %s & %s" % (coords[0], coords[1])