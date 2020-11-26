/*
~Instructions~
1. Run g++ -std=c++11 Project1New.cpp
2. Run a.out (inputFile) (outputFile)

Note: test cases are labeled t(1-7)

*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <bitset>

using namespace std;

//for holding an entire line of assembly code
struct AssemblyLine
{
  string label;
  string instruction;
  string field0;
  string field1;
  string field2;
  string comment;
  int PC;
  string binary;
  bool beqlabel = false;
};


void toBinary(string&);
void fileRead(vector<AssemblyLine> &bitArray, int &PC, char * argv[]);
void instructionToBinary(string &instruction);
void fieldToBinary(string &field);
void labelToAddress(string &label, vector<AssemblyLine> &bitArray, const int i);
void fieldLabelHandler(vector<AssemblyLine> &bitArray);
void theConcatenator(vector<AssemblyLine> &bitArray);


int main(int argh, char * argv[])
{
  //declaring registers
  int r0 = 0;
  int r1, r2, r3, r4, r5, r6, r7;

  //The program counter
  int PC = 0;

  //vectors for storing lines of assembly code and labels with their pc
  vector<AssemblyLine> bitArray;


  ofstream out;
  out.open(argv[2]);

  if(!out)
  {
    cerr << "Could not open requested output file";
    exit(1);
  }

  // reads the file and fills the bitArray
  fileRead(bitArray, PC, argv);

  //goes through the bitArray to find labels in the fields
  fieldLabelHandler(bitArray);

  //starts converting everything to binary, line by line
  for(int i = 0; i < PC; i++)
  {
    //translates opcode to binary
    instructionToBinary(bitArray[i].instruction);

    //translates fields to binary for R-Type instructions
    if(bitArray[i].instruction == "000" || bitArray[i].instruction == "001" ||
       bitArray[i].instruction == "101")
    {
      fieldToBinary(bitArray[i].field0);
      fieldToBinary(bitArray[i].field1);
      fieldToBinary(bitArray[i].field2);
    }

    //translates fields to binary for I-Type instructions
    else if(bitArray[i].instruction == "010"||bitArray[i].instruction == "011")
    {
      fieldToBinary(bitArray[i].field0);
      fieldToBinary(bitArray[i].field1);

      //checks to make sure offset is valid
      if(stoi(bitArray[i].field2) < -32768 || stoi(bitArray[i].field2) > 32767)
      {
        cerr << "Error: Invalid offset field on line " << i;
        exit(1);
      }

      toBinary(bitArray[i].field2);
    }

    //translates fields to binary for beq
    else if(bitArray[i].instruction == "100")
    {
      fieldToBinary(bitArray[i].field0);
      fieldToBinary(bitArray[i].field1);

      //checks to make sure offset is valid
      if(stoi(bitArray[i].field2) < -32768 || stoi(bitArray[i].field2) > 32767)
      {
        cerr << "Error: Invalid offset field on line " << i;
        exit(1);
      }

      //calculates offset in decimal
      if(bitArray[i].beqlabel)
        bitArray[i].field2 = to_string(stoi(bitArray[i].field2) - bitArray[i].PC - 1);

      toBinary(bitArray[i].field2);
    }
  }


  //checks to make sure we have no duplicate labels
  for(int i = 0; i < bitArray.size() - 1; i++)
  {
    if(!bitArray[i].label.empty())
    {
      for(int j = 0; j < bitArray.size() - 1; j++)
      {
        if(bitArray[i].label == bitArray[j].label && i != j)
        {
          cerr << "Error: Duplicate label on line " << i << endl;
          exit(1);
        }
      }
    }
  }


  theConcatenator(bitArray);


  //prints the output to the file
  for(int i = 0; i < bitArray.size(); i++)
  {
    if(bitArray[i].instruction != ".fill")
    {
      //converts to decimal then to binary
      bitset<32> bitsetBinary (bitArray[i].binary);
      out << bitsetBinary.to_ulong();
    }
    else
      out << bitArray[i].field0;

    if(i != bitArray.size() - 1)
      out << endl;

  }





  exit(0);
}


// function to convert decimal to binary
void toBinary(string& binary)
{
  //converts the string decimal to an int
  int decimal = stoi(binary);

  //converts the int decimal into a bitset
  bitset<16> bitsetBinary (decimal);

  //converts the bitset back to a string, but now in 2's complement
  binary = bitsetBinary.to_string();

  return;
}


void fileRead(vector<AssemblyLine> &bitArray, int &PC, char * argv[])
{
  ifstream in;
  in.open(argv[1]);

  //makes sure the input file opens
  if (!in)
  {
    cerr << "\nError: Bad input file or filename" << endl;
    exit(1);
  }

  // Reads the file
  while (!in.eof()) {
      //makes empty spot to put new stuff in at back of vector
      bitArray.push_back(AssemblyLine());

      //begins reading
      string input;
      in >> input;

      //checks for add, nand, lw,, sw, beq, and cmov, since they are all the
      //same for input
      if (input == "add" || input == "nand" || input == "lw" ||
          input == "sw" || input == "beq" || input == "cmov")
      {
        bitArray[PC].instruction = input;
        in >> bitArray[PC].field0 >> bitArray[PC].field1 >> bitArray[PC].field2;
        getline(in, bitArray[PC].comment, '\n');
        bitArray[PC].PC = PC;
      }

      //checks for .fill instruction
      else if (input == ".fill")
      {
          bitArray[PC].instruction = input;
          in >> bitArray[PC].field0;
          getline(in, bitArray[PC].comment, '\n');
      }

      //checks for halt and noop
      else if (input == "halt" || input == "noop")
      {
        bitArray[PC].instruction = input;
        bitArray[PC].PC = PC;
      }

      //catches labels
      else
      {
        bitArray[PC].label = input;

        if(bitArray[PC].label.size() > 6 || isdigit(bitArray[PC].label[0]))
        {
          cerr << "Bad label on line " << PC << endl;
          exit(1);
        }

        //checks if operation is a .fill
        in >> input;
        if (input == ".fill")
        {
          bitArray[PC].instruction = input;
          in >> bitArray[PC].field0;
        }

        //checks if operation is halt
        else if(input == "halt")
        {
          bitArray[PC].instruction = input;
          bitArray[PC].PC = PC;
        }

        //any other instruction goes here
        else
        {
            bitArray[PC].instruction = input;
            in  >> bitArray[PC].field0 >> bitArray[PC].field1
                >> bitArray[PC].field2;
        }

        getline(in, bitArray[PC].comment, '\n');
        bitArray[PC].PC = PC;
      }

      //increments the PC
      PC++;
  }
}

//translates the instruction to binary
void instructionToBinary(string &instruction)
{
  if(instruction == "add")
    instruction = "000";
  else if(instruction == "nand")
    instruction = "001";
  else if(instruction == "lw")
    instruction = "010";
  else if(instruction == "sw")
    instruction = "011";
  else if(instruction == "beq")
    instruction = "100";
  else if(instruction == "cmov")
    instruction = "101";
  else if(instruction == "halt")
    instruction = "110";
  else if(instruction == "noop")
    instruction = "111";

  return;
}

//translates the field to binary
void fieldToBinary(string &field)
{
  //converts the string decimal to an int
  int decimal = stoi(field);

  //converts the int decimal into a bitset
  bitset<3> bitsetBinary (decimal);

  //converts the bitset back to a string, but now in 2's complement
  field = bitsetBinary.to_string();

  return;
}

//converts the given label to an address
void labelToAddress(string &label, vector<AssemblyLine> &bitArray, const int i)
{
  //checks for the label in the bitArray
  int j = 0;
  while (j < bitArray.size())
  {
    //if found, sets the label to be the PC stored for the label
    if(label == bitArray[j].label)
    {
      label = to_string(bitArray[j].PC);
      break;
    }

    j++;
  }

  //checks to make sure the label was appropriately decoded
  if(isalpha(label[0]))
  {
    cerr << "\nError: Unknown label on line " << i << endl;
    exit(1);
  }

  return;
}

//if a label is found, converts the label to the address
void fieldLabelHandler(vector<AssemblyLine> &bitArray)
{
  //cycles through the bitArray to find labels
  for(int i = 0; i < bitArray.size(); i++)
  {
    //if label is found, checks for match, then applies appropriate address
    //Checks all instances of field0
    if(isalpha(bitArray[i].field0[0]))
    {
      labelToAddress(bitArray[i].field0, bitArray, i);
      bitArray[i].beqlabel = true;
    }

    //Checks all instances of field1
    if(isalpha(bitArray[i].field1[0]))
    {
      labelToAddress(bitArray[i].field1, bitArray, i);
      bitArray[i].beqlabel = true;
    }

    //Checks all instances of field2
    if(isalpha(bitArray[i].field2[0]))
    {
      labelToAddress(bitArray[i].field2, bitArray, i);
      bitArray[i].beqlabel = true;
    }
  }

  return;
}

void theConcatenator(vector<AssemblyLine> &bitArray)
{
  for(int i = 0; i < bitArray.size(); i++)
  {
    //puts in the unused bits and the opcode
    bitArray[i].binary = "0000000" + bitArray[i].instruction;

    //handles R-type instructions
    if(bitArray[i].instruction == "000" || bitArray[i].instruction == "001" ||
       bitArray[i].instruction == "101")
    {
      bitArray[i].binary += bitArray[i].field0;
      bitArray[i].binary += bitArray[i].field1;
      bitArray[i].binary += "0000000000000";
      bitArray[i].binary += bitArray[i].field2;
    }

    //handles I-type instructions
    else if(bitArray[i].instruction == "010" || bitArray[i].instruction == "011"||
            bitArray[i].instruction == "100")
    {
      bitArray[i].binary += bitArray[i].field0;
      bitArray[i].binary += bitArray[i].field1;
      bitArray[i].binary += bitArray[i].field2;
    }

    //handles O-type instructions
    else if (bitArray[i].instruction == "110" || bitArray[i].instruction == "111")
    {
      bitArray[i].binary += "0000000000000000000000";
    }
  }
}
