from os.path import splitext
from pdbx.reader.PdbxReader import PdbxReader
from pdbx.reader.PdbxContainers import *
from sys import argv, exit

def prepareOutFile(file, name) :
    file.write("windowsize 500 500\n") # Set the window size to 500 x 500 px
    file.write("open %s\n" % name) # Open the CIF file
    file.write("preset apply pub 4\n") # Apply publication preset #4
    file.write("color white\n") # Color the entire molecule white
    file.write("set bg_color gray\n") # Color the background gray
    file.write("repr bs\n") # Represent the atoms in ball-and-stick format
    file.write("savepos fullview\n") # Remember this position (the full view of the molecule)

def writeConnection(selection, file) :
    sel = " | ".join(selection) 
    file.write("sel %s\n" % sel) # Select the two partner atoms 
    file.write("color byelement sel; label sel\n") # Color them by element
    file.write("sel sel za<3.0; wait 20\n") # Further select all atoms within 3.0 angstroms of the partner atoms
    file.write("focus sel; wait 25; ~disp ~sel; wait 68\n"); # Focus on the selection and hide all non-selected atoms
    file.write("disp ~sel; ~label sel; reset fullview 20\n") # Return to the full molecule view
    file.write("color white sel; ~sel; wait 20\n") # Uncolor and drop the selection

# Check for improper usage
if len(argv) != 2 :
    exit("Usage: python Connections2.py /path/to/file.cif");

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

# Get the struct_asym category table, which details structural elements in the asymmetric unit
struct_asym = block.getObj("struct_asym")

# Get the entity category table, which details the molecular entities present in the crystallographic structure
entity = block.getObj("entity")

# Use the CIF file pathname to generate the Chimera command file (.COM) pathname
(file, ext) = splitext(argv[1])
comFileName = file + ".com"

# Open the COM command file for writing
comFile = open(comFileName, 'w');

# Write out some basic Chimera initialization commands
prepareOutFile(comFile, argv[1])

# Iterate over the rows in struct_conn, where each row delineates an interatomic connection
for index in range(struct_conn.getRowCount()) :

    # A container to hold each partner atom's entity type
    entities = []

    # A container to hold each partner atom's Chimera selection string
    selection = []

    # Verify that the connection is covalent
    if struct_conn.getValue("conn_type_id", index) == "covale" :

        # Analyze the current row twice, once per partner
        for partner in ["ptnr1_", "ptnr2_"] :
    
            # Retrieve the partner atom's asym_id, with which we will index into struct_asym
            asym_id = struct_conn.getValue(partner + "auth_asym_id", index) 
	
	        # Add this atom's Chimera selection string to the container	
            selection.append(":%s,%s.%s@%s.%s" % (struct_conn.getValue(partner + "auth_seq_id", index), 
                                      struct_conn.getValue(partner + "auth_comp_id", index),
                                      struct_conn.getValue(partner + "auth_asym_id", index), 
                                      struct_conn.getValue(partner + "label_atom_id", index),
                                      struct_conn.getValue("pdbx_" + partner + "label_alt_id", index)))
        
            # Holds the atom's entity ID, which we will find in the struct_asym category table
            entityID = 0
 
            # Find the atom's entity ID in the struct_asym category table
            for i in range(struct_asym.getRowCount()) :
                if struct_asym.getValue("id", i) == asym_id :
                    entityID = (int)(struct_asym.getValue("entity_id", i))
                    break

			# Retrieve and store the atom's entity type
            entities.append(entity.getValue("type", entityID - 1))
           
		# Write satisfactory entity pairings to the COM file (in this case polymer-polymer)
        if entities[0] == "polymer" and entities[1] == "polymer" :        
            writeConnection(selection, comFile)

# Write out the Chimera close command
comFile.write("stop\n")

# Close the COM file as all connections have been processed
comFile.close()
