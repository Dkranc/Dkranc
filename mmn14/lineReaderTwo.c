#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

/*declaring local functions*/
int readLineTwo(char *line);
int dealWithEntry(char *line,int isLabel);
int updateEntry(char *labelName);
int fillValues(int addressTypeOne,int adressTypeTwo, int address,char *firstPara,char *secondPara);
void nillAddress();

/*global variables*/
int address=100;


/*turns address to 100*/
void nillAddress(){
 address=100;
}


/*this is the second read line function. it read the line for the second time and adds values to the instruction words that were created but didnt have values*/
int readLineTwo(char *line){
 char *firstWord;
 char *secondWord;
 int errorFound=0;
 int isLabel=0;
 int addressingTypeOne;
 int addressingTypeTwo;
 char *firstPara;
 char *secondPara;
 enum Func func=-1;/*setting the func to -1 to be ready to accept a new function*/

 /*if it a comment line we skip the line*/
 if(checkIfComment(line)==1){/*we want to skip the line if its a comment*/
  return errorFound;
 }
 /*get the first and second word*/
 firstWord=getFirstWord(line);
 secondWord=getSecondWord(line);/*used to get the next word so we know what to after the label*/

 /*if we have a label defined in our line we raise a flag*/
 if(checkIfLabel(firstWord)==1){
   isLabel=1;
 } 

 /*check that both words are valid*/
 if(firstWord==NULL||secondWord==NULL) {
   if(secondWord==NULL&&(strcmp(firstWord,"stop")==0||strcmp(firstWord,"rts")==0)) {
         errorFound=0;/*no problem found*/
         address++;/*we want to get the next word. stop and rts take up only one word*/
   }
   else  errorFound=1;/*if the second word was null and first one wasnt stop or rts , or if the first word was null*/
  }
 
  /*finding the method that the user wnats to use*/
  else if(strcmp(firstWord,".data")==0||strcmp(firstWord,".string")==0||strcmp(secondWord,".data")==0||strcmp(secondWord,".string")==0){
    /*we took care of this in first read. so we continue to the next line*/
    return errorFound;/*we skip the line if its data*/
  }

  else if(strcmp(firstWord,".extern")==0||strcmp(secondWord,".extern")==0){
   return errorFound;/*we skip the line if its extern*/
 }

  else if(strcmp(firstWord,".entry")==0||strcmp(secondWord,".entry")==0){
    errorFound= dealWithEntry(line,isLabel);/*send the line to the Entry function*/
    if(errorFound==1) printf("ilegal entry statement: %s\n",line);
 }

  /*find the rigth function and assign func to the currect value*/
  else if(strcmp(firstWord,"mov")==0||strcmp(secondWord,"mov")==0)   func=mov;

  else if(strcmp(firstWord,"cmp")==0||strcmp(secondWord,"cmp")==0)   func=cmp;

  else if(strcmp(firstWord,"add")==0||strcmp(secondWord,"add")==0)   func=add;

  else if(strcmp(firstWord,"sub")==0||strcmp(secondWord,"sub")==0)   func=sub;

  else if(strcmp(firstWord,"lea")==0||strcmp(secondWord,"lea")==0)   func=lea;

  else if(strcmp(firstWord,"clr")==0||strcmp(secondWord,"clr")==0)   func=clr;

  else if(strcmp(firstWord,"not")==0||strcmp(secondWord,"not")==0)   func=not;

  else if(strcmp(firstWord,"inc")==0||strcmp(secondWord,"inc")==0)   func=inc;

  else if(strcmp(firstWord,"dec")==0||strcmp(secondWord,"dec")==0)   func=dec;

  else if(strcmp(firstWord,"jmp")==0||strcmp(secondWord,"jmp")==0)   func=jmp;

  else if(strcmp(firstWord,"bne")==0||strcmp(secondWord,"bne")==0)   func=bne;

  else if(strcmp(firstWord,"jsr")==0||strcmp(secondWord,"jsr")==0)   func=jsr;

  else if(strcmp(firstWord,"red")==0||strcmp(secondWord,"red")==0)   func=red;

  else if(strcmp(firstWord,"prn")==0||strcmp(secondWord,"prn")==0)   func=prn;

 /*add the values to the words that were created*/
 if(func>=0&&func<=4){/*two para function*/
          /*get the parameters*/
          firstPara=getFirstPara(line,isLabel);
          secondPara=getSecondPara(line,isLabel);

          if(firstPara==NULL||secondPara==NULL) return 1;/*make sure that both parameters are legal and line is ok*/

          /*get addressing types so we know how many words to fix*/
          addressingTypeOne=getAddressingType(firstPara);
          addressingTypeTwo=getAddressingType(secondPara);
        
          /*send the address the parameters and addressing type to be added to the word list*/
          if (errorFound==0) errorFound=fillValues(addressingTypeOne,addressingTypeTwo,address,firstPara,secondPara);

          /*now that we know addressing types we can get the address .*/
          address+=2;/*we have 2 words at least for each word*/
          address+=addNumOfWordsByAddressingType(addressingTypeOne);
          address+=addNumOfWordsByAddressingType(addressingTypeTwo);
 }

 else if(func>=5&&func<=13)  {/*one parameter function*/
          firstPara=getFirstPara(line,isLabel);             
          if(firstPara==NULL) return 1;/*make sure that both parameters are legal and line is ok*/

          /*get addressing types so we know how many words to fix*/
          addressingTypeOne=getAddressingType(firstPara);

          if (errorFound==0) errorFound=fillValues(addressingTypeOne,3,address,firstPara,NULL);/*we send 3 so we know in the fillvalues method that we dont need 
                                                                                                   to use the second address type, and we send null as second
                                                                                                    parameter so we dont need use it.*/
          /*now that we know addressing types we know how many words in memory we need to allocate.*/
          address+=2;/*we have 2 words at least for each word*/
          address+=addNumOfWordsByAddressingType(addressingTypeOne);      
 }
return errorFound;
}


