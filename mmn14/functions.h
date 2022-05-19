enum Func{mov,cmp,add,sub,lea,clr,not,inc,dec,jmp,bne,jsr,red,prn,rts,stop};
enum Type{code,data};
enum otherAccess{null,ent,ext};


/*this struct type is used as a linked list node. we use the nodes to keep track of the macros in the original file */
typedef struct Macro{
    char* name;
    char* text;
    struct Macro* next;
}Macro;


/*this will be a node in an arraylist for the labels we get in the program. when we meet the first Label we will start the list*/
typedef struct Label{
    int valDec;
    int valBase;
    int valOffset;
    int extAddress;/*special value for the exter labels so we can find them later when we build the ext file*/
    char* name;
    int oldAddress;/*used to find data address before updating to the final address*/
    enum otherAccess entOrExt;/*can be external or entery */
    enum Type type;/*can be code or data */
    struct Label* next;
}Label;


/*this will be a node in an arraylist for the Data we get in the program. when we meet the first Data usage in the program we will start the list*/
typedef struct Data{
    int val;
    int valBase;
    int valOffset;
    int oldAddress;/*used to find data address before updating to the final address*/
    struct Data* next;
}Data;


/*this will be a node in an arraylist for the Instructions we get in the program. when we meet the first Instruction  in the program we will start the list*/
typedef struct Instruction{
    int val;
    int valBase;
    int valOffset;
    struct Instruction* next;
}Instruction;

/*this will be a node in an arraylist for the Words we get in the program.*/
typedef struct Word{
    int val;
    int valBase;
    int valOffset;
    struct Word* next;
}Word;



int firstRead(char *fileName);
int secondRead(char *fileName);
int readLineTwo(char *line);
int readLine(char* line);
void openMacros(FILE *fBeforeMacros, FILE *fAfterMacros);
int checkIfNewMacro(char *line);
char* getMacroName(char *line);
int checkIfEndOfMacro(char *line);
char* getFirstWord(char *line);
int checkIfLabel(char *line);
int checkIfComment(char *line);
int checkIfValidLabel(char *firstWord,int newDef);
int checkLegalEntry(char *line,int islabel);
int checkIfValidLine(char *line, int islabel);
int legalParanteciesAddressingType(char *para);
int partOfList(char *label);
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
int hasParantecies(char *para);
int isRegister(char *para);
int getDirectNum(char *firstPara);
int checkIfValidLineNoPara(char *line,int isLabel);
int checkLegalData(char *line,int isLabel);
int twosComplement(int num);
int hasText(char *line);
int getNewAddress(int oldAddress);
void buildFiles(char *fileName);
int addNumOfWordsByAddressingType(int addressTypeOne);
void writeToExt(int address,char *labelName,char bOrO);
void nillAddress();
char* getWordVal(int val);
char* getHexVal(int num);
char* getLabelNameFromTypeTwo(char *firstPara);
int getOffsetValFromLabel(char *para,int address);
int getBaseValFromLabel(char *para,int address);
char* getMacroName(char *line);
char* getFirstWord(char *line);
char* getMacro(FILE *fBeforeMacros);
void  setLabelAddresses();


#define maxLine 81
#define A 262144
#define R 131072
#define E 65536
#define ASENDING 3 /*length of .as ending in file name*/
#define MAXLINE 81 /*length of maximum line*/
#define MAXMACRONAME 31 /*max length of macro name*/
#define MAXMACRO 1620 /*max maxro length*/
#define MAXLABEL 31
#define MAXNUM 32767
#define MINNUM -32768


extern int IC;
extern int DC;
extern int FIC;
extern char *fileName;


extern Label *prevLabel;
extern Label *headLabel;
extern Label *iteratorLabel;


extern Data *prevData;
extern Data *headData;
extern Data *iteratorData;


extern Instruction *prevInstruction;
extern Instruction *headInstruction;
extern Instruction *iteratorInstruction;

/*initializing a linked list of word nodes*/
extern Word *prevWord;
extern Word *headWord;
extern Word *iteratorWord;

