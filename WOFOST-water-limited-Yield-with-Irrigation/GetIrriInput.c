#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wofost.h"
#include "extern.h"

/* --------------------------------------------------------------------------*/
/*  function GetIrriInput()                                                  */
/*  Purpose: Get the names of irrigation files and the start and end year */
/*           of the simulations. Read the la and lon of the boxes. Store the */
/*           information of each cell in the FERT struct.                    */
/* --------------------------------------------------------------------------*/

void GetIrriInput(char *irrilist)
{
    FILE *irrif;
    
    Water_Irri *initial = NULL;
    int i;
    
    char line[MAX_STRING];
    unsigned int type;
    int StartYear;
    int EndYear;
    int NrSeasons;
    char path[MAX_STRING];
    char mask[MAX_STRING];
    char filename[MAX_STRING];
    char filetype[MAX_STRING];
    char varname[MAX_STRING];
    
    irrif = fopen(irrilist, "r");

    if (irrif == NULL) 
    {
        fprintf(stderr, "Can't open %s\n", irrilist);
        exit(1);
    }
    
    while(fgets(line, MAX_STRING, irrif)) {
        if(line[0] == '*' || line[0] == ' ' || line[0] == '\n'){
            continue;
        }
        
        sscanf(line,"%s %d %d %d %s" , path, &StartYear, &EndYear, &NrSeasons, mask);
        
        if (initial == NULL) 
        {
            Irri = initial = malloc(sizeof(Water_Irri));
        }
        else 
        {
            Irri->next = malloc(sizeof(Water_Irri));
            Irri = Irri->next;  
        }  

        memset(Irri->mask,'\0',MAX_STRING);
        strncpy(Irri->mask, mask, strlen(mask));
        Irri->StartYear = StartYear;
        Irri->EndYear = EndYear;
        Irri->Seasons   = NrSeasons;
        Irri->next = NULL;
        
        for (i = 0; i < IRRI_NTYPES; i++) {
            if(fgets(line, MAX_STRING, irrif) == NULL){
                fprintf(stderr, "Missing irrigation rate\n");
                exit(1);
            }
            
            if(line[0] == '*' || line[0] == ' ' || line[0] == '\n'){
                continue;
            }
        
            sscanf(line,"%s %s %s" , filename, filetype, varname);
                    
            if (strlen(filename) >= MAX_STRING) exit(0);

            if (strcmp(filetype,"IRRIRATE") == 0) {type = IRRI_RATE;} 
            else{
                fprintf(stderr, "Unknown irrigation info %s\n", filetype);
                exit(1);
            }
            memset(Irri->type[type],'\0',MAX_STRING);
            strncpy(Irri->type[type], filetype, strlen(filetype));
            memset(Irri->file[type],'\0',MAX_STRING);
            strncpy(Irri->file[type], path, strlen(path));
            strncat(Irri->file[type], filename, strlen(filename));
            memset(Irri->var[type],'\0',MAX_STRING);
            strncpy(Irri->var[type], varname, strlen(filename));
        }
    }
          
    Irri = initial;
    fclose(irrif);
  
}