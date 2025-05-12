#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "FancyTerminal.h"
#include "ExternalLib/rogueutil.h"
#include "pthread.h"


enum Essentials{
    ARROW_LEFT = 99900, 
    ARROW_RIGHT= 99901, 
    ARROW_UP   = 99902, 
    ARROW_DOWN = 99903, 
    BACKSPACE  = 99904,
    SPACE      = 99905,
    ESC        = 99906,
    ENTER      = 99907,
};


void renderFTLogo(void*,int*, int, int, int, int, int,int);
void renderFTButton(void*,int*, int, int, int, int, int,int);
void renderFTBanner(void*,int*, int, int, int, int, int,int);
void renderFTTextField(void*,int*, int, int, int, int, int,int);
void renderFTCheckBox(void*,int*, int, int, int, int, int,int);
void renderFTRadioBox(void*,int*, int, int, int, int, int,int);
void renderFTTreeView(void*,int*, int, int, int, int, int,int);

int selectHandlerFTButton(FancyTerminal*,FTElement*,void*);
int unselectableHandler(FancyTerminal*,FTElement*,void*);
int unselectHandlerFTCheckBoxOrRadioBox(FancyTerminal*,FTElement*,void*);

int inputHandlerFTTextField(FancyTerminal*,FTElement*,void*);
int inputHandlerFTCheckBox(FancyTerminal*,FTElement*,void*);
int inputHandlerFTRadioBox(FancyTerminal*,FTElement*,void*);
int scrollHandlerFTTree(FancyTerminal*,FTElement*,void*);

void updateFancyTerminal(FancyTerminal *ft);
void renderFancyTerminal(FancyTerminal *ft);
// void processFancyTerminal(FancyTerminal *ft);

int ft_detectKeys();
char* ft_strdup(char *str);
char* ft_strdup_n(char *str, int len);
int ft_eventHelper(FancyTerminal*,FTElement*,void*,int);
void ft_default_decorations(FTElement *element);














//---------------------------------------------------
#pragma region Creation
//---------------------------------------------------


FancyTerminal* initFancyTerminal(){

    FancyTerminal *ft=(FancyTerminal*)malloc(sizeof(FancyTerminal));
    
    ft->height=trows();
    ft->width=tcols();
    ft->selector=0;
    ft->selected=0;
    ft->enabled=0;
    
    ft->layout=(FTLayout*)malloc(sizeof(FTLayout));
    ft->layout->elemPointers=malloc(sizeof(void*));
    ft->layout->elemTypes=malloc(sizeof(int));
    ft->layout->elemCount=0;
    ft->layout->loadedElemCount=0;

    return ft;
}




FTElement* createFTLogo(int height, int width, char *pattern){

    FTLogo *logo=(FTLogo*)malloc(sizeof(FTLogo));
    logo->height=height;
    logo->width=width;
    logo->pattern=pattern;

    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_LOGO;
    element->element=logo;
    element->renderer=renderFTLogo;
    element->eventsCallable[FT_EVENT_SELECTED]=unselectableHandler;
    ft_default_decorations(element);
    return element;
}




FTElement* createFTBanner(char *text){ 
    
    FTBanner *banner=(FTBanner*)malloc(sizeof(FTBanner));
    banner->text=text;
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_BANNER;
    element->element=banner;
    element->renderer=renderFTBanner;
    element->eventsCallable[FT_EVENT_SELECTED]=unselectableHandler;
    ft_default_decorations(element);
    return element;
}




FTElement* createFTButton(char *text){ 
    
    FTButton *button=(FTButton*)malloc(sizeof(FTButton));
    button->text=text;
    button->callable=NULL;
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_BUTTON;
    element->element=button;
    element->renderer=renderFTButton;
    element->eventsCallable[FT_EVENT_SELECTED]=selectHandlerFTButton;
    ft_default_decorations(element);
    return element;
}




FTElement* createFTTextField(char *textHint, char *textData){
    
    FTTextField *textfield=(FTTextField*)malloc(sizeof(FTTextField));
    textfield->textHint=textHint;
    textfield->textData=(char*)malloc(strlen(textData)+1);
    strcpy(textfield->textData,textData);
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_TEXTFIELD;
    element->element=textfield;
    element->renderer=renderFTTextField;
    element->eventsCallable[FT_EVENT_INPUT_STREAM]=inputHandlerFTTextField;
    ft_default_decorations(element);
    return element;
}



