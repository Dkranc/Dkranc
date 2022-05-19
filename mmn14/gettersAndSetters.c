#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

/*this module has all the get and set functions for the program. we use this to reach data from diferent lists and variables*/


/*declaring all local functions*/
char* getFirstPara(char *line,int isLabel);
char* getSecondPara(char *line,int isLabel);
char* getLabelName(char *firstWord);
int getBaseAddress(int Dc);
int getOffsetAddress(int Dc);
char* getSecondWord(char *line);
int getAddressingType(char *firstPara);
int getFunctionCode(enum Func func);
int setOpCode(enum Func func);
int setFuncWord(enum Func func,int addressTypeOne,int addressTypeTwo,char *firstPara,char *secondPara);
int getRegiserNumber(int addressTypeOne,char *firstPara);
int getDirectNum(char *firstPara);
int twosComplement(int num);
int reverseBits( int num);
int getNewAddress(int oldAddress);
char* getWordVal(int val);
char* getHexVal(int num);
char* getLabelNameFromTypeTwo(char *firstPara);
int getOffsetValFromLabel(char *para,int address);
int getBaseValFromLabel(char *para,int address);
char* getMacroName(char *line);
char* getFirstWord(char *line);
char* getMacro(FILE *fBeforeMacros);
void  setLabelAddresses();



/*this method creates the function value for a word.this is the second command in a 2para or 1para function*/
int setFuncWord(enum Func func,int addressTypeOne,int addressTypeTwo,char *firstPara,char *secondPara){
 int ret=0;
 int mask=0;

 ret=A;/*assigning the A type to the word at the begining*/
 ret=ret|getFunctionCode(func);/*light the function bit*/
 
 /*lght the register number fro first parameter*/
 if(addressTypeOne>=2){
  mask=getRegiserNumber(addressTypeOne,firstPara);
  mask=mask<<8;
  ret=ret|mask;
 }

 /*light addressing type for first parameter*/
 mask=addressTypeOne;
 mask=mask<<6;
 ret=ret|mask;

 /*lght the register number fro second parameter*/
 if(addressTypeTwo>=2){
  mask=getRegiserNumber(addressTypeTwo,secondPara);
  mask=mask<<2;
  ret=ret|mask;
 }

 /*light addressing type for second parameter*/
 mask=addressTypeTwo;
 ret=ret|mask;

 return ret;/*the value is a number but we care about the first 20 bytes*/
}



/*this function return the first parameter from the line of assembler code*/
char* getFirstPara(char *line,int isLabel){
  char *para=(char*)malloc(MAXLABEL);
  int i=0;
  int c=0;
  
  /*reach the end of the label */
  if(isLabel==1){/*if we have a label before the name of he method*/   
     while(line[i]!=':') i++;
     i++;   
  }
   
  while(line[i]==' '||line[i]=='\t') i++;/*skip all white spaces*/
  
  i+=3;/*get to the white space after the method name. all method names are 3 letters long so this will give us the first white space after the method name*/

  while(line[i]==' '||line[i]=='\t') i++;/*skip all white spaces after method name*/

  while(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'&&line[i]!=','){/*copy the parameter name with the addressing method */
     para[c]=line[i];
     c++;
     i++;
  }

  if(c>0){/*if we found a parameter name we return it*/
    para[c]='\0';
    return para;
  } 
  else return NULL;/*we return null if no parameter was found. this is an edge case that is taken care of when recieving the first parameter*/
}



/*returns the second parameter from a line of assembler code*/
char* getSecondPara(char *line,int isLabel){
  char *para=(char*)malloc(MAXLABEL);
  int i=0;
  int c=0;
  

   /*cleaning the parameter*/
  for(;i<MAXLABEL;i++) para[i]=0;
  i=0;

  /*reach the end of the first comma  */
  while(i<strlen(line)&&line[i]!=',') i++;

  if(i==strlen(line)) return NULL; /*if we reached the end of the line and didnt find a comma*/ 
                    
  i++;  /*get the first white space or character*/ 
 
  while(line[i]==' '||line[i]=='\t') i++;/*skip all white spaces after comma*/
  
  /*copy the parameter name with the addressing type */
  while(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'){
    para[c]=line[i];
    c++;
    i++;
  }

  /*skip all white spaces after line to find extra text*/
  while(line[i]==' '||line[i]=='\t') i++;
  if(line[i]!='\n') return NULL;

  if(c>0){/*if we have a parameter to return*/
    para[c]='\0';
    return para;
  } 
  else return NULL;  
}



