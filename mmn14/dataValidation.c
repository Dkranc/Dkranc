#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

/*declare local functions that deal with special cases in code*/
int checkIfLabel(char *line);
int checkIfComment(char *line);
int checkIfValidLabel(char *firstWord,int newDef);
int checkLegalEntry(char *line,int isLabel);
int checkIfValidLine(char *line, int islabel);
int legalParanteciesAddressingType(char *para);
int checkIfValidLineNoPara(char *line,int isLabel);


/*checks if a word is a label decleration*/
int checkIfLabel(char *firstWord){
 int ans=0;
 int i=0;

 i=strlen(firstWord)-1;/*point to the last char in the word*/
 /*we check that the first word ends with :*/
 if(firstWord[i]==':') ans=1;
 return ans;
}


/*checks if a line is a comment line*/
int checkIfComment(char *line){
 int ans=0;
 int i=0;

 /* skip white spaces */
 while (line[i]==' '||line[i]=='\t') i++;

 if(line[i]==';') ans=1;/*if the first char after the wihte spaces was ; */
 return ans;
}



/*checks if the label name is legal*/
int checkIfValidLabel(char *label,int newDef){
 int i=0;

 if(isalpha(label[0])==0) return 1; /*if the first letter isnt a letter*/

 else if(!strcmp(label,"r1")||!strcmp(label,"r2")||!strcmp(label,"r3")||!strcmp(label,"r4")||!strcmp(label,"r5")||!strcmp(label,"r6")||!strcmp(label,"r7")||
  !strcmp(label,"r8")||!strcmp(label,"r9")||!strcmp(label,"r10")||!strcmp(label,"r11")||!strcmp(label,"r12")||!strcmp(label,"r13")||!strcmp(label,"r14")||
  !strcmp(label,"r15")||!strcmp(label,".entry")||!strcmp(label,".extern")||!strcmp(label,".data")||!strcmp(label,".string")||!strcmp(label,".entry")||
  !strcmp(label,"mov")||!strcmp(label,"cmp")||!strcmp(label,"add")||!strcmp(label,"sub")||!strcmp(label,"lea")||!strcmp(label,"clr")||!strcmp(label,"not")||
  !strcmp(label,"not")||!strcmp(label,"inc")||!strcmp(label,"dec")||!strcmp(label,"jmp")||!strcmp(label,"bne")||!strcmp(label,"jsr")||!strcmp(label,"red")||
  !strcmp(label,"prn")||!strcmp(label,"rts")||!strcmp(label,"stop")) return 1;/*if the word is a saved word from the language*/

 else if(partOfList(label)==1&&newDef==1) return 1;/*if were trying to create a new label we need to check the name dosent allready exist*/

 else{
   for(i=0;i<strlen(label);i++){
     if(isdigit(label[i])==0&&isalpha(label[i])==0) return 1; /*if we have a non number or non alphabetic character*/
   }
 }

return 0;/*this is the default return. if an error appeard then we wont reach this*/
}



/*checks if the line that has a entry statement is legal*/
int checkLegalEntry(char *line,int isLabel){
 int errorFound=0;
 int i=0;
 char* labelName=(char *)malloc(MAXLABEL);
 int charCount=0;
 
 /*reach end of label if needed*/
 if (isLabel==1)
 while(i<strlen(line)&&line[i]!=':') i++;/*reach end of label*/

 i++;/*get first char after label*/

 while(i<strlen(line)&&line[i]!='y') i++;/*reaching end of entry statement*/
 i++;/*we want to have the first white space*/

 while (line[i]==' '||line[i]=='\t') i++;/*skip white spaces*/

 /*find the labels name*/
 while(i<strlen(line)&&line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'){
	if(charCount<MAXLABEL){
                labelName[charCount]=line[i];
		i++;
		charCount++;              
	}
        else errorFound=1;
 }
 labelName[charCount]='\0';
 
 while (line[i]==' '||line[i]=='\t') i++;/*finish reading line to find errors*/
 
 if (charCount==0||line[i]!='\n') {/*if no label name was specified or if we found extra text*/
  errorFound=1;
  printf("Ilegal entry statment\n");
 }
 else if(checkIfValidLabel(labelName,0)==1){/*we send 0 because its not a new label*/
     printf("Ilegal label name: %s\n",labelName);
     errorFound=1;;
 }
 return errorFound;
}



/*checks if line is  valid for a single parameter command*/
int checkIfValidLine(char *line, int isLabel){
 int i=0;
  
 /*skip label name if needed*/ 
 if(isLabel==1){/*if we have a label before the name of the method*/
    while(line[i]!=':') i++;
    i++;   
 }
   
 while(i<strlen(line)&&(line[i]==' '||line[i]=='\t')) i++;/*skip all white spaces*/
  
 i+=3;/*get to the white space after the method name. all method names are 3 letters long so this will give us the first white space after the method name*/

 while(i<strlen(line)&&(line[i]==' '||line[i]=='\t')) i++;/*skip all white spaces after method name*/

 while(i<strlen(line)&&line[i]!=' '&&line[i]!='\t'&&line[i]!='\n') i++;  /*pass all of the first parameter*/

 while(i<strlen(line)&&(line[i]==' '||line[i]=='\t')) i++;/*skip all white spaces after line to find extra text*/
 
 if(i>=strlen(line)||line[i]!='\n') return 1; /*if we found a char that isnt the end of the line after the parameter name*/
  
 else return 0;  
}