FTElement* createFTCheckBox(int count, char **text){
    FTCheckBox *checkbox=(FTCheckBox*)malloc(sizeof(FTCheckBox));
    checkbox->elemCount=count;
    checkbox->texts = text;
    checkbox->checked = (int*)malloc(count*sizeof(int));
    for(int i=0;i<count;i++)
    checkbox->checked[i]=0;
    checkbox->localPointer=-1;
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_CHECKBOX;
    element->element=checkbox;
    element->renderer=renderFTCheckBox;
    element->eventsCallable[FT_EVENT_INPUT_STREAM]=inputHandlerFTCheckBox;
    element->eventsCallable[FT_EVENT_UNSELECTED]=unselectHandlerFTCheckBoxOrRadioBox;
    ft_default_decorations(element);
    return element;
}

FTElement *createFTRadioBox(int count, char **text){
    FTRadioBox *radiobox=(FTRadioBox*)malloc(sizeof(FTRadioBox));
    radiobox->elemCount=count;
    radiobox->texts = text;
    radiobox->selected=-1;
    radiobox->localPointer=-1;
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_RADIOBOX;
    element->element=radiobox;
    element->renderer=renderFTRadioBox;
    element->eventsCallable[FT_EVENT_INPUT_STREAM]=inputHandlerFTRadioBox;
    element->eventsCallable[FT_EVENT_UNSELECTED]=unselectHandlerFTCheckBoxOrRadioBox;
    ft_default_decorations(element);
    return element;
}

FTElement *createFTTreeView(FTTreeNode *root){
    FTTreeView *tree=(FTTreeView*)malloc(sizeof(FTTreeView));
    tree->root=root; 
    tree->scrollPos=0;
    
    FTElement *element=(FTElement*)malloc(sizeof(FTElement));
    element->type=FT_TREEVIEW;
    element->element=tree;
    element->renderer=renderFTTreeView;
    element->eventsCallable[FT_EVENT_SELECTED]=unselectableHandler;
    element->eventsCallable[FT_EVENT_INPUT_STREAM]=scrollHandlerFTTree;
    ft_default_decorations(element);
    return element;
}













//--------------------------------------------------------
#pragma region Add to layout
//--------------------------------------------------------



void addToFancyTerminal(FancyTerminal *ft, FTElement *element){
    ft->layout->elemPointers=realloc(ft->layout->elemPointers, (ft->layout->loadedElemCount+1)*sizeof(void*));
    ft->layout->elemPointers[ft->layout->loadedElemCount]=(void*)element;
    ft->layout->elemTypes=realloc(ft->layout->elemTypes, (ft->layout->loadedElemCount+1)*sizeof(int));
    ft->layout->elemTypes[ft->layout->loadedElemCount]=element->type;
    ft->layout->loadedElemCount++;
    
    updateFancyTerminal(ft);
}












//---------------------------------------------------
#pragma region Rendering
//---------------------------------------------------


void renderFTBox(int width, int height, int x, int y, int color){
    setColor(color);
    int w=width;
    int h=height;
    for(int i=0; i<w; i++){
        for(int j=0; j<h; j++){
            if(i==0 || i==w-1)
                printXY(x+i,y+j,"│");
            else if(j==0 || j==h-1)
                printXY(x+i,y+j,"─");
        }
        printXY(x+0,y+0,"╭");
        printXY(x+w-1,y+0,"╮");
        printXY(x+0,y+h-1,"╰");
        printXY(x+w-1,y+h-1,"╯");
    }
    resetColor();
}

void renderFTLogo(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTLogo *logo=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    setColor(decorations[FT_COLOR]);
    int logo_y=y+height/2.0-logo->height/2.0-1;
    int logo_x=x+width/2.0-logo->width/2.0-2;

    char *copy=ft_strdup(logo->pattern);
    char *ptn=strtok(copy,",");

    if(strlen(logo->pattern)){
        int i=0;
        while(ptn){
            printXY(logo_x+2,logo_y+1+i,ptn);
            i++;
            ptn=strtok(NULL,",");
        }
    }
    free(copy);
}

void renderFTBanner(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTBanner *banner=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    setColor(decorations[FT_COLOR]);

    int lineCnt=0;
    char *copy=ft_strdup(banner->text);
    for(int j=0;j<strlen(copy);j++) 
        if(copy[j]=='\n')
            lineCnt++;
    char *ptn=strtok(copy,"\n");
    if(strlen(banner->text)){
        int i=0;
        while(ptn){
            printXY(x+width/2-strlen(ptn)/2,y+height/2+i-lineCnt/2+1,ptn);
            i++;
            ptn=strtok(NULL,"\n");
        }
    }
}

void renderFTButton(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTButton *button=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    setColor(decorations[FT_COLOR]);
    printXY(x+width/2-strlen(button->text)/2,y+height/2,button->text);
}

