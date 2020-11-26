#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
//#include <hex>

using namespace std;

bool isLabel(string label);
void createLabel(string label, int PC);
string instructionToBinary(string instruction);

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
};

//for storing the labels and location of the label
struct Labels
{
  string labelName;
  int labelPC;
};

int main()
{
  int PC = 0;
  int t0, t1, t2, t3, t4, t5, t6, t7;

  ifstream in1;
  string filename = "assembly.txt";

  string temp;
  bool checkedInstruction;

  vector<AssemblyLine> bitArray;
  vector<Labels> labelArray;


  //open file and check that it opened properly
  in1.open(filename);
  if (!in1)
  {
    cerr << "\nError: Bad file or filename" << endl;
    exit(1);
  }

  while(!in1.eof())
  {
    //makes empty line and places at back
    bitArray.push_back(AssemblyLine());

    //Bring in label, check it, and put it into the array
    in1 >> temp;
    checkedInstruction = isLabel(temp);
    if(checkedInstruction)
    {
      bitArray[PC].label = temp;
      in1 >> temp;
    }
    else if(isdigit(temp[0]) || temp.length() > 6)
    {
      cerr << "Error: Bad label on line " << PC << endl;
      exit(1);
    }

    temp = instructionToBinary(temp);

    cout << temp;

    //check if the op is valid
    if(temp == "label")
    {
      cerr << "Error: Bad operation on line " << PC << endl;
      exit(1);
    }

    if(bitArray[PC].instruction == "000" ||
       bitArray[PC].instruction == "001" ||
       bitArray[PC].instruction == "101")
    {
      // Bring in field0, field1, field2
      in1 >> bitArray[PC].field0;
      in1 >> bitArray[PC].field1;
      in1 >> bitArray[PC].field2;


      cout << bitArray[PC].field0 << " " << bitArray[PC].field1 << " " << bitArray[PC].field2;
    }

    else if(bitArray[PC].instruction == "010" ||
            bitArray[PC].instruction == "011" ||
            bitArray[PC].instruction == "100")
    {
      // Bring in field0, field1, field2
      in1 >> bitArray[PC].field0;
      in1 >> bitArray[PC].field1;
      in1 >> bitArray[PC].field2;


      cout << bitArray[PC].field0 << " " << bitArray[PC].field1 << " " << bitArray[PC].field2;        //BROKEN??????????????
    }

    else if(bitArray[PC].instruction == "110" ||
            bitArray[PC].instruction == "111")
    {
      //no fields
    }

    else if(bitArray[PC].instruction == ".fill")
    {
      in1 >> bitArray[PC].field0;
      //createLabel(bitArray[PC].label, bitArray[PC].field0);                 //  DOESN'T WORK. NEED TO FIGURE OUT STRING TO INT AND STUFF
    }

    //Gets whatever comments may be at the end of the line
    getline(in1, bitArray[PC].comment, '\n');

    cout << endl << endl << bitArray[PC].comment << endl << endl;


    //increment the program counter
    PC++;
  }



}

//checks if something is a label
bool isLabel(string label)
{
  if(instructionToBinary(label) == "label" && label.length() <= 6
     && !isdigit(label[0]))
    return true;
  else
    return false;
}

// //checks if meets the requirements of a label
// int labelCheck(label)
// {
//   if(!is_digit(label[0]) || label.length > 6 || !isLabel(label))
//   {
//     cerr << "Error: Bad label.\n"
//     exit(1);
//   }
// }

void createLabel(string label, int PC)
{
  return;
}

string instructionToBinary(string instruction)
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
  else if(instruction == ".fill")
  {
    //

    //            FILL IN WITH WHATEVER TF FILL DOES

    //
  }
  else           //For bad instruct error and isLabel check!
    instruction = "label";

  return instruction;
}

string toBinary (string instruction)
{
  if(instruction == "0")
    return "000";
  else if(instruction == "1")
    return "001";
  else if(instruction == "2")
    return "010";
  else if(instruction == "3")
    return "011";
  else if(instruction == "4")
    return "100";
  else if(instruction == "5")
    return "101";
  else if(instruction == "6")
    return "110";
  else if(instruction == "7")
    return "111";
  else
    return "invalid";
}
