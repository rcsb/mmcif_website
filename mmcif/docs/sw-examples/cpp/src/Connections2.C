/*************************
* Connections2.C
*
* For some CIF file, generate a Chimera command (COM) file
* to iterate through and emphasize connections of specific connection types
* and involving specific types of entities. As an example, we will
* look for polymer-polymer covalent linkages. 
*
* Method: For connections of interest, determine each partner atom's entity
* type by indexing into the struct_asym category table with the atom's asym_id to 
* determine its entity ID. Then, index into the entity category table to 
* determine its entity type.
*
* Lines with superscriptions contain footnoted references or explanations.
*************************/

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CifFile.h"
#include "CifParserBase.h"
#include "ISTable.h"

void prepareOutFile(std::ofstream& outFile, const string& outFileName);
void showUsage();
void writeConnection(std::ofstream& outFile, const string& select);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        showUsage();
    }

    // The name of the CIF file
    string cifFileName = argv[1];
    
    // A string to hold any parsing diagnostics
    string diagnostics;

    // Create CIF file and parser objects
    CifFile *cifFileP = new CifFile;
    CifParser *cifParserP = new CifParser(cifFileP);

    // Parse the CIF file
    cifParserP->Parse(cifFileName, diagnostics);

    // Delete the CIF parser, as it is no longer needed
    delete cifParserP;

    // Display any diagnostics
    if (!diagnostics.empty())
    {
        std::cout << "Diagnostics: " << std::endl << diagnostics << std::endl;
    }

    // Get the first data block name in the CIF file 
    string firstBlockName = cifFileP->GetFirstBlockName();

    // Retrieve the first data block 
    Block &block = cifFileP->GetBlock(firstBlockName);

    // Retrieve the table corresponding to the struct_conn category, which delineates connections
    ISTable& struct_conn = block.GetTable("struct_conn");

    // Retrieve the table corresponding to the struct_asym category, which 
    // details structural elements in the asymmetric unit
    ISTable& struct_asym = block.GetTable("struct_asym"); 
	
    // Retrieve the table corresponding to the entity category, which details the 
    // molecular entities present in the crystallographic structure
    ISTable& entity = block.GetTable("entity");

    // Use the CIF file name to generate the Chimera command file (.COM) name
    size_t fileExtPos = cifFileName.find(".cif");
    string outFileName = cifFileName.substr(0, fileExtPos) + ".com";

    // Create the command file
    std::ofstream outFile;
    outFile.open(outFileName.c_str());
	
    // Write out some basic Chimera initialization commands
    prepareOutFile(outFile, cifFileName);

    // Iterate through every row in the struct_conn category table, where each row delineates an interatomic connection 
    for (unsigned int i = 0; i < struct_conn.GetNumRows(); ++i)
    {
        // We are only interested in checking the entity types for covalent linkages
        if (struct_conn(i, "conn_type_id") != "covale")
        {
            continue;
        }

        // Holds each partner atom's entity type
        string entityTypes[2];

        // A Chimera selection string for the two partner atoms
        string select;

        // Analyze the current row twice, once per partner
        for (unsigned int j = 0; j < 2; ++j)
        {   
            // Determine which partner we are dealing with
            string partner = (!j) ? "ptnr1_" : "ptnr2_";

            // Retrieve all the information necessary to uniquely identify that partner atom
            string alt_id = struct_conn(i, "pdbx_" + partner + "label_alt_id");
            string asym_id = struct_conn(i, partner + "auth_asym_id");
            string atom_id = struct_conn(i, partner + "label_atom_id");
            string comp_id = struct_conn(i, partner + "auth_comp_id");
            string seq_id = struct_conn(i, partner + "auth_seq_id");

            // Add to the Chimera selection string for this atom
            select += ":" + seq_id + "," + comp_id + "." + asym_id + "@" + atom_id + "." + alt_id;
 
            // Add an OR to the selection string before partner 2 so that Chimera grabs both partner atoms
            if (!j)
            {
                select += " | ";
            }

            // Holds the atom's entity ID, which we will find in the struct_asym category table
            unsigned int entityID (0);
			
            // Iterate over every row in the struct_asym category table
            for (unsigned int k = 0; k < struct_asym.GetNumRows(); ++k)
            {	
                // Check that we have the right asym_id
                if (struct_asym(k, "id") == asym_id)
                {
                    // Retrieve the entity ID
                    entityID = atoi(struct_asym(k, "entity_id").c_str());
					
                    // Finished looking
                    break;
                }
            }

            // Retrieve and store the entity type (subtract 1 because the rows are zero-indexed)
            entityTypes[j] = entity(entityID - 1, "type");
        }

        // Write satisfactoy entity pairings to the COM file (in this case polymer-polymer)
        if (entityTypes[0] == "polymer" && entityTypes[1] == "polymer")
        {
            writeConnection(outFile, select);
        }
    }

    // Write out the Chimera close command
    outFile << "stop\n";

    // Close the COM file as all connections have been processed
    outFile.close();

    return 0;
}

void prepareOutFile(std::ofstream& outFile, const string& outFileName)
{
    outFile << "windowsize 500 500\n"; // Set the window size to 500 x 500 px
    outFile << "open " + outFileName + "\n"; // Open the CIF file
    outFile << "preset apply pub 4\n"; // Apply publication preset #4
    outFile << "color white\n"; // Color the entire molecule white
    outFile << "set bg_color gray\n"; // Color the background gray
    outFile << "repr bs\n"; // Represent the atoms in ball-and-stick format
    outFile << "savepos fullview\n"; // Remember this position (the full view of the molecule)
}

void showUsage()
{
    std::cout << "Usage: ./Connections2 /path/to/file.cif" << std::endl;
    exit(1);
}

void writeConnection(std::ofstream& outFile, const string& select)
{
    outFile << "sel " + select << std::endl; // Select the two partner atoms
    outFile << "color byelement sel; label sel\n"; // Color them yellow
    outFile << "sel sel za<3.0; wait 20\n"; // Further select all atoms within 3.0 angstroms of the partner atoms
    outFile << "focus sel; wait 25; ~disp ~sel; wait 68\n"; // Focus on the selection and hide all non-selected atoms
    outFile << "disp ~sel; ~label sel; reset fullview 20\n"; // Return to the full molecule view
    outFile << "color white sel; ~sel; wait 20\n"; // Uncolor and drop the selection
}