/*finds the label name that needs to be updated and sets the entry value to true*/
int dealWithEntry(char *line,int isLabel){
 int errorFound=0;
 char labelName[MAXLABEL];
 int i=0;
 int charCount=0;
 
 /*reach the beging of the label name*/
 if (isLabel)
 while(i<strlen(line)&&line[i]!=':') i++;/*reach end of label*/

 i++;/*get first char after label*/

 while(line[i]!='y') i++;/*reaching end of entry statement*/
 i++;/*we want to have the first white space*/

 while (line[i]==' '||line[i]=='\t') i++;/*skip white spaces*/

/*get the label name that we need to update*/
 while(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'){
	if(charCount<MAXLABEL){
		labelName[charCount]=line[i];
		i++;
		charCount++;
	}
        else errorFound=1;
 }
 labelName[charCount]='\0';

 while (line[i]==' '||line[i]=='\t') i++;/*finish reading line to find errors*/
 
 if (charCount==0||line[i]!='\n') {
  errorFound=1;
  printf("Ilegal entry statment");
 }

 if (errorFound==0) errorFound=updateEntry(labelName);/*we send the label name to be updated in the label list*/

 return errorFound;
}



/*updates the label name to have the entry field*/
int updateEntry(char *labelName){
 int found=1;

 /*iterate the label list*/
 iteratorLabel=headLabel;
  while(iteratorLabel!=NULL){
   if(strcmp(iteratorLabel->name,labelName)==0) {/*if we found the label name*/
      found=0;
      iteratorLabel->entOrExt=ent; 
   }
   iteratorLabel=iteratorLabel->next;
  }
   if (found==1) printf("label could not be found: %s\n",labelName);/*if the label wasnt found*/

  return found;/*we return 0 if found and 1 if the label wasnt found */
}


/*this method updates the instruction with the address number we got in the function, according to the parameter types and values*/
int fillValues(int addressTypeOne,int addressTypeTwo, int address,char *firstPara,char *secondPara){
 int val=0; 
 struct Word* ptrWord;

 /*iterate the word array to find the word with right address*/
 iteratorWord=headWord;
 while( iteratorWord!=NULL){
  if( iteratorWord->valBase+ iteratorWord->valOffset==address){/*if we found the word we are looking for*/
      ptrWord=iteratorWord;/*now ptr is pointed to the opcode word of the instruction*/
  }
  iteratorWord=iteratorWord->next;
 } 

 ptrWord=ptrWord->next;/*point to the function word*/
 ptrWord=ptrWord->next;/*point to the word after the function, meaning to the first word that holds a parameter location*/

 /*if the first parameter is a label*/
 if(addressTypeOne==1||addressTypeOne==2){
   if(addressTypeOne==2) firstPara=getLabelNameFromTypeTwo(firstPara);/*if we have type two of addressing we need to get the name of the label*/ 
   val=getBaseValFromLabel(firstPara,address);/*setting the first word witch is the base val of the label*/
   if(val==0) return 1;
   ptrWord->val=val;
   
   ptrWord=ptrWord->next;/*point to the word for offset val of label*/
   val=getOffsetValFromLabel(firstPara,address);/*setting the first word witch is the base val of the label*/
   if(val==0) return 1;
   ptrWord->val=val;
   ptrWord=ptrWord->next;/*point to the word after the first label*/
   
   val=0;
 }

 /*direct addressing type. the word value has been set already*/
 else if(addressTypeOne==0){
     ptrWord=ptrWord->next;/*we were pointing to the number , so we want to point to the word afterwards, that might contain a label name*/
 }

 /* if the second parameter is a label*/ 
 if(addressTypeTwo==1||addressTypeTwo==2){
   if(addressTypeTwo==2) secondPara=getLabelNameFromTypeTwo(secondPara);/*if we have type two of addressing we need to get the name of the label*/ 
   val=getBaseValFromLabel(secondPara,address);/*setting the first word witch is the base val of the label*/
   if(val==0) return 1;
   ptrWord->val=val;
   
   ptrWord=ptrWord->next;/*point to the word for offset val of label*/
   val=getOffsetValFromLabel(secondPara,address);/*setting the first word witch is the base val of the label*/
   if(val==0) return 1;
   ptrWord->val=val;
   ptrWord=ptrWord->next;/*point to the word after the first label*/
   
   val=0;
 }


return 0;
}






