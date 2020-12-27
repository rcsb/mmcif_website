/*************************
 * Structures.C
 *
 * For some CIF file, generate a Chimera command (COM) file
 * to iterate through and emphasize each structurally relevant site.
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

void prepareOutFile(std::ofstream& outFile, const string& cifFileName);
void showUsage();
void writeSite(std::ofstream& outFile, const string& select);

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

    // Retrieve the table corresponding to the struct_site_gen category, which delineates structurally relevant sites
    ISTable& struct_site_gen = block.GetTable("struct_site_gen");

    // Use the CIF file name to generate the Chimera command file (.COM) name
    size_t fileExtPos = cifFileName.find(".cif");
    string outFileName = cifFileName.substr(0, fileExtPos) + ".com"; 

    // Create the command file
    std::ofstream outFile;
    outFile.open(outFileName.c_str());
	
    // Write out some basic Chimera initialization commands
    prepareOutFile(outFile, cifFileName);

    // A string to remember the ID of the current site being read
    string currentSite;

    // A Chimera selection string for the site members
    string select;

    // Iterate through every row in the struct_site_gen category table
    for (unsigned int i = 0; i < struct_site_gen.GetNumRows(); ++i)
    {

        // Get the site identifier for the current row
        string site_id = struct_site_gen(i, "site_id");

        // Check for the first site
        if (currentSite.empty())
        {
            currentSite = site_id;
        }
        
        // Check for a new site
        else if (currentSite != site_id)
        {	
            // Write out commands for Chimera to customize the display of the current site
            writeSite(outFile, select);
            
            // Clear the Chimera selection string
            select.clear();
  
            // Make this site our new current site
            currentSite = site_id;
        }

        // Otherwise, we are adding another site member
        else
        {   
            select += " | ";
        }

        // Retrieve all information necessary to uniquely identify this site member
        string asym_id = struct_site_gen(i, "auth_asym_id");
        string comp_id = struct_site_gen(i, "auth_comp_id");
        string seq_id = struct_site_gen(i, "auth_seq_id");

        // Add the member to the Chimera selection string for this site
        select += ":" + seq_id + "," + comp_id + "." + asym_id;
    }
	
    // Write out the last site
    writeSite(outFile, select);

    // Write out the Chimera close command
    outFile << "stop\n";

    // Close the COM file as all sites have been processed
    outFile.close();

    return 0;
}    

void prepareOutFile(std::ofstream& outFile, const string& cifFileName)
{
    outFile << "windowsize 500 500\n"; // Set the window size to 500 x 500 px
    outFile << "open " + cifFileName << std::endl; // Open the CIF file
    outFile << "preset apply pub 4\n"; // Apply publication preset #4 
    outFile << "color white\n"; // Color the entire molecule white
    outFile << "set bg_color gray\n"; // Color the background gray
    outFile << "disp; repr bs; set silhouette\n"; // Represent the atoms in ball-and-stick format 
    outFile << "savepos fullview\n"; // Remember this position (i.e., the full view of molecule)
}

void showUsage()
{
    std::cout << "Usage: ./Structures /path/to/file.cif" << std::endl;
    exit(1);
}

void writeSite(std::ofstream& outFile, const string& select)
{
    outFile << "color green ligand\n"; // Color the ligand green
    outFile << "sel " + select << std::endl; // Select the site members
    outFile << "color blue sel\n"; // Color them blue
    outFile << "sel sel | ligand; wait 20\n"; // Further select the ligand
    outFile << "focus sel; wait 25; ~disp ~sel; wait 100\n"; // Focus in on the selection and hide non-selected atoms
    outFile << "disp ~sel; reset fullview 20\n"; // Return to the full view of the molecule
    outFile << "color white sel; ~sel; wait 20\n"; // Uncolor and drop the selection
}
