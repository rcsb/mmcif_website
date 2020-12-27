#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multi_array.hpp>
#include <boost/tokenizer.hpp>

using boost::lexical_cast;

#include "CifFile.h"
#include "CifParserBase.h"
#include "ISTable.h"

// Type to facilitate the construction of 4x4 matrices
typedef boost::multi_array<double, 2> matrix;

unsigned int findOperationIndex(ISTable& oper_list, const string& id);
void parseOperationExpression(vector<string>& ops, const string& expression);
void prepareOperation(ISTable& oper_list, matrix& operation, const unsigned int oper1, const int oper2);

int main(int argc, char **argv)
{
    // The pathname of the CIF file
    string cifName = argv[1];
	
    // A string to hold any parsing diagnostics
    string diagnostics;

    // Create CIF file and parser objects
    CifFile *cifFileP = new CifFile;
    CifParser *cifParserP = new CifParser(cifFileP);

    // Parse the CIF file
    cifParserP->Parse(cifName, diagnostics);

    // Delete the CIF parser, as it is no longer needed
    delete cifParserP;
	
    // Display any parsing diagnostics
    if (!diagnostics.empty())
	{
	    std::cout << "Diagnostics: " << std::endl << diagnostics << std::endl;
	}

    // Get the first data block name in the CIF file
    string firstBlockName = cifFileP->GetFirstBlockName();

    // Retrieve the first data block
    Block& block = cifFileP->GetBlock(firstBlockName);

    // Retrieve the table corresponding to the atom_site category, which delineates constituent atoms
    ISTable& atom_site = block.GetTable("atom_site");
	
    // Make a reference copy of the atom_site category table
    ISTable atom_site_ref = ISTable(atom_site);

    // Retrieve the table corresponding to the pdbx_struct_assembly_gen category, 
    // which details the generation of each macromolecular assembly
    ISTable& assembly_gen = block.GetTable("pdbx_struct_assembly_gen");
	
    // Retrieve the table corresponding to the pdbx_struct_oper_list category, which details 
    // rotation and translation operations required to generate/transform assembly coordinates
    ISTable& oper_list = block.GetTable("pdbx_struct_oper_list");

    // Use the CIF file pathname to prepare to generate the CIF file pathname for each assembly
    size_t extPos = cifName.find(".cif");


    // Count the number of assemblies
    vector<string> assemIds;
    for (unsigned int i = 0; i < assembly_gen.GetNumRows(); ++i) 
    {
      string id = assembly_gen(i, "assembly_id");

      for (unsigned j = 0; j < assemIds.size(); ++j) {
	if (assemIds[j] == id) 
	  continue;
      }
      assemIds.push_back(id);
    }

    // Create a CIF file for every assembly specified in pdbx_struct_assembly_gen
    for (unsigned int i = 0; i < assemIds.size(); ++i)
    {        
	// Create a new atom_site category table for this assembly
        atom_site = ISTable("atom_site");

        // Add the attribute/column names to the new atom_site category table
        vector<string> cols = atom_site_ref.GetColumnNames();
        for (unsigned int y = 0; y < cols.size(); ++y)
        {
            atom_site.AddColumn(cols[y]);
        }

	// Keep track of the current atom and model number
        unsigned int atomNum (1), modelNum (0);

	// Generate the CIF out file pathname using the value of the assembly_id attribute
        string outName = cifName.substr(0, extPos) + "-" + assemIds[i] + ".cif";

	// Retrieve the operation expression for this assembly from the oper_expression attribute
	for (unsigned int opId = 0 ; opId < assembly_gen.GetNumRows(); ++opId) 
	{
	    string id = assembly_gen(opId, "assembly_id");
	    // Skip operations not for this assembly
	    if (id != assemIds[i])
	    {
	        continue;
	    }

	    string operations = assembly_gen(opId, "oper_expression");
        
	    // Vectors to hold the individual operations 
	    vector<string> opers, opers2;

	    // Handles assemblies without parens (e.g., "1" or "1,2,3")
	    if (operations.find("(") == string::npos)
	    {   

	        // Pretend that this looks like a () operation and share code
	        string s = "(" + operations + ")";
	        parseOperationExpression(opers, s);
	    }

	    // Handles multiple operation assemblies, no Cartesian products (e.g., "(1-5)")
	    else if (operations.rfind("(") == 0)
	    {
	      parseOperationExpression(opers, operations);
	    }

	    // Handles Cartesian product expressions (e.g., "(X0)(1-60)")
	    else
	    {
	        // Break the oper_expression into two parenthesized expressions for parsing
	        size_t tempPos = operations.find_first_of(")");
		string firstHalf = operations.substr(0, tempPos + 1);
		string secondHalf = operations.substr(tempPos + 1);
               
		// Parse each expression, propagating opers and opers2
		parseOperationExpression(opers, firstHalf);
		parseOperationExpression(opers2, secondHalf);
	    }
        
	    // Retrieve the asym_id_list, which indicates which atoms to apply the operations to
	    string asym_id_list = assembly_gen(opId, "asym_id_list");

	    unsigned int temp = 1 > opers2.size() ? 1 : opers2.size();

	    // A matrix to hold the current rotation/translation operation	
	    matrix operation(boost::extents[4][4]);
	
	    // For every operation in the first parenthesized list
	    for (unsigned int j = 0; j < opers.size(); ++j)
	    {
	        // Find the index of the current operation in the oper_list category table
	        unsigned int operIndex = findOperationIndex(oper_list, opers[j]);
		
		// For every operation in the second parenthesized list
		for (unsigned int k = 0; k < temp; ++k)
		{
	            // Determine the index of the current operation in the second parenthesized list
	            int oper2Index (-1);
				
		    if (!opers2.empty())
		    {
		        oper2Index = findOperationIndex(oper_list, opers2[k]);
		    }

		    // Prepare the operation matrix
		    prepareOperation(oper_list, operation, operIndex, oper2Index);
			
		    vector<string> row;
                
		    string model = lexical_cast<string>(modelNum);

		    // Iterate over every atom in the atom_site reference table
		    for (unsigned int r = 0; r < atom_site_ref.GetNumRows(); ++r)
		    {
		        // If the asym_id of the current atom is not in the asym_id_list, skip to the next atom
		        if (asym_id_list.find(atom_site_ref(r, "label_asym_id")) == string::npos)
			{
		            continue;
			}
				
			// Retrieve the atom's row from the atom_site reference table
			row = atom_site_ref.GetRow(r);

			// Write out the current atom and model numbers
			row[1] = lexical_cast<string>(atomNum);
			row[25] = model;

			// Retrieve the coordinates for this atom
			vector<double> coords;
			coords.push_back(lexical_cast<double>(atom_site_ref(r, "Cartn_x")));
			coords.push_back(lexical_cast<double>(atom_site_ref(r, "Cartn_y")));
			coords.push_back(lexical_cast<double>(atom_site_ref(r, "Cartn_z")));
			coords.push_back(1.000);

					
			double sum;
			// Perform the matrix operation
			for (unsigned int a = 0; a < 3; ++a)
			{
			    sum = 0.0;
			    for (unsigned int b = 0; b < 4; ++b)
			    {
			        sum += (operation[a][b] * coords[b]);
			    }
			    // Write out the modified coordinate
			    row[10 + a] = boost::str(boost::format("%|.3f|") % sum);
			}
			// Add the modified row to the atom_site category table for this assembly
			atom_site.InsertRow(atomNum - 1, row);
			++atomNum;
		    }
		    ++modelNum;
	        }
	    }
	}
	// Write the atom_site table for this assembly
        block.WriteTable(atom_site);
		
	// Write out the CIF file for this assembly
	cifFileP->Write(outName);
    }
    return 0;
}

