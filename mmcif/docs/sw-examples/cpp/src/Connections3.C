/*************************
* Connections3.C
*
* For some CIF file, determine the (x, y, z) Cartesian coordinates
* of every atom involved in a covalent linkage.
*
* Method: Using the identifying information in the struct_conn category table,
* whittle down the set of possible indices in the atom_site category table to one.
*
* Highlighted lines contain footnoted references or explanations.
*************************/

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CifFile.h"
#include "CifParserBase.h"
#include "ISTable.h"

void showUsage();

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
 
    // Retrieve the table corresponding to the atom_site category, which describes atomic constituents
    ISTable& atom_site = block.GetTable("atom_site");
     
    // Iterate through every row in the struct_conn category table, where each row delineates an interatomic connection 
    for (unsigned int i = 0; i < struct_conn.GetNumRows(); ++i)
    {
        // Verify that the linkage is covalent
        if (struct_conn(i, "conn_type_id") != "covale")
        {
            continue;
        }
		
        std::cout << "\nFound a covalent bond between atoms located at: ";
         
        // Analyze the current row twice, once per partner
        for (unsigned int j = 0; j < 2; ++j)
        {  
            // Determine which partner we are dealing with
            string partner = (!j) ? "ptnr1_" : "ptnr2_";
 
            // Will hold the index of the atom in the atom_site category table
            vector<unsigned int> results;
			
			// Holds the attribute names and target values for these attributes
			vector<string> colNames, targets;
			
			colNames.push_back("label_alt_id");
			targets.push_back(struct_conn(i, "pdbx_" + partner + "label_alt_id"));
			
			colNames.push_back("auth_asym_id");
			targets.push_back(struct_conn(i, partner + "auth_asym_id"));
			
			colNames.push_back("label_atom_id");
			targets.push_back(struct_conn(i, partner + "label_atom_id"));
			
			colNames.push_back("auth_comp_id");
			targets.push_back(struct_conn(i, partner + "auth_comp_id"));
			
			colNames.push_back("auth_seq_id");
			targets.push_back(struct_conn(i, partner + "auth_seq_id"));
			
			// Perform a search on the atom_site table using the atom's unique identification information
			atom_site.Search(results, targets, colNames);
			
			// Retrieve and display the atom's coordinates
			vector<string> temp;
			atom_site.GetRow(temp, results[0], "Cartn_x", "Cartn_z");
			std::cout << "(" + temp[0] + ", " + temp[1] + ", " + temp[2] + ")";

            // Add an 'and' before partner 2
            if (!j)
            {
                std::cout << " & ";
            }
        }
    }
    std::cout << std::endl;
    return 0;
}

void showUsage()
{
    std::cout << "Usage: ./Connections3 /path/to/file.cif" << std::endl;
    exit(1);
}