/*returns the name of a label from a word that looks like "this:". we just take off the : char*/
char* getLabelName(char *firstWord){
 char *label=(char *)malloc(strlen(firstWord)-1);
 int i=0;

 /*copy all characters besides the last one*/
 while(i<strlen(firstWord)-1){
     label[i]=firstWord[i];
     i++;
 }
 label[i]='\0';
 return label;
}



/*returns the base address of an address*/
int getBaseAddress(int Dc){
  int address=0;
  
  /*create the base address*/
  while(Dc>=16){
    address+=16;
    Dc-=16;
  }
return address;
}



/*returns the base address of an address*/
int getOffsetAddress(int Dc){
  
  /*create the offset address*/
  while(Dc>=16){
    Dc-=16;
  }
return Dc;
}



/*returns the name of a register form a type 2 addressing type*/
char* getRegisterName(char *para){
 
 int i=0;
 int c=0;
 char *reg=(char*)malloc(sizeof(char)*4);
 
 /*reach begening of the register name*/
 while (para[i]!='[') i++;
 i++;
 
 /*copy register name*/
 while (para[i]!=']'){/*we dont need to check for errors because it is called only after  we know it is a valid parameter*/
  reg[c]=para[i];
  i++;
  c++;
 }
 reg[3]='\0';
 
 return reg;
}



/*returns the second word of a line*/
char* getSecondWord(char *line){
 char *word=(char *)malloc(MAXLABEL);
 int i=0;
 int charCount=0;
 
 /*pass white space before words*/
 while(line[i]==' '||line[i]=='\t') i++;

 /*if were out of the array or if reached end of line*/
 if(i>=strlen(line)||line[i]=='\n') return NULL;

 /*reach the end of the first word*/
 while (line[i]!=' '&&line[i]!='\t') i++;

 /*if were out of the array or if reached end of line*/
 if(i>=strlen(line)||line[i]=='\n') return NULL;

 /*pass white space between words*/
 while(line[i]==' '||line[i]=='\t') i++;

 /*if were out of the array or if reached end of line*/
 if(i>=strlen(line)||line[i]=='\n') return NULL;

 /*copy the second word into the new variable*/
 while (line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'){
   word[charCount]=line[i];
   i++;
   charCount++;   
 }

 word[charCount]='\0';/*close the word with the end of string character*/
 return word;
}



/*gets a parameter and returns the addressing type*/
int getAddressingType(char *para){
 int ret=0;

 /*if the first character is # we know it is the first addressing type*/
 if(para[0]=='#') ret=0;

 /*check if it is the the index type of addressing*/
 else if(hasParantecies(para)==1){
  if(legalParanteciesAddressingType(para)==0) ret=2;/*if the Parantecies are legal and register inside them is between 10 to 15*/
  else ret=4;/*if there are parantecies but the register on parameter arent legal*/
 }

 /*check if type 3*/
 else if(isRegister(para)==1) ret=3;
 
 /*case 1. if its just  alabel*/
 else if(checkIfValidLabel(para,0)==0) ret=1;
 
 else ret=4;/*4 means we had an error with the parameter and it isnt legal*/

 return ret;
}



/*returns the number of the register in the method. we know the parameters are valid so we can use that in our favor*/
int getRegiserNumber(int addressType,char *firstPara){
int i;
char reg[3];
int c=0;
int ret=0;

 /*if the register name is in parantecies*/
 if(addressType==2){
    while(i<strlen(firstPara)&&firstPara[i]!='[') i++;
    i+=2;/*reach the first digit in the register*/
    
    /*copy the register number*/
    while(i<strlen(firstPara)&&firstPara[i]!=']'){
       reg[c]=firstPara[i];
       i++;
       c++;
     }
      
 }

 /*if the register is just a simple string*/
 else{
   i=1;
   /*copy number with out the r character*/
   while(i<strlen(firstPara)&&c<3){
     reg[c]=firstPara[i];
     i++;
     c++;
   }
   
 reg[c]='\0';

 ret=atoi(reg);/*convert number to int*/
 }
 return ret;
}



/*returns an int with the function number. the coresponding bit that symbols the functions will be 1*/
int getFunctionCode(enum Func func){
 int ret=0;

 if(func==2||func==5||func==9) ret=10;
 else if(func==3||func==6||func==10)  ret=11;
 else if(func==11||func==7)  ret=12;
 else if(func==8)  ret=13;


 ret=ret<<12;/*move the 1 digits to the right place in the new word*/

 return ret;
}



