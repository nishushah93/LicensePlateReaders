#include <stdio.h>
#include <unistd.h>
#include <string.h>



struct LicencePlate
{
    char state[3];
    char code[3];
    int plateNumber;
};
struct Owner
{
    char lastName [10];
    char firstName [10];
    struct LicencePlate licencePlateInfo;
};

typedef struct Owner Owner;
typedef struct LicencePlate LicencePlate;

struct Owner newOwner()
{
    char firstname[10];
    char lastname[10];
    char code[4];
    char state[3];
    int plateNumber;
    
    printf("Please enter your First name: ");
    scanf("%s",firstname);
    
    printf("Please enter your Last name: ");
    scanf("%s",lastname);
    
    printf("Please enter your state ");
    scanf("%s",state);
    
    printf("Please enter your code ");
    scanf("%s",code);
    
    printf("Please enter your plate number ");
    scanf("%d",&plateNumber);
    
    struct Owner nOwner;
    strcpy(nOwner.firstName ,firstname);
    strcpy(nOwner.lastName ,lastname);
    strcpy(nOwner.licencePlateInfo.state, state);
    strcpy(nOwner.licencePlateInfo.code ,code);
    nOwner.licencePlateInfo.plateNumber = plateNumber;
    
    
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
    fprintf(fp,"<owner firstName='%s' lastName='%s'> \n  <state>%s</state>  \n  <code>%s</code> \n  <plateNumber>%d</plateNumber> \n</owner> \n",owner.firstName,owner.lastName,owner.licencePlateInfo.state,owner.licencePlateInfo.code,owner.licencePlateInfo.plateNumber);
    fclose(fp);
}



int main(int argc, const char * argv[])
{
    struct Owner owner;
    
    owner = newOwner();
    
    WriteToXML(owner);
    
    return 0;
}