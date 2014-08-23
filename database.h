#ifndef database_h_
#define database_h_

struct LicencePlate
{
    char state[3];
    char platenumber[7];
};
struct Owner
{
    char lastName [10];
    char firstName [10];
    struct LicencePlate licencePlateInfo;
};

typedef struct Owner Owner;
typedef struct LicencePlate LicencePlate;


Owner newOwner(char *state, char *number);
char * getCurrentDirectory();
void WriteToXML(Owner owner);

#endif /* FOO_H_ */