/*returns the binary value of the number in the parameter. saves the number in  2s complement method*/
int getDirectNum(char *firstPara){
 int num;
 char newNum[MAXLABEL];
 int i=1;
 int c=0;
 
 /*copy the number from the parameter*/
 for (i=1;i<strlen(firstPara);i++){
	newNum[c]=firstPara[i];
	c++;
 }
 newNum[c]='\0';
 num=atoi(newNum);/*convert the number*/

 /*if the number is too big or small. 50,000 is our error number */
 if(num>MAXNUM||num<MINNUM) num=50000;

 else num=twosComplement(num);/*turns the number to the right represting*/

 return num;
}



/*2s complement returns the number in a 16 bit long 2s complement type*/
int twosComplement(int num){  
 int mask;
 int fliped;
 int ret=A;

 /*if we have a positive number*/
 if(num>=0) return num;

 /*if the number is negetive we need to turn it to 2s complement*/
 else{
    num=-num;/*fliping the nmber to positive*/
    fliped=reverseBits(num); /*revers the first 16 bits*/
    fliped++;/*add one to the invert*/
    mask=1<<15;/*move the 1 to the sign bit so we know its negetive*/
    ret=ret|mask|fliped;
 }
return ret; 
}



/*reverse the first 16 bits of a number */
int reverseBits( int num){
 int  NO_OF_BITS = 17;
 unsigned int reverse_num;
    
 reverse_num=~num;/*invert all bits*/

 /*switch the 1ns to 0s, on the left side*/
 reverse_num= reverse_num<<NO_OF_BITS;/*shift left to get rid of all 16 left digits*/
 reverse_num= reverse_num>>NO_OF_BITS; /*shift right to make the first 16 bits 0*/

 return reverse_num;
}



/*finds a data node with a DC value, and returns the new address in the final word list*/
int getNewAddress(int oldAddress){
 int ret;

 /*iterate data list and find the data node with the same old DC value as the label address*/
 iteratorData=headData;
 while(iteratorData!=NULL){
  if(iteratorData->oldAddress==oldAddress) ret=iteratorData->valBase+iteratorData->valOffset;
  iteratorData=iteratorData->next;
 }
 return ret;
}



/*returns the string in the new type, shown by the special base for the object file*/
char* getWordVal(int val){
 char *word=(char *)malloc(sizeof(char));
 int mask=983040;/*we want the 4 last bits to be 1*/
 int num;
 int i=0;
 int c=0;
 char* charVal;
 char ch;

 /*create the string that is returnd*/
 for(;i<5;i++){
   /*light the 4 bits that we care about*/
   num=mask&val;
   num=num>>4*(4-i);
 
   /*get the hex value of the number*/
   charVal=getHexVal(num);
   ch=charVal[0];

   /*add a letter to the string according to the place in string*/
   switch(i){
     case 0: 
            word[c]='A';
            break;
     case 1: 
            word[c]='B';
            break;
     case 2: 
            word[c]='C';
            break;
     case 3: 
            word[c]='D';
            break;
     case 4: 
            word[c]='E';
            break;
   }   
   c++;
   word[c]=ch;
   c++;
   
   if(i!=4) { word[c]='-';
   c++; }

   mask=mask>>4; /*we want to shift mask 4 bits to the left untill it equals 0*/
 }
 return word;
}



/*returns the hex value of a number*/
char* getHexVal(int num){/*main issue*/
 char* ret=malloc(sizeof(char*));
 
   switch(num){
       case 0: ret[0]='0';
                break;
       case 1: ret[0]='1';
                break;
       case 2: ret[0]='2';
                break;
       case 3: ret[0]='3';
                break;
       case 4: ret[0]='4';
                break;
       case 5: ret[0]='5';
                break;
       case 6: ret[0]='6';
                break;
       case 7: ret[0]='7';
                break;
       case 8: ret[0]='8';
                break;
       case 9: ret[0]='9';
                break;
       case 10: ret[0]='a';
                break;
       case 11: ret[0]='b';
                break;
       case 12: ret[0]='c';
                break;
       case 13: ret[0]='d';
                break;
       case 14: ret[0]='e';
                break;
       case 15: ret[0]='f';
                break;
   }
 return ret;
}



/*returns the base address of a label*/
int getBaseValFromLabel(char *para,int address){
 int mask;
 int ret;

/*iterate the labels and find the right name*/
 iteratorLabel=headLabel;
 while( iteratorLabel!=NULL){
   if( strcmp(iteratorLabel->name,para)==0){/*if we found the label*/
     ret=iteratorLabel->valBase;
     if(iteratorLabel->entOrExt==ext){/*if the label is ext*/ 
       writeToExt(address,iteratorLabel->name,'B');/*we write to the ext file,B means its a base address*/
       mask=E;
       ret=mask|ret;
     }
     else{/*if the label isnt external*/
       mask=R;
       ret=mask|ret;
     }
   }
 
  iteratorLabel= iteratorLabel->next;
 }
 return ret;/*will return 0 if the label wasnt found, E if the label is extern and a R+baseValue otherwise*/
}



