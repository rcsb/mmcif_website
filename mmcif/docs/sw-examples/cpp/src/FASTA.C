/*************************
* FASTA.C
*
* For some CIF file, generate a FASTA file based on the monomer sequences
* stored in the struct_poly_seq category table. Optionally compare our
* codification with the one stored in the struct_poly category table.
*
* Lines with superscriptions contain footnoted references or explanations.
*************************/

#include <fstream>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CifFile.h"
#include "CifParserBase.h"
#include "ISTable.h"

#include <boost/assign/list_of.hpp>

#define COL_WIDTH_MAX 80

// Holds amino acid codification information
std::map<string, char> codification;

char identifyMonomer(const string& monomer);
void prepareCodes();
void showUsage();

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        showUsage();
    }

    // The name of the CIF file
    string cifFileName = argv[1];

    // Parsing diagnostics
    string diagnostics;

    // Compare our codifications with those contained in the CIF file
    bool compare (true);

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
  
    // Fill the codification map
    prepareCodes();

    // Get the first data block name in the CIF file
    string firstBlockName = cifFileP->GetFirstBlockName();

    // Retrieve the first data block
    Block &block = cifFileP->GetBlock(firstBlockName);

    // Retrieve the entity category table, which contains information that will be used in the FASTA header
    ISTable& entity = block.GetTable("entity"); 
	
    // Holds non-mandatory entity attributes that could serve as FASTA header lines, ordered preferentially
    string candidates[] = {"pdbx_description", "details", "type"};
	
    // The entity category table attribute to be used to fill the FASTA header
    string headerDescriptor;
	
    // Find the first present candidate
    for (unsigned int i = 0; i < 3; ++i)
    {
        if (entity.IsColumnPresent(candidates[i]))
        {
            headerDescriptor = candidates[i];
            break;
        }  
    }

    // If none of the optional descriptors are present, just use the entity id
    if(headerDescriptor.empty())
    {
        headerDescriptor = "id";
    } 

    // Retrieve the entity_poly_seq category group table, which contains the monomer sequences for entities
    ISTable& entity_poly_seq = block.GetTable("entity_poly_seq"); 
   
    // Use the CIF file name to generate the FASTA file name
    size_t fileExtPos = cifFileName.find(".cif");
    string outFileName = cifFileName.substr(0, fileExtPos) + ".fasta";

    // Create and open the FASTA file
    std::ofstream outFile;
    outFile.open(outFileName.c_str());

    // Container to hold each codified entity for optional comparison
    vector<string> codifiedEntities;
	
    // Keep track of the current column width and current entity id
    unsigned int columnWidth (0), entityID (1);

    // Holds the codification of the current entity
    string codifiedEntity;

    // Write the header to the FASTA file	
    outFile << ">" << entity(entityID - 1, headerDescriptor) << std::endl;

    for (unsigned int i = 0; i < entity_poly_seq.GetNumRows(); ++i, ++columnWidth)
    {
	
        // Obtain the ID of the entity described by this row
        unsigned int currentEntityID = atoi(entity_poly_seq(i, "entity_id").c_str());

        // If we are dealing with a new entity
        if (currentEntityID != entityID) 
        {
            // Write out the current entity's FASTA codification
            outFile << codifiedEntity;
			
            // Store the current entity's FASTA codification
            codifiedEntities.push_back(codifiedEntity);

            // Set the new entity ID
            entityID = currentEntityID;
         
            // Write out the new header
            outFile << "\n>" << entity(entityID - 1, headerDescriptor) << std::endl;
         
            columnWidth = 0;
            codifiedEntity.clear();
        }

        // If we have hit the maximum column width
        if (columnWidth == COL_WIDTH_MAX)
        {
            // Move to a new line and reset the width
            codifiedEntity += "\n";
            columnWidth = 0;
        }

        // Retrieve the monomer stored in this row 
        string monomer = entity_poly_seq(i, "mon_id");
        
        // Identify the monomer and add its codification
        codifiedEntity.push_back(identifyMonomer(monomer));
    }
	
    // Write out and store the last entity
    outFile << codifiedEntity;
    codifiedEntities.push_back(codifiedEntity);	

    // Close the FASTA file as we have no more monomers to read
    outFile.close();

    // Optional comparison against the codified sequences stored in entity_poly
    if (compare)
    {   
        // Retrieve the table corresponding to the entity_poly category, which contains one-letter
        // codified canonical sequences for entities, against which we can compare our conversions
        ISTable& entity_poly = block.GetTable("entity_poly");
		
        for (unsigned int i = 0; i < codifiedEntities.size(); ++i)
        {
            string temp = entity_poly(i, "pdbx_seq_one_letter_code_can");
            std::cout << "For entity #" << i + 1 << ":" << std::endl << std::endl;
            std::cout << "The codified version in the CIF file is as follows: " << std::endl << temp << std::endl << std::endl;
            std::cout << "The codified version obtained from translating entity_poly_seq is as follows: " << std::endl 
                << codifiedEntities[i] << std::endl << std::endl;
            int comparison = codifiedEntities[i].compare(temp);
            string equality = (!comparison) ? "equivalent" : "inequivalent";
            std::cout << "The two are " << equality + "." << std::endl << "---------------" << std::endl;
        }
    }
    return 0;
}

char identifyMonomer(const string& monomer)
{
    char code;

    // If we are dealing with an amino acid 
    if (monomer.length() == 3)
    {
        // Find its codification in the map, using 'X' if it is not found
        code = (codification.find(monomer) != codification.end()) ? codification[monomer] : 'X';
    }

    // If we are dealing with a nucleic acid, there is nothing to codify
    else
    {
        code = monomer[0];
    }

    return code;
}

void prepareCodes()
{
    codification = boost::assign::map_list_of
        ("ALA", 'A')
        ("CYS", 'C')
        ("ASP", 'D')
        ("GLU", 'E')
        ("PHE", 'F')
        ("GLY", 'G')
        ("HIS", 'H')
        ("ILE", 'I')
        ("LYS", 'K')
        ("LEU", 'L')
        ("MET", 'M')
        ("ASN", 'N')
        ("PYL", 'O')
        ("PRO", 'P')
        ("GLN", 'Q')
        ("ARG", 'R')
        ("SER", 'S')
        ("THR", 'T')
        ("SEC", 'U')
        ("VAL", 'V')
        ("TRP", 'W')
        ("TYR", 'Y');
}

void showUsage()
{
    std::cout << "Usage: ./FASTA /path/to/file.cif" << std::endl;
    exit(1);
}