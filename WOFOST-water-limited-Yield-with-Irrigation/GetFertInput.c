#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wofost.h"
#include "extern.h"

/* --------------------------------------------------------------------------*/
/*  function GetFertInput()                                                  */
/*  Purpose: Get the names of fertilization files and the start and end year */
/*           of the simulations. Read the la and lon of the boxes. Store the */
/*           information of each cell in the FERT struct.                    */
/* --------------------------------------------------------------------------*/

void GetFertInput(char *fertlist)
{
    FILE *fertf;
    
    Nutri *initial = NULL;
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
    
    fertf = fopen(fertlist, "r");

    if (fertf == NULL) 
    {
        fprintf(stderr, "Can't open %s\n", fertlist);
        exit(1);
    }
    
    while(fgets(line, MAX_STRING, fertf)) {
        if(line[0] == '*' || line[0] == ' ' || line[0] == '\n'){
            continue;
        }
        
        sscanf(line,"%s %d %d %d %s" , path, &StartYear, &EndYear, &NrSeasons, mask);
        
        if (initial == NULL) 
        {
            Fert = initial = malloc(sizeof(Nutri));
        }
        else 
        {
            Fert->next = malloc(sizeof(Nutri));
            Fert = Fert->next;  
        }  

        memset(Fert->mask,'\0',MAX_STRING);
        strncpy(Fert->mask, mask, strlen(mask));
        Fert->StartYear = StartYear;
        Fert->EndYear = EndYear;
        Fert->Seasons   = NrSeasons;
        Fert->next = NULL;
        
        for (i = 0; i < FERT_NTYPES; i++) {
            if(fgets(line, MAX_STRING, fertf) == NULL){
                fprintf(stderr, "Missing fertilization types\n");
                exit(1);
            }
            
            if(line[0] == '*' || line[0] == ' ' || line[0] == '\n'){
                continue;
            }
        
            sscanf(line,"%s %s %s" , filename, filetype, varname);
                    
            if (strlen(filename) >= MAX_STRING) exit(0);

            if (strcmp(filetype,"RESRATIO") == 0) {
                type = FERT_RESRATIO;
            } else if (strcmp(filetype, "N_MANURE") == 0) {
                type = FERT_N_MANURE;
            } else if (strcmp(filetype, "P_MANURE") == 0) {
                type = FERT_P_MANURE;
            } else if (strcmp(filetype, "N_UREA") == 0) {
                type = FERT_N_UREA;
            } else if (strcmp(filetype, "N_OTHERINOG") == 0) {
                type = FERT_N_OTHERINOG;
            } else if (strcmp(filetype, "P_INORG") == 0) {
                type = FERT_P_INORG;
            } else if (strcmp(filetype, "EFNOX") == 0) {
                type = FERT_EFNOX;
            } else {
                fprintf(stderr, "Unknown fertilization type %s\n", filetype);
                exit(1);
            }
            memset(Fert->type[type],'\0',MAX_STRING);
            strncpy(Fert->type[type], filetype, strlen(filetype));

            memset(Fert->file[type],'\0',MAX_STRING);
            strncpy(Fert->file[type], path, strlen(path));
            strncat(Fert->file[type], filename, strlen(filename));
            memset(Fert->var[type],'\0',MAX_STRING);
            strncpy(Fert->var[type], varname, strlen(filename));
        }
    }
          
    Fert = initial;
    fclose(fertf);
  
}