/*check valid line for no parameter function*/
int checkIfValidLineNoPara(char *line,int isLabel){
  int i=0;
  
  if(isLabel==1){/*if we have a label before the name of the method*/
                              /*reach the end of the label */
     while(line[i]!=':') i++;
     i++;   
  }
   
  while(i<strlen(line)&&(line[i]==' '||line[i]=='\t')) i++;/*skip all white spaces*/
  
  while(i<strlen(line)&&(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n')) i++;/*get to the white space after the method name. all method names are 3 letters long 
                                                                            so this will give us the first white space after the method name*/

  while(i<strlen(line)&&(line[i]==' '||line[i]=='\t')) i++;/*skip all white spaces after line to find extra text*/
  if(i>=strlen(line)||line[i]!='\n') return 1; /*if we found a char that isnt the end of the line after the parameter name*/
  
  else return 0;  
}



/*checks the type 2 addressing type*/
int legalParanteciesAddressingType(char *para){
 int i=0;
 char *reg=(char*)malloc(sizeof(char)*4);/* a register can be 3 chars long*/
 int c=0;
 
 /*nill the array*/
 for(c=0;c<3;c++) reg[c]=0;

 while (i<strlen(para)&&para[i]!='[') i++;/*we know that there is at leats one [ becuase thats why we are in the method. it is called only after w efound a [*/
 
 i++;/*first char in register name*/
 c=0;
 
 /*copying the register in the [] to reg */
 while (i<strlen(para)&&c<3&&para[i]!=']'){
  reg[c]=para[i];
  c++;
  i++;
 }
 reg[c]='\0';

/*if ther was no ] after label name or if the length of register type wasnt legal or if the register isnt legal*/
 if(i>=strlen(para)||(strcmp(reg,"r10")!=0&&strcmp(reg,"r11")!=0&&strcmp(reg,"r12")!=0&&strcmp(reg,"r13")!=0&&strcmp(reg,"r14")!=0&&strcmp(reg,"r15")!=0)) return 1;

 return 0;/*if the parameter is legal*/
}


/*reads a line that has numbers in it and checks if the line is legal or not*/
int checkLegalData(char *line,int isLabel){
 int maxLength=6;
 int i=0;
 int counter=0;
 int comma=0;

 /*skip label if needed*/
 if(isLabel==1) {
    while(i<strlen(line)&&line[i]!=':') i++;
    i++;/*first char after the :*/
 }

 while(i<strlen(line)&&line[i]!='t') i++;
 i+=2;/*reach first char after .data*/

 while(i<strlen(line)&&(line[i]=='\t'||line[i]==' ')) i++;
 
 if(i==strlen(line)||line[i]=='\n'||(isdigit(line[i])==0&&line[i]!='-'&&line[i]!='+')) return 1;/*if we have a comma before any number or if weve reache dthe 
                                                                                                        end of the line and no data apeard*/
 while(i<strlen(line)&&line[i]!='\n') {

     /*if we have a white space*/
     if(line[i]==' '||line[i]=='\t') { 
       if(counter>0){/*if we found a white space after a number started we want to see if a comma is the next char , if not the number is ilegal*/
		while(line[i]==' '||line[i]=='\t') i++;
                if(line[i]!=','&&line[i]!='\n') return 1; /*if we found the char and its not a comma or the end of a line we return 1 as an error*/       
       } 
      i++;
      counter=0;
     }

     /*if we have a digit ot minus or plus*/
     else if(isdigit(line[i])>0||line[i]=='-'||line[i]=='+'){
        if((line[i]=='-'||line[i]=='+')&&counter>0) return 1;/*if we got a + or - in the middle of a nummber */
	counter++;
        if(counter>maxLength) return 1;/*if we have to many digits in the muber*/
        comma=0;
        i++;  
     }

     /*if we have a comma*/
     else if(line[i]==','){
      if (comma==1) return 1;/*if we have 2 commas one after the other*/
      comma=1;
      counter=0;
      i++;   
     }
   
     /*ilegal char*/
     else return 1; /*if the character isnt a white space, isnt a +,-,comma or number. that means its ilegal*/         
 }/*end of while*/

 if (counter==0&&comma==1) return 1;/*if we reached the end of the line and found a comma or if no data was collected*/ 

 return 0;
}



/*checks if a line has text in it*/
int hasText(char *line){
 int i=0;
 int ret=1;
 
 /*reach end of line*/
 while(line[i]!='\n'){
  if(line[i]!=' '&&line[i]!='\t') ret=0;/*if we have a non white space char*/
  i++;
 }
 return ret;
}