/*returns the offset address for label*/
int getOffsetValFromLabel(char *para,int address){
 int mask;
 int ret;

 /*iterate the labels*/
 iteratorLabel=headLabel;
 while(iteratorLabel!=NULL){
   if( strcmp(iteratorLabel->name,para)==0){/*if we found the label*/
     ret=iteratorLabel->valOffset;
     if(iteratorLabel->entOrExt==ext){/*if the label is ext*/
       writeToExt(address,iteratorLabel->name,'O');/*we write to the ext file. O for offset*/
       mask=E;
       ret=mask|ret;
     }
     else{/*the label is local and not external*/
       mask=R;
       ret=mask|ret;
     }
   }
 
  iteratorLabel= iteratorLabel->next;
 }
 return ret;/*will return 0 if the label wasnt found, E if the label is extern and a R+baseValue otherwise*/
}




/*returns the label name without []*/
char* getLabelNameFromTypeTwo(char *para){
 char *label=(char*)malloc(MAXLABEL);
 int i=0;
 int c=0;

  /*copy the parameter name with the addressing method */
  while(i<strlen(para)&&para[i]!='['){
     label[c]=para[i];
     c++;
     i++;
  }
  label[c]='\0';
  return label;
}



/*this method returns the name of the macro after we know that the line is declaring a new Macro*/
char* getMacroName(char *line){
 int s=0;
 int c=0;
 char *macroName=(char *)malloc(MAXMACRONAME * sizeof(char));/*we assume the length of the variable macro will be less then 20*/

 while(line[s]==' '||line[s]=='\t') s++;/*pass all white spaces*/
 while(line[s]!=' '&&line[s]!='\t') s++;/*pass the macro keyword*/
 while(line[s]==' '||line[s]=='\t') s++;/*pass all white spaces*/
 /*get the name of the macro*/
 while(line[s]!=' '&&line[s]!='\t'&&line[s]!='\n') {
  macroName[c]=line[s];   
  s++;
  c++;
 }
 macroName[c]='\0';
 return macroName;
}



/*returns the first word in the line*/
char* getFirstWord(char *line){
 int s=0;
 int c=0;
 char *firstWord=(char *)malloc(MAXMACRONAME * sizeof(char));/*we assume the length of the variable macro will be less then 31*/

 /*pass all white spaces*/
 while(line[s]==' '||line[s]=='\t') s++;


 /*copy the first word into the first word variable*/
 while(line[s]!=' '&&line[s]!='\t'&&line[s]!='\n') {
  firstWord[c]=line[s];   
  s++;
  c++;
 }
 firstWord[c]='\0';/*ending the word*/

 return firstWord;
}



/*this method returns the full macro declared by a macro statement */
char* getMacro(FILE *fBeforeMacros){
 char line[MAXLINE];/*81*/
 char *theMacroText=(char *)malloc(MAXMACRO * sizeof(char));/*we assume the length of the macro will be less then 20 lines*/
 
 fgets(line, MAXLINE-1, fBeforeMacros);/*recieve the next line from the file after the macro definitipn line*/
 strcpy(theMacroText,line);/*the macro will be at least one line*/
 fgets(line, MAXLINE-1, fBeforeMacros);/*recieving the next line*/

 /*untill we reach the endm keyword we fill up the macro text*/
 while(checkIfEndOfMacro(line)==1){
  strcat(theMacroText,line);
  fgets(line, MAXLINE-1, fBeforeMacros);/*get the next line from file*/
 }
 return theMacroText;
}



/*sets the addresses of each label to the right address*/
void  setLabelAddresses(){
 int ic;

 /*iterate the label list*/
 iteratorLabel=headLabel;
 while(iteratorLabel!=NULL){
  /*find the old address and set the new one*/
  if(iteratorLabel->type==data){
  ic=getNewAddress(iteratorLabel->oldAddress);
  iteratorLabel->valDec=ic;
  iteratorLabel->valBase=getBaseAddress(ic);/*setting the new address according to the datas new address*/
  iteratorLabel->valOffset=getOffsetAddress(ic);
  }
   iteratorLabel=iteratorLabel->next;
 }
}