void renderFTTextField(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTTextField *textfield=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    setColor(decorations[FT_COLOR]);
    char *copy=ft_strdup(textfield->textData);
    char *ptn=strtok(copy,"\n");
    int lastLen=0,last_i=0;
    if(strlen(textfield->textData)){
        lastLen=strlen(textfield->textData);
        int i=0;
        while(ptn){
            printXY(x+2,y+2+i,ptn);
            last_i=i;
            i++;
            ptn=strtok(NULL,"\n");
        }
    }else{
        setColor(GREY);
        printXY(x+2,y+2,textfield->textHint);
        resetColor();
    }
    if(lastLen && selected){
        printXY(x+2+lastLen,y+2+last_i,"_");
    }
}

void renderFTCheckBox(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTCheckBox *checkbox=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    for(int i=0;i<checkbox->elemCount;i++){
        setColor(decorations[FT_COLOR]);
        if(i==checkbox->localPointer) setBackgroundColor(1);
        char *prefix=" ▢ ";
        if(checkbox->checked[i]) prefix=" ▣ ";
        printXY(x+5,y+2+i,prefix);
        printXY(x+8,y+2+i,checkbox->texts[i]);
        resetColor();
    }
}

void renderFTRadioBox(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTRadioBox *radiobox=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    for(int i=0;i<radiobox->elemCount;i++){
        setColor(decorations[FT_COLOR]);
        if(i==radiobox->localPointer) setBackgroundColor(1);
        char *prefix=" ○ ";
        if(i==radiobox->selected) prefix=" ● ";
        printXY(x+5,y+2+i,prefix);
        printXY(x+8,y+2+i,radiobox->texts[i]);
        resetColor();
    }
}

void __renderFTTreeView(FTTreeNode *node, const char *prefix,int *x,int *y, int isLast) {
    if (!node) return;
    locate(*x, *y);

    printf("%s", prefix);
    if (isLast)
        printf("└── ");
    else
        printf("├── ");
    node->childCount==0 ? setColor(GREEN) : setColor(GREY);
    printf("%s\n", node->label ? node->label : "(null)");
    resetColor();

    char newPrefix[1024];
    snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "│   ");
    for (int i = 0; i < node->childCount; i++) {
        *y+=1;
        __renderFTTreeView(node->childs[i], newPrefix, x, y, i == node->childCount - 1);
    }
}

void renderFTTreeView(void *v, int* decorations, int width, int height, int x, int y, int hovered, int selected){
    FTTreeView *treeview=v;
    int color=(hovered ? (selected ? CYAN : decorations[FT_SELECTOR_BORDER_COLOR]) : decorations[FT_BORDER_COLOR]); 
    if(color!=-1) 
        renderFTBox(width,height,x,y, color);
    setColor(decorations[FT_COLOR]);
    FTTreeNode *root = treeview->root;
    x+=2;
    y+=2;
    __renderFTTreeView(root, "", &x, &y, 1);
}


#pragma endregion















//------------------------------------------------------
#pragma region Processing
//------------------------------------------------------


// select handler

int selectHandlerFTButton(FancyTerminal *ft, FTElement *element, void* data){
    renderFancyTerminal(ft);
    msleep(50);
    ft->selected=0;
    renderFancyTerminal(ft);
    ft_eventHelper(ft,element,data,FT_EVENT_CLICK);
    ft_eventHelper(ft,element,NULL,FT_EVENT_UNSELECTED);
    return 0;
}

int unselectableHandler(FancyTerminal *ft, FTElement *element, void* data){
    ft->selected=0;
    ft_eventHelper(ft,element,NULL,FT_EVENT_UNSELECTED);
    return 0;
}


// unselect handler

int unselectHandlerFTCheckBoxOrRadioBox(FancyTerminal *ft, FTElement *element, void* data){
    FTCheckBox *checkbox=element->element;
    checkbox->localPointer=-1;
    return 0;
}




// Input stream handler 

int inputHandlerFTTextField(FancyTerminal *ft, FTElement *element, void* data){
    int key=*((int*)data);
    FTTextField *textfield=element->element;
    char *temp=textfield->textData;
    if(key==BACKSPACE){
        if(strlen(textfield->textData)>0){
            char *new=ft_strdup_n(temp,strlen(temp)-1);
            free(textfield->textData);
            textfield->textData=new;
        }
    }else{
        if(key==SPACE)key=' ';
        if(key==ENTER)key='\n';
        char *new=ft_strdup_n(temp,strlen(temp)+1);
        new[strlen(temp)]=key;
        free(textfield->textData);
        textfield->textData=new;
    }
    ft_eventHelper(ft,element,data,FT_EVENT_STATE_CHANGED);
    return 0;
}

