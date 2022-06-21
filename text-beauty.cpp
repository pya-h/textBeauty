#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>

#define ON_INDENT_SPACES 4
#define ON_PUNCTUATE_SPACES 1
#define LINE_BREAKPOINT_LENGTH 90
#define ON_PARAGRAPH_EMPTY_LINES 1

using namespace std;

string readTextFileCompletely(string);
string beautifyText(string, int, int, int, int);
char capitalize(char);
char uncapitalize(char);
void punctuate(string& strTextRef, char sign, bool needsSpace);
bool isWritingMark(char);
void save(string strFilename, string strText);
bool possibleJunkCharacter(char ch);
bool isEndSentenceCharacter(char ch);
int extractOperand(string&, bool&);
double evaluate(string expression, bool& isMath);
bool isDigit(char);
bool isOperator(char);
void displayStatistics(string);

void horizontalLine() {
  cout<<endl<<"=================================================================="<<endl;
}

int main(int argc, char const *argv[]) {
  string filename = "";
  cout<< "Please enter a file name: ";
  try {

    filename = "test.txt";
    getline(cin, filename); // read file name (can contain spaces)

    string text = readTextFileCompletely(filename);
    
    horizontalLine();
    cout<< "Original File:\n" << text << endl;
    displayStatistics(text);
    string beautified = beautifyText(text, ON_PARAGRAPH_EMPTY_LINES, ON_INDENT_SPACES, ON_PUNCTUATE_SPACES, LINE_BREAKPOINT_LENGTH);
    horizontalLine();
    cout<< "Editted File:\n" << beautified <<endl ;
    int i = filename.length();
    for(; i >= 0 && filename[i] != '.'; i--); //seperating file actual name from its extension
    string actualFileName = filename.substr(0, i);
    save(actualFileName + ".beautified.txt", beautified);
    displayStatistics(beautified);
  }
  catch(ios_base::failure err){
    cout<<err.what()<<endl;
  }
  catch(invalid_argument err){
    cout<<err.what()<<endl;
  }
  horizontalLine();
  cout<< "The application has came to an end.\n press enter to continue...";
  getchar();
  return 0;

}

string readTextFileCompletely(string strFilename) {

  string strFileText = "", strLine = ""; //line is used for reading line by line
  ifstream fTextFile(strFilename);

  if(!fTextFile){
    fTextFile.close();
    throw ios_base::failure("App couldn\'t open the desired file. sure it exists?");
  }
  // using a loop together with the getline() function to read the file line by line
  while (getline (fTextFile, strLine)) {
    // Output the text from the file
    strFileText += strLine + "\n";
  }

  fTextFile.close();

  return strFileText;
}

void save(string strFilename, string strText){
  ofstream fTextFile(strFilename);

  fTextFile << strText;

  fTextFile.close();
}

