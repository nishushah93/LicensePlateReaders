
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "database.h"

Owner newOwner(char *state, char *number)
{
    char firstname[10];
    char lastname[10];
    
    printf("Please enter your First name: ");
    scanf("%s",firstname);
    
    printf("Please enter your Last name: ");
    scanf("%s",lastname);
     
    struct Owner nOwner;
    strcpy(nOwner.firstName ,firstname);
    strcpy(nOwner.lastName ,lastname);
    strcpy(nOwner.licencePlateInfo.state, state);
    strcpy(nOwner.licencePlateInfo.platenumber ,number);
    
    
    return nOwner;
}

char * getCurrentDirectory()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
    strcat(cwd, "/test.xml");
    
    fprintf(stdout, "Current working dir: %s\n", cwd);
    return cwd;
}

void WriteToXML(Owner owner)
{
    FILE *fp ;
    char * cwd = getCurrentDirectory();
    fp = fopen(cwd, "a");
    fprintf(fp,"<owner firstName='%s' lastName='%s'> \n  <state>%s</state>  \n  <code>%s</code>  \n</owner> \n",owner.firstName,owner.lastName,owner.licencePlateInfo.state,owner.licencePlateInfo.platenumber);
    fclose(fp);
}




  //  Owner owner;
    
  //  owner = newOwner();
    
  //  WriteToXML(owner);
    