int inputHandlerFTCheckBox(FancyTerminal *ft, FTElement *element, void* data){
    int key=*((int*)data);
    FTCheckBox *checkbox=element->element;
    if(key==SPACE){
        checkbox->checked[checkbox->localPointer]=!checkbox->checked[checkbox->localPointer];
        ft_eventHelper(ft,element,data,FT_EVENT_STATE_CHANGED);
    }
    if(key=='j') checkbox->localPointer--;
    if(key=='k') checkbox->localPointer++;
    checkbox->localPointer=(checkbox->localPointer+checkbox->elemCount)%(checkbox->elemCount);
    return 0;
}

int inputHandlerFTRadioBox(FancyTerminal *ft, FTElement *element, void* data){
    int key=*((int*)data);
    FTRadioBox *radiobox=element->element;
    if(key==SPACE){
        radiobox->selected=radiobox->localPointer;
        ft_eventHelper(ft,element,data,FT_EVENT_STATE_CHANGED);
    }
    if(key=='j') radiobox->localPointer--;
    if(key=='k') radiobox->localPointer++;
    radiobox->localPointer=(radiobox->localPointer+radiobox->elemCount)%(radiobox->elemCount);
    return 0;
}

int scrollHandlerFTTree(FancyTerminal *ft, FTElement *element, void* data){
    int key=*((int*)data);
    FTTreeView *tree=element->element;
    if(key=='j') tree->scrollPos--;
    if(key=='k') tree->scrollPos++;
}





#pragma endregion

















//------------------------------------------------------
#pragma region Core Management
//------------------------------------------------------


void setFTLayoutPattern(FancyTerminal *ft,int rowCount, int colCount, char *pattern){
    ft->layout->pattern = (int**)malloc(sizeof(int*)*rowCount);
    for(int i=0; i<rowCount; i++)
        ft->layout->pattern[i] = (int*)malloc(sizeof(int)*colCount);
    
    ft->layout->patternDims = (int*)malloc(sizeof(int)*2);
    ft->layout->patternDims[0]=rowCount;
    ft->layout->patternDims[1]=colCount;

    int offset=0;

    for(int i=0; i<rowCount; i++){
        for(int j=0; j<colCount; j++){
            int a=1;
            int local_offset;
            sscanf(pattern+offset, "%d%n", &a, &local_offset );
            offset+=local_offset;
            ft->layout->pattern[i][j]=a-1;
        }
        offset++;
    }

    updateFancyTerminal(ft);
}



void updateFancyTerminal(FancyTerminal *ft){
    ft->layout->elemCount=0;

    int **ptt= ft->layout->pattern;
    int rows = ft->layout->patternDims[0];
    int cols = ft->layout->patternDims[1];

    ft->width = (tcols()/ft->layout->patternDims[1])*ft->layout->patternDims[1];
    ft->height= (trows()/ft->layout->patternDims[0])*ft->layout->patternDims[0];

    int doneArr[rows*cols];
    for(int i=0; i<rows*cols; i++) doneArr[i]=0;

    for(int i=0,target=0; i<rows; i++){
        int size_width = 0;
        int size_height= 0;
        
        for(int j=0; j<cols; j++){
            if(ptt[i][j]==-1)continue;
            if(ptt[i][j]==target && !doneArr[target]){
        
                ft->layout->elemDatas = (int**)realloc(ft->layout->elemDatas, (target+1)*sizeof(int*));
                ft->layout->elemDatas[target] = (int*)malloc(sizeof(int)*4);

                int k=0;
                while((++k+j)<cols && ptt[i][k+j]==target);
                size_width=k;
                k=0;
                while((++k+i)<rows && ptt[k+i][j]==target);
                size_height=k;
                doneArr[target]=1;

                ft->layout->elemDatas[target][0]=size_width;
                ft->layout->elemDatas[target][1]=size_height;
                ft->layout->elemDatas[target][2]=j;
                ft->layout->elemDatas[target][3]=i;
                ft->layout->elemCount++;
                target++;
            }
        }
    }
}



