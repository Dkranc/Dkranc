#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"



/*This module reads holds the main method for the assembler application.
The module reads a file from the users argument line then expands macros if needed, and then creates the instruction and data arrays.
when the file is read comletly the program creates a word array that holds all of the machine commands
After that the module creats the output files based on the commands form the word array */


/*initializing a linked list of word nodes*/
 Word *prevWord=NULL;
 Word *headWord=NULL;
 Word *iteratorWord=NULL;


/*declaring functions for this module*/
int checkIfAssembly(char * file);
void expandMacros(FILE *fBeforeMacros, FILE *fAfterMacros);
int checkIfNewMacro(char *line);
int checkIfEndOfMacro(char *line);
void createWordList();



/*declaring global variables*/
char *fileName;/*we use this so we have the name of the file without the ending so we know how to name the output files*/


/*The main method of the project calls all reading methods, then sends the expanded macro file to build the machine code.
when thats done the second read is done and words values that havent been set will be set then.
then we call the the function that builds all the output files*/

int main(int argc, char **argv)/*we have to get the arguments from the command line and the number of arguments*/
{
    
    FILE *fp;/*this will be the pointer the original file */
    FILE *fileAfterMacros;/*this will be the pointer to the file after macros are expanded */

    int i=1;
    int error;
    char fileFullName[maxLine];
    char fileNameAfterMacros[maxLine];

    /*parse arguments */
    if (argc >= 2){
     for(;i<argc;i++){/*we have a for loop so if theres more then one file that needs to be worked on*/

         fileName=argv[i];/*we get the file name with out the ending*/
         sprintf(fileFullName, "%s%s", fileName, ".as");/*adding .as to the file name that we got*/

         fp = fopen(fileFullName, "r");/*openning the file for reading*/

         if(fp==NULL){/*if the file wasnt loaded succefully*/
            printf("file not found\n");
            return 0;
          }
          else{
            printf("succes loading  \n");          
          } 

          /*expanding macros*/
          sprintf(fileNameAfterMacros, "%s%s", fileName, ".am");/*adding .as to the file name that we got*/         
          fileAfterMacros=fopen(fileNameAfterMacros, "w+");/*open the new file .am*/
          expandMacros(fp,fileAfterMacros);/*write the expanded version to the new file*/
          fclose(fileAfterMacros);
          printf("Macros expanded\n");

          /*first red of the assembly commands*/
          error=firstRead(fileNameAfterMacros);

          if(error==0){/*if no error was found in first read*/

           /*create the full machine code picture*/
           createWordList();
           printf("word List created\n");

           /*give the correct addressees to the labels we have*/
           setLabelAddresses();
           printf("Labels Addresses set\n");

           /*send file to second read*/
           error=secondRead(fileNameAfterMacros);

           if(error==0){/*if no error were found in second read*/


            /*cleanup */
            fclose(fp);
            fclose(fileAfterMacros);
       
            /*build the output files*/
            buildFiles(fileName);/*sending the files to be build*/
           }
          }

       /*clean up for next file to come in to the assembler*/
       IC=100;
       DC=0;
       error=0;
      }/*end of or loop*/
    }/*end of if argc>=2*/

    else {/*if no files were inputed*/
     printf("no file inputed\n");
    }
 return 0;
}




/*this method expands the macros that we have in the original files
 we use a linked list made out of the Macro struct type. each Macro has a name and a text that holds the macros code*/
void expandMacros(FILE *fBeforeMacros, FILE *fAfterMacros){
 int isOldMacro=0;/*used as a boolean to track if the line holds an old macro name that we allready have in the linked list*/
 char *firstWord;
 char *macroName=NULL;
 char line[MAXLINE];/*81*/
 char *theNewMacro=NULL; 
 Macro *prevMacro=NULL;
 Macro *headMacro=NULL;
 Macro *iteratorMacro=NULL;
 
 
 memset(&line[0], 0, sizeof(line));/*setting the line data to zero*/

 /*parsing the file line by line*/
 while ( fgets(line, MAXLINE-1, fBeforeMacros) != NULL){/*reading line by line untill we reach null*/
  isOldMacro=0; /*clearing the oldmacro var from last line */

  /*checking if the line is declaring a new macro*/
  if(checkIfNewMacro(line)==1){
   struct Macro* newMacro = (struct Macro*)malloc(sizeof(struct Macro));
   macroName=getMacroName(line);
   newMacro->name=macroName;
   /*movig  th pointers of the linked list*/
   if (prevMacro==NULL) {
    headMacro=newMacro;
   }
   else {
    prevMacro->next=newMacro;
   }
   theNewMacro=getMacro(fBeforeMacros);/*getting all the inner code from the macro*/
   newMacro->text=theNewMacro;
   newMacro->next=NULL;
   fprintf(fAfterMacros, "%s", newMacro->text);/*printing the expanded macro to the new file*/
   memset(&line[0], 0, sizeof(line));
   prevMacro=newMacro;
   }

  /*checking is the macro exists allready*/
  else {
   firstWord=getFirstWord(line);/*we assume there can be a call to a macro with more text after it. like said in page 34 in project explanation*/
   iteratorMacro=headMacro;
   while(iteratorMacro!=NULL){/*iterating over the list to find a matching name of macro*/
    if(strcmp(firstWord, iteratorMacro->name)==0){
     isOldMacro=1;
     fprintf(fAfterMacros, "%s",iteratorMacro->text);/*printing the expanded macro to the new file*/
    }/*end of if*/
    iteratorMacro=iteratorMacro->next;
   }/*end of while*/
   iteratorMacro=NULL;
 
    if(isOldMacro!=1){ /*if the line dosent exist in the list*/
     fprintf(fAfterMacros, "%s",line); /*printing the the line to the new file*/        
    }
  }/*end of big else */
 }/*end of big while*/

 /*clean up for next file to come. we dont want old macros to stay*/
 prevMacro=NULL;
 headMacro=NULL;
 iteratorMacro=NULL;
}/*end of method*/




/*this method checks if the line holds a new macro decleration
 we return 1 if it is  a new macro decleration else we return 0*/
int checkIfNewMacro(char *line){
 int i=0;
 char macro[]="macro";
 char firstWord[6];
 int lettercounter=0;

 while(line[i]==' '||line[i]=='\t') i++;/*skiping all white spaces*/

 /*get the first word from the line*/
 while(line[i]!=' '&&line[i]!='\t'&&lettercounter<5) {
  firstWord[lettercounter]=line[i];
  i++;
  lettercounter++;
 }

 firstWord[lettercounter]='\0';


 if(strcmp(macro,firstWord)==0) return 1;/*if we found a new macro decleration*/

 else return 0;
}





/*this method checks if we reached the end of amacro decleration. returns 0 if its the end and 1 if it isnt*/
int checkIfEndOfMacro(char *line){
 char end[]="endm";
 char *firstWord=NULL;

 firstWord=getFirstWord(line);
 if(strcmp(end,firstWord)==0) return 0;/*if we found the end of the macro*/

 else return 1;
}






/*this method creates the word array that will hold all instructions and data*/
void createWordList(){
 struct Word* newWord;

 /*copying all the instructions in to  a new list that combines the instructions with the data*/
 iteratorInstruction=headInstruction;
 while(iteratorInstruction!=NULL){

  /*setting the values of the new word*/
  newWord=(struct Word*)malloc(sizeof(struct Word));
  newWord->val=iteratorInstruction->val;
  newWord->valBase=iteratorInstruction->valBase;
  newWord->valOffset=iteratorInstruction->valOffset;

  /*movig  th pointers of the linked list*/
   if (prevWord==NULL) {
       headWord=newWord;
   }
   else {
       prevWord->next=newWord;
   }
   newWord->next=NULL;
   prevWord=newWord;

   
   iteratorInstruction=iteratorInstruction->next;
 }

 /*after the instruction list we need to add the data list*/
 iteratorData=headData;
 while(iteratorData!=NULL){
  newWord=(struct Word*)malloc(sizeof(struct Word));
  newWord->val=iteratorData->val;
  newWord->valBase=getBaseAddress(IC);
  newWord->valOffset=getOffsetAddress(IC);
  
   /* changing the address of the data as well so we can update the labels with no need to go throgh all instruction list*/
  iteratorData->valBase=getBaseAddress(IC);
  iteratorData->valOffset=getOffsetAddress(IC);

  IC++;/*we give the data words a new address depending on the size of the first array of instructions*/

  /*movig  th pointers of the linked list*/
   if (prevWord==NULL) {
       headWord=newWord;
   }
   else {
       prevWord->next=newWord;
   }
   newWord->next=NULL;
   prevWord=newWord;

   iteratorData=iteratorData->next;
 }
}