inline unsigned int findOperationIndex(ISTable& oper_list, const string& id)
{
    // Search the oper_list category table for the operation specified by id
    for (unsigned int i = 0; i < oper_list.GetNumRows(); ++i)
    {
        if (oper_list(i, "id") == id)
	    {
	        return i;
	    }
    }
    return 0;
}

inline void parseOperationExpression(vector<string>& ops, const string& expression)
{
    // Iterate over the operation expression
    for (unsigned int i = 1; i < expression.length() - 1; ++i)
    {
	// Read an operation
        unsigned int pos = i;
        while(expression[pos] != ',' &&
              expression[pos] != '-' &&
              expression[pos] != ')')
        {
            ++pos;
        }
        string currentOp = expression.substr(i, pos - i);
    
        // Handle single operations
        if (expression[pos] != '-')
        {
            ops.push_back(currentOp);
            i = pos;
        }    

	// Handle ranges
        else
        {
	    // Read in the range's end value
            ++pos;
            i = pos;
            while(expression[pos] != ',' &&
                  expression[pos] != ')')
            {
                ++pos;
            }
            // Add all the operations in [currentOp, end]
            unsigned int end = lexical_cast<unsigned int>(expression.substr(i, pos - i));
            for (unsigned int j = lexical_cast<unsigned int>(currentOp); j <= end; ++j)
            {
                ops.push_back(lexical_cast<string>(j));
            }
            i = pos;
        }
    }
}

inline void prepareOperation(ISTable& oper_list, matrix& operation, const unsigned int oper1, const int oper2)
{
    // Prepare matrices for operations 1 & 2
    matrix op1(boost::extents[4][4]), op2(boost::extents[4][4]);
    for (unsigned int i = 0; i < 4; ++i)
    {
        op1[3][i] = op2[3][i] = (i == 3) ? 1 : 0;
    }
	
    // Fill the operation matrices for operations 1 & 2
    for (unsigned int i = 0; i < 3; ++i)
    {
        string iStr = lexical_cast<string>(i + 1);

	op1[i][3] = lexical_cast<double>(oper_list(oper1, "vector[" + iStr + "]"));

        if (oper2 != -1)
	{
            op2[i][3] = lexical_cast<double>(oper_list(oper2, "vector[" + iStr + "]"));
	}
		
        for (unsigned int j = 0; j < 3; ++j)
	{
	    string jStr = lexical_cast<string>(j + 1);

            op1[i][j] = lexical_cast<double>(oper_list(oper1, "matrix[" + iStr + "][" + jStr + "]"));

	    if (oper2 != -1)
	    {
                op2[i][j] = lexical_cast<double>(oper_list(oper2, "matrix[" + iStr + "][" + jStr + "]"));
	    }
	}
    }

    // Handles non-Cartesian product expressions
    if (oper2 == -1)
    {
        operation = op1;
	    return;
    }

    // Handles Cartesian product expressions (4x4 matrix multiplication)
    double sum;
    for (unsigned int row = 0; row < 4; ++row)
    {
        for (unsigned int col = 0; col < 4; ++col)
        {
            sum = 0.0;
            for (unsigned int r = 0; r < 4; ++r)
            {
                sum += (op1[row][r] * op2[r][col]);
            }
            operation[row][col] = sum;
        }
    }
    return;
}