void renderFancyTerminal(FancyTerminal *ft){

    ft->width = (tcols()/ft->layout->patternDims[1])*ft->layout->patternDims[1];
    ft->height= (trows()/ft->layout->patternDims[0])*ft->layout->patternDims[0];

    float unitWidth  = 1.0*ft->width/ft->layout->patternDims[1];
    float unitHeight = 1.0*ft->height/ft->layout->patternDims[0];
    unitHeight = unitHeight<3 ? 3:unitHeight;

    cls();
    hidecursor();

    for(int i=0;i<ft->layout->elemCount;i++){
        FTElement *elem=ft->layout->elemPointers[i];

        int **elemIdxData=ft->layout->elemDatas;
        int width=elemIdxData[i][0]*unitWidth;
        int height=elemIdxData[i][1]*unitHeight;
        int x=elemIdxData[i][2]*unitWidth;
        int y=elemIdxData[i][3]*unitHeight;
        
        elem->renderer(elem->element, elem->decorations, width, height, x, y, i==ft->selector, ft->selected && i==ft->selector); // note : auto type converts
        resetColor(); 
    }
}





void processInputFancyTerminal(FancyTerminal *ft){

    int key = ft_detectKeys();

    if(ft->selected){           
        // Route to Individual Events 

        FTElement *selected_elem=ft->layout->elemPointers[ft->selector];
        if(key==ESC){
            ft->selected=0;
            ft_eventHelper(ft,selected_elem,NULL,FT_EVENT_UNSELECTED);
        }else{
            ft_eventHelper(ft,selected_elem,&key, FT_EVENT_INPUT_STREAM);
        }

    }else{                      
        // Master Control

        if(key=='q') {          
            cls();
            showcursor();
            exit(0);
        }else if(key==ENTER){   
            ft->selected=1;
            FTElement *newly_selected_elem=ft->layout->elemPointers[ft->selector];
            ft_eventHelper(ft,newly_selected_elem,NULL,FT_EVENT_SELECTED);
        }else{                  
            if(key==ARROW_DOWN)    ft->selector++;
            else if(key==ARROW_UP) ft->selector--;
            ft->selector += ft->layout->elemCount;
            ft->selector %= ft->layout->elemCount;
            
        }
    }
}



void setElementDecoration(FTElement *element, int decoration, int value){
    element->decorations[decoration]=value;
}




void enterFancyTerminal(FancyTerminal *ft){
    ft->enabled=1;
    while(ft->enabled) {
        renderFancyTerminal(ft);
        processInputFancyTerminal(ft);
    }
}

void exitFancyTerminal(FancyTerminal *ft){
    ft->enabled=0;
}



void ft_connect(FTElement *element, int event, int (*func)(FancyTerminal*, FTElement*,void*), void *data){
    element->eventsCallable[event]=func;
    element->eventsCallableData[event]=data;
}


















//------------------------------------------------------
#pragma region Utility
//------------------------------------------------------

int ft_eventHelper(FancyTerminal *ft, FTElement *elem, void *data, int _EVENT){
    if(data == NULL) data=elem->eventsCallableData[_EVENT];
    int (*eventHandler)(FancyTerminal*,FTElement*,void*)=elem->eventsCallable[_EVENT];
    if(eventHandler){
        elem->eventsCallable[_EVENT](ft, elem, data);
        return 0;
    }
    return 1;
}

char* ft_strdup(char *str){
    char *copy=(char*)malloc(strlen(str)+1);
    strcpy(copy,str);
    copy[strlen(str)]='\0';
    return copy;
}

char* ft_strdup_n(char *str, int len){
    char *copy=(char*)malloc(len+1);
    strncpy(copy,str,len);
    copy[len]='\0';
    return copy;
}

int ft_detectKeys() {
    static int detectedEscapeSequence;
    // sleep(1);
    char c = getch();

    if(detectedEscapeSequence) {
        if(c == '[') {
            c=getchar();
            if(c=='A')      return ARROW_UP;
            else if(c=='B') return ARROW_DOWN;
            else if(c=='C') return ARROW_RIGHT; 
            else if(c=='D') return ARROW_LEFT; 
        }
        detectedEscapeSequence = 0;
    }
    if(c=='\033') { 
        detectedEscapeSequence = 1;
        return ESC;
    } 
    else if(c=='\b'||c==127)    return BACKSPACE;
    else if(c=='\r'||c=='\n')   return ENTER;
    else if(c==' ')             return SPACE;
    else                        return c;

    return -1; 
}


void ft_default_decorations(FTElement *element){
    element->decorations=(int*)malloc(sizeof(int)*TOTAL_DECORATIONS);
    element->decorations[FT_COLOR]=WHITE;
    element->decorations[FT_BORDER_COLOR]=WHITE;
    element->decorations[FT_SELECTOR_BORDER_COLOR]=YELLOW;
    element->decorations[FT_SELECTED_BORDER_COLOR]=CYAN;
}