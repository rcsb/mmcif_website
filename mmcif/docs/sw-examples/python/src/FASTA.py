from os.path import splitext
from pdbx.reader.PdbxReader import PdbxReader
from pdbx.reader.PdbxContainers import *
from sys import argv, exit

# Amino acid FASTA codification dictionary
codification = { "ALA" : 'A',
                 "CYS" : 'C',
                 "ASP" : 'D',
                 "GLU" : 'E',
                 "PHE" : 'F',
                 "GLY" : 'G',
                 "HIS" : 'H',
                 "ILE" : 'I',
                 "LYS" : 'K',
                 "LEU" : 'L',
                 "MET" : 'M',
                 "ASN" : 'N',
                 "PYL" : 'O',
                 "PRO" : 'P',
                 "GLN" : 'Q',
                 "ARG" : 'R',
                 "SER" : 'S',
                 "THR" : 'T',
                 "SEC" : 'U',
                 "VAL" : 'V',
                 "TRP" : 'W',
                 "TYR" : 'Y' }

# Compare our translations against the codifications stored in the CIF file
compare = True

# Check for improper usage
if len(argv) != 2 :
    exit("Usage: python FASTA.py /path/to/file.cif")

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

# Retrieve the entity category table, which contains information that will be used in the FASTA header
entity = block.getObj("entity")

# Holds non-mandatory entity attributes that could serve as FASTA header lines, ordered preferentially
candidates = ["pdbx_description", "details", "type"]

# The entity category table attribute to be used to fill the FASTA header
headerDescriptor = ""

# Set the header descriptor
for c in candidates :
    if entity.hasAttribute(c) :
        headerDescriptor = c
        break

# If none of the optional descriptors are present, just use the entity id
if not headerDescriptor: headerDescriptor = "id"

# Retrieve the entity_poly_seq category table, 
entity_poly_seq = block.getObj("entity_poly_seq")

# Use the CIF file pathname to generate the FASTA file (.fasta) pathname
(file, ext) = splitext(argv[1])
fastaFileName = file + ".fasta"

# Open the FASTA file for writing
fasta = open(fastaFileName, "w")

# A container to hold each codified entity for optional comparison
codifiedEntities = []

# Track the current entity
currentEntity = 1

# Track the column width
columnWidth = 0

# Codification of the current entity
codifiedEntity = ""

# Write the first header to the FASTA file
fasta.write(">%s\n" % entity.getValue(headerDescriptor, currentEntity - 1))

# Iterate over every row in entity_poly_seq, each containing an entity monomer
for index in range(entity_poly_seq.getRowCount()) :

    # Obtain the ID of the entity monomer described by this row
    tempEntity = (int)(entity_poly_seq.getValue("entity_id", index))
    
    # If we are dealing with a new entity
    if currentEntity != tempEntity :

        # Write out the current entity's FASTA codification
        fasta.write(codifiedEntity)
        
        # Store the current entity's FASTA codification
        codifiedEntities.append(codifiedEntity)

        # Set the new entity ID
        currentEntity = tempEntity

        # Write out the new header
        fasta.write("\n>%s\n" % entity.getValue(headerDescriptor, currentEntity - 1))

        columnWidth = 0
        codifiedEntity = ""

    # If we have hit the maximum column width
    if columnWidth == 80 :

        # Move to a new line and reset the width
        codifiedEntity += "\n"
        columnWidth = 0

    # Retrieve the monomer stored in this row
    monomer = entity_poly_seq.getValue("mon_id", index)

    # If the monomer is an amino acid
    if len(monomer) == 3 :
	
        # If it's in the codification dictionary, add it
        if codification.has_key(monomer) :
            codifiedEntity += codification[monomer]
			
        # Otherwise, use the default value "X"
        else :
            codifiedEntity += "X"

    # If it's a nucleic acid, there is nothing to codify
    else :
        codifiedEntity += monomer

    columnWidth += 1

# Write out and store the last entity
fasta.write(codifiedEntity)
codifiedEntities.append(codifiedEntity)

# Close the FASTA file
fasta.close()

# Optional comparison against the codified sequences stored in entity_poly
if compare :

    # Retrieve the table corresponding to the entity_poly category, which contains one letter
    # codified canonical sequences for entities, against which we can compare our conversions
    entity_poly = block.getObj("entity_poly")

    # Compare every codification with the one in the CIF file
    for i in range(len(codifiedEntities)) :
        code = entity_poly.getValue("pdbx_seq_one_letter_code_can", i)
        print "For entity #%d: " % (i + 1)
        print "\nThe codified version in the CIF file is as follows: " 
        print code
        print "\nThe codified version obtained from translating entity_poly_seq is as follows: "
        print codifiedEntities[i]
        equality = (code == codifiedEntities[i]) and "equivalent" or "inequivalent"
        print "\nThe two are %s." % (equality) 
        print "---"