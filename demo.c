#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./Renderer/FancyTerminal.h"


typedef struct move_file_data{
    char **from;
    char **to;
} move_file_data;




int move_f(FancyTerminal *ft, FTElement *elem, void *data){
    ft->enabled=0;
}




int main(){
    FancyTerminal *ft=initFancyTerminal();
    setFTLayoutPattern(
        ft,
        5,7,
        "1 1 2 2 3 3 3,"
        "1 1 2 2 4 4 4,"
        "5 5 2 2 6 6 6,"
        "5 5 2 2 6 6 6,"
        "7 7 7 7 7 7 7,"
    );
    

    char *menus[] = {
        "Option a", 
        "Option b", 
        "Option c"
    };
    FTElement *checkmenus = createFTCheckBox(3, (char **)menus);            //1
    addToFancyTerminal(ft,checkmenus);

    FTElement *logo = createFTLogo(                                         //2                            
        11,20,
        "   _     _   ,"
        "  (c).-.(c)  ,"
        "   / ._. \\   ,"
        " __\\( Y )/__ ,"
        "(_.-/'-'\\-._),"
        "   || O ||   ,"
        " _.' `-' '._ ,"
        "(.-./`-'\\.-.),"
        " `-'     `-' "
    );
    addToFancyTerminal(ft,logo);
    
    FTElement *move_btn=createFTButton("Button");                             //3
    addToFancyTerminal(ft,move_btn);

    FTElement *from_file_path=createFTTextField("Write something... (1) ","");     //4
    addToFancyTerminal(ft,from_file_path);
    
    FTElement *radiomenus = createFTRadioBox(3, (char **)menus);            //5
    addToFancyTerminal(ft,radiomenus);

    FTElement *to_file_path=createFTTextField("Write something... (2) ","");         //6
    addToFancyTerminal(ft,to_file_path);
    
    


    FTElement *banner=createFTBanner("(banner) \n 'q' to quit,     'enter' to select     arrow keys to move    J-K to scroll     space to toggle \n");
    addToFancyTerminal(ft,banner);

    ft_connect(move_btn, FT_EVENT_CLICK, move_f, NULL);
    

    enterFancyTerminal(ft);

    printf("\npost end");
}