string beautifyText(string strText, int onPragraphEmptyLines, int onIndentSpaces, int onPunctuateSpaces, int lineBreakpointLength){
  string indent = " ";
  string strSpaces = "";
  for(int c = 0; c < onPunctuateSpaces; c++)
      strSpaces += ' ';

  for(int i = 1; i < onIndentSpaces; indent += ' ', i++);

  string strNewText = indent; //first character tab for indeting the start of the text
  bool startOfTheSentence = false;

  for(int i = 0; strText[i]; i++){

    // first stage: skip unnecessary tabs/spaces
    while((strText[i] == '\t' || strText[i] == ' ' || i == 0) &&
      strText[i + 1] && (strText[i + 1] == '\t' || strText[i + 1] == ' ')) i++; // increase i to skip all unnecessary tabs

    // next stage: indent paragraphs or remove extra tabs/spaces/ or new lines at the start of lines
    if((strText[i] == '\n' || strText[i] == '\r') && strText[i + 1]){
      punctuate(strNewText, '.', false);

      // put determined empty lines between paragraphs
      for(int el = 0; el <= onPragraphEmptyLines; el++)
        strNewText += '\n';
      strNewText += indent; // indent new paragraph
      // next stage: skip unnecessary AFTER INDENT tabs/spaces
      // and remove extra empty lines in between
      while(strText[i + 1] && (strText[i + 1] == '\n' || strText[i + 1] == '\r' || strText[i + 1] == '\t' || strText[i + 1] == ' ')) i++; // increase i to skip all unnecessary tabs

      // inform the new sentence is starting
      startOfTheSentence = true;
    }
    else if(isWritingMark(strText[i])){
        punctuate(strNewText, strText[i], strText[i + 1] && (strText[i + 1] != ' ' && strText[i + 1] != '\t'));

        startOfTheSentence = isEndSentenceCharacter(strText[i]); //this characters end a sentence and start new one
    }
    else if(strText[i] != '\t' && strText[i] != ' '){ // copy non white character to newText or replace tabs in the middle of the text with single space
      if(isDigit(strText[i])){
        int j = i;
        string mathexp = "";
        while(isDigit(strText[j]) || isOperator(strText[j]) || strText[j] == ' ' || strText[j] == 't') {
          if(strText[j] != ' ' && strText[j] != 't' && strText[j] != '=') {
            mathexp += strText[j];
          }
          else if(isDigit(strText[j-1]) && isDigit(strText[j+1]))
            break;
          j++;
        }
        bool isMath = true;
        double result = evaluate(mathexp, isMath);
        if(isMath){
          string formattedEquation = "";
          for(int s = 0; mathexp[s] ;s++){
            if(isOperator(mathexp[s]))
              formattedEquation += strSpaces + mathexp[s] + strSpaces;
            else
              formattedEquation += mathexp[s];
          }
          strNewText += formattedEquation + strSpaces + "=" + strSpaces + to_string(result) + strSpaces;
          i = j - 1;//go to next loop again
          continue;
        }
      }
      if(!startOfTheSentence) // if its the first letter in the current sentence then convert it to capital case
        strNewText += uncapitalize(strText[i]) ; // if its not the start of sence or its not an alphabet character to capitalize , do this code
      else {
        strNewText += capitalize(strText[i]);
        startOfTheSentence = strText[i] < 'A' && strText[i] > 'z'; // if the shown char was alphabet => it was capitalized => Paragraph started correctly. otherwise next char must be caped
      }
    }
    else
      strNewText += ' ';

    //text wrap around
    int currenLineLength = 0, lastCharIndex = strNewText.length() - 1;
      if(strNewText[lastCharIndex] == ' ' || strNewText[lastCharIndex] == '\t' || isWritingMark(strNewText[lastCharIndex])){
      for(currenLineLength = 0; strNewText[lastCharIndex - currenLineLength] && strNewText[lastCharIndex - currenLineLength] != '\n'; currenLineLength++);
      if(currenLineLength >= lineBreakpointLength)
        strNewText += '\n';
    }
  }

  punctuate(strNewText, '.', false);
  return strNewText;
}

bool isEndSentenceCharacter(char ch){
  return ch == '.' || ch == '!' || ch == '?' || ch == ':' || ch == ';';
}
bool isWritingMark(char ch){
  return ch == '.' || ch == '!' || ch == ',' || ch == ':' || ch == ';' || ch == '-';
}

char capitalize(char ch){
  const char lowToHighCharacterConverter = 'A' - 'a';

  return ch >= 'a' && ch <= 'z' ? ch + lowToHighCharacterConverter : ch;
}

char uncapitalize(char ch){
  const char lowToHighCharacterConverter = 'a' - 'A';

  return ch >= 'A' && ch <= 'Z' ? ch + lowToHighCharacterConverter : ch;
}

bool possibleJunkCharacter(char ch){
  // characters that are not supposed to be at the end of the text r are supposed to be replaced by punctuate function
  return ch == '\t' || ch == ' ' || ch =='\n' || isWritingMark(ch);
}

void punctuate(string& strTextRef, char sign, bool isOriginalSign){
  // isOriginalSign: whether if the sign actually exists in the text or its a program choice to add one
  int end = strTextRef.length() - 1; //last character of the new string till now
  // remove junk empty lines pr spaces or \t at the end of the text just to prevent any bugs
  while(possibleJunkCharacter(strTextRef[end])) {
    strTextRef.erase(end);// remove junk char and then decrease end value by one
    end--;
  }
  //strTextRef[end + 1] = sign;

  if(!isEndSentenceCharacter(strTextRef[end])) // this code means: if at the end of current text there is another end sententence character => dont punctuate
    if(isWritingMark(strTextRef[end]))
      strTextRef[end] = sign; // if there is a sign like comma or dash at the end of the line => replace with better punctuation ( usually dot)
    else // if there is no writing sign at all, just add sign to text
      strTextRef += sign; // if there is

  if(isOriginalSign && sign != '-')
    for(int c = 0; c < ON_PUNCTUATE_SPACES; c++)
      strTextRef += ' ';

}

bool isDigit(char c) {
   return (c >= '0' && c <= '9');
}

int extractOperand(string& expression, bool& isMath){
  if(!isDigit(expression[0])){
    isMath = false;
    return 0;
  }
  try{
    int operand = expression[0] - '0';
    int i = 0;
    while(expression[++i] && isDigit(expression[i]))
      operand = operand * 10 + (expression[i] - '0');
    expression = expression.substr(i, expression.length());

    return operand;
  }
  catch(exception err){
    cout<< "Something went wrong while calculating math opertion... your string has wrong format!";
  }
  throw invalid_argument("invalid math expression detected");
}
bool isOperator(char ch){
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '^' || ch == 'r' || ch == '=';
}
// given expression is invalid.
double evaluate(string expression, bool& isMath)
{
    double result =  extractOperand(expression, isMath);
    // Base Case: Given expression is empty
    if (expression[0] == '\0')  return 0;
    // Traverse the remaining characters in pairs
    while(expression[0])
    {
      // The next character must be an operator, and
      // next to next an operand
      char _operator = expression[0];
      expression = expression.substr(1, expression.length());
      int operand = extractOperand(expression, isMath);
      if(!isMath) return 0.0;
      // Update result according to the operator
      if (_operator == '+')
        result += operand;
      else if (_operator == '-')
        result -= operand;
      else if (_operator == '*')
        result *= operand;
      else if (_operator == '/')
        result = double(result / operand);
      else if (_operator == '%')
          result = int(result) % operand;
      else if (_operator == '^')
          result = pow(result, operand);
      else if (_operator == 'r')
          result *= sqrt(operand);
      // If not a valid operator
      else {
        isMath = false;
        return 0;
      }
    }
    return result;
}

void displayStatistics(string text){
  int alpha = 0, words = 0, sentences = 0, paragraphs = 0;
  horizontalLine();
  int i =0;
  while(text[i] && (text[i] == '\t' || text[i++] == ' '));
  for(; text[i]; i++){
    bool isSign = false;
    if(text[i] >= 'A' && text[i] <= 'z') alpha++;
    else if(text[i] == ' ' || text[i] == '\t') {
      isSign = true;
      if(!isDigit(text[i - 1]) && !isOperator(text[i - 1]))
        words++;
    }
    else if(text[i] == '\n' || text[i] == '\r' ){
      isSign = true;

      if(!isDigit(text[i - 1]) && !isOperator(text[i - 1]))
      {
        words++;
        sentences++;
      }
      if(!text[i + 1] || ((text[i + 1] == '\r' || text[i + 1] == '\n') && (!text[i + 2] || (text[i + 2] == '\r' || text[i + 2] == '\n' || text[i + 2] == ' ' || text[i + 2] == '\t'))))
          paragraphs++;
    }
    else if(text[i] == '\0' || text[i] == EOF){
      isSign = true;
      if(!isDigit(text[i - 1])  && !isOperator(text[i - 1]))
        words++;
      sentences++;
      paragraphs++;
    }
    else if(isEndSentenceCharacter(text[i])){
      isSign = true;
      if(!isDigit(text[i - 1]) && !isOperator(text[i - 1]))
        words++;
      if(!isDigit(text[i - 1]) || (text[i + 1 ] && !isDigit(text[i + 1]))) // =>> 1.2
      {
        sentences++;
        if(!text[i + 1] || ((text[i + 1] == '\r' || text[i + 1] == '\n') && (!text[i + 2] || (text[i + 2] == '\r' || text[i + 2] == '\n' || text[i + 2] == ' ' || text[i + 2] == '\t'))))
          paragraphs++;
      }
    }
    else if(isWritingMark(text[i]) ){
      isSign = true;
      if(!isDigit(text[i - 1]) && !isOperator(text[i - 1]))
        words++;
    }
    // skip repeated junk characters
    if(isSign)
    {
      while(text[i + 1] == ' ' || text[i + 1] == '\t' ||
       text[i + 1] == '\n' || text[i + 1] == '\r' ||
        text[i + 1] == '\0' || text[i + 1] == EOF || isEndSentenceCharacter(text[i + 1]) || isWritingMark(text[i + 1]))
          i ++;
    }
  }

  cout<<endl<< "Text Statistics:"<<endl<<"\t";
  cout<< "Alphabets: " << alpha << "\t" << "Words: "<< words<<endl<<"\t";
  cout<< "Sentences: " << sentences << "\t" << "Paragraphs: "<< paragraphs << endl<<endl;

  horizontalLine();

}
