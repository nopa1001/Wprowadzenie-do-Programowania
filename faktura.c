#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define cls_clear "CLS"       // zmienic druga wartosc zaleznie od systemu (CLEAR(LINUX)/CLS(WINDOWS))

int menu();		//glowne menu programu
int settings_menu();	//menu ustawien przedsiebiorcy
int add_invoice();	//dodawanie faktury do pliku

typedef struct fakturka{

    char numer_faktury[15];

    char nazwa_przedsiebiorcy[51];
    char nip_p[11];
    char nr_konta[33];

    char nazwa_kontrahenta[51];
    char nip_kontrahenta[11];
    char data_wystawienia[15];
    char data_sprzedazy[15];
    char termin_platnosci[15];

    int ilosc_pozycji;

    char nazwa_1[50], nazwa_2[50], nazwa_3[50];
    int ilosc_1, ilosc_2, ilosc_3;
    float cena_1, cena_2, cena_3;           //zmiana z int bo cena może być po ","

}faktura_t;

typedef struct Faktura_node{
    faktura_t invoice;
    struct Faktura_node *next;
}faktura_node;


int size_fakt(faktura_node *head);
void push_front(faktura_node **head, faktura_t temp);
void print_fakt(faktura_node *current);
void print_all(faktura_node *head);
void clean(faktura_node **head);
int print_one(faktura_node **head, char nr[11]);
int inv_del(faktura_node **head, char nr[51]);

        int main(){

    menu();

    return 0;
}

int menu(){
    int choice = 0;
    do{
        system(cls_clear);
        printf("Menu\n\n");
        printf("1 - ustawienia przedsiebiorcy na pliku settings.db\n");
        printf("2 - dodawanie nowej faktury na pliku invoices.db\n");
        printf("3 - wyswietlanie listy faktur\n");
        printf("4 - wyswietlanie pojedynczej faktury\n");
        printf("5 - usuwanie faktury\n");
        printf("9 - wyjscie\n");

        scanf("%d", &choice);
        system(cls_clear);
        FILE *invoices;
        switch (choice) {
            case 1:
                settings_menu();
                break;

            case 2:
                add_invoice();
                break;

            case 3:     //<- wszystkie casy ktore operuja na invoices (poza dodawaniem)
            case 4:
            case 5:

                if ((invoices = fopen("invoices.db", "r")) == NULL)
                {
                    printf("Nie moge otworzyc pliku invoices.db\n");
                    exit(1);
                }
                faktura_node *head = NULL;    //head listy jednokierunkowej
                faktura_t temp;
                while (!feof(invoices))
                {
                    fscanf(invoices, "%[^;];\n%[^;];%[^;];%[^;];\n%[^;];%[^;];%[^;];%[^;];%[^;];\n%d;\n",
                           temp.numer_faktury, temp.nazwa_przedsiebiorcy, temp.nip_p, temp.nr_konta,
                           temp.nazwa_kontrahenta, temp.nip_kontrahenta, temp.data_wystawienia, temp.data_sprzedazy,
                           temp.termin_platnosci, &temp.ilosc_pozycji);
                    strcpy(temp.nazwa_1,"NULL");
                    strcpy(temp.nazwa_2,"NULL");
                    strcpy(temp.nazwa_3,"NULL");
                    temp.ilosc_1 = -1;
                    temp.ilosc_2 = -1;
                    temp.ilosc_3 = -1;

                    temp.cena_1 = -1;
                    temp.cena_2 = -1;
                    temp.cena_3 = -1;

                    if (temp.ilosc_pozycji > 0)
                    {
                        fscanf(invoices, "%[^;];%d;%f;\n", temp.nazwa_1, &temp.ilosc_1, &temp.cena_1);
                    }
                    if (temp.ilosc_pozycji > 1)
                    {
                        fscanf(invoices, "%[^;];%d;%f;\n", temp.nazwa_2, &temp.ilosc_2, &temp.cena_2);
                    }
                    if (temp.ilosc_pozycji == 3)
                    {
                        fscanf(invoices, "%[^;];%d;%f;\n", temp.nazwa_3, &temp.ilosc_3, &temp.cena_3);
                    }

                    if (strcmp(temp.nip_p, ""))
                    {
                        push_front(&head, temp);
                    } else {
                        printf("Error");
                        return 0;
                    }
                    //printf(temp.numer_faktury);
                }
                fclose(invoices);

                if (choice == 3)    //wyswietlanie wszystkich
                {
                    if(size_fakt(head)!=0)
                    {
                       print_all(head);
                    }
                    printf("q wyjscie\n");
                    while((getchar()) != 'q'){}

                }
                if(choice == 4)    //wyswietlanie jednej
                {
                    printf("Podaj numer faktury do wyswietlenia:\n");
                    char numer[15];
                    scanf(" %[^\n]%*c", numer);

                    if(!print_one(&head, numer))
                    {
                        printf("W bazie nie ma faktury o nazwie %s\n", numer);
                        printf("q wyjscie\n");
                        while((getchar()) != 'q'){}
                    }
                    printf("q wyjscie\n");
                    while((getchar()) != 'q'){}

                }
                if(choice == 5)     //usuwanie faktury
                {
                    if ((invoices = fopen("invoices.db", "w")) == NULL)
                    {
                        printf("Nie moge otworzyc pliku invoices.db\n");
                        exit(1);
                    }
                    printf("Podaj numer faktury ktora chcesz usunac:\n");
                    char numer[51];
                    scanf(" %[^\n]%*c", numer);

                    if(inv_del(&head, numer)==0)        //usuwanie faktury z listy jednokierunkowej
                    {
                        printf("W bazie nie ma faktury o nazwie %s\n", numer);
                        printf("q wyjscie\n");
                        while((getchar()) != 'q'){}
                    }
                    else
                    {
                        printf("Usunieto fakture o numerze: %s\n", numer);
                        printf("q wyjscie\n");
                        while((getchar()) != 'q'){}
                    }
                    faktura_node *curr = head;
                    while (curr != NULL)
                    {

                        fprintf(invoices, "%s;\n%s;%s;%s;\n%s;%s;%s;%s;%s;\n%d;\n",
                               curr->invoice.numer_faktury, curr->invoice.nazwa_przedsiebiorcy, curr->invoice.nip_p, curr->invoice.nr_konta,
                                curr->invoice.nazwa_kontrahenta, curr->invoice.nip_kontrahenta, curr->invoice.data_wystawienia, curr->invoice.data_sprzedazy,
                                curr->invoice.termin_platnosci, curr->invoice.ilosc_pozycji);

                        if(curr->invoice.ilosc_pozycji > 0)
                        {
                            fprintf(invoices, "%s;%d;%f;\n", curr->invoice.nazwa_1, curr->invoice.ilosc_1, curr->invoice.cena_1);
                        }
                        if(curr->invoice.ilosc_pozycji > 1)
                        {
                            fprintf(invoices, "%s;%d;%f;\n", curr->invoice.nazwa_2, curr->invoice.ilosc_2, curr->invoice.cena_2);
                        }
                        if(curr->invoice.ilosc_pozycji == 3)
                        {
                            fprintf(invoices, "%s;%d;%f;\n", curr->invoice.nazwa_3, curr->invoice.ilosc_3, curr->invoice.cena_3);
                        }

                        curr = curr->next;
                    }
                    fclose(invoices);
                }

                clean(&head);

                break;
            default:
                printf("Nie ma takiej opcji, wybierz jeszcze raz\n");
                break;
        }
    }while(choice != 9);
    system(cls_clear);

    return 0;
}
int settings_menu(){
    int choice_s;
    FILE *fp;
    do{
        system(cls_clear);
        printf("1 - wyswietlanie \n2 - zmiana lub tworzenie pliku\n9 - powrot do glownego menu\n");
        scanf("%d", &choice_s);
        system(cls_clear);
        char nazwa[51];
        char nip[11];
        char numer_konta[33];

        switch(choice_s){
            case 1:
                printf("wyswietlanie: \n");
                if ((fp=fopen("settings.db", "r"))==NULL)
                {
                    printf ("Nie moge otworzyc pliku settings.db \n");
                    exit(1);
                }
                fscanf(fp, "%[^;];%[^;];%[^;];", nazwa, nip, numer_konta);
                printf("nazwa: %s \nnip: %s \nnumer konta: %s \n", nazwa, nip, numer_konta);
                fclose(fp);
                printf("q wyjscie\n");
                while((getchar()) != 'q'){}
                //getch na win z conio.h ale na linuxie nie ma
                break;

            case 2:
                if ((fp=fopen("settings.db", "w"))==NULL)
                {
                    printf ("Nie moge otworzyc pliku settings.db \n");
                    exit(1);
                }
                printf("podaj nazwe swojej firmt:\n");
                scanf(" %[^\n]%*c", nazwa);
                printf("podaj swoj nip:\n");
                scanf(" %[^\n]%*c", nip);
                printf("podaj swoj numer konta: \n");
                scanf(" %[^\n]%*c", numer_konta);

                fprintf(fp, "%s;%s;%s;\n", nazwa, nip, numer_konta);

                fclose(fp);

                break;
            default:
                printf("nie ma takiego\n");
        }

    }
    while(choice_s != 9);
    return 0;
}

int add_invoice(){
    FILE *fakt;
    FILE *settings;
    char copy[250];
    char nr[15];
    char nazwa_kontrahenta[51];
    char nip_kontrahenta[11];
    char data_wystawienia[15];
    char data_sprzedazy[15];
    char termin_platnosci[15];
    int ilosc_pozycji;
    int ilosc;
    float cena;
    if((settings=fopen("settings.db", "r"))==NULL)
    {
        printf ("Nie moge otworzyc pliku settings.db do odczytu\n");
        exit(1);
    }
    if((fakt=fopen("invoices.db", "a"))==NULL)
    {
        printf ("Nie moge otworzyc pliku invoices.db\n");
        exit(1);
    }
    printf("podaj numer/nazwe faktury\n");
    scanf(" %[^\n]%*c", nr);
    fprintf(fakt, "%s;\n", nr);
    fscanf(settings,"%[^\n]", copy);	//kopiuje z pliku settings
    fprintf(fakt,"%s\n", copy);

    printf("podaj nazwe kontrahenta:\n");
    scanf(" %[^\n]%*c", nazwa_kontrahenta);
    printf("podaj numer nip kontrahenta:\n");
    scanf(" %[^\n]%*c", nip_kontrahenta);
    printf("podaj date wystawienia faktury:\n");
    scanf(" %[^\n]%*c", data_wystawienia);
    printf("podaj date sprzedazy faktury:\n");
    scanf(" %[^\n]%*c", data_sprzedazy);
    printf("podaj termin platnosci faktury:\n");
    scanf(" %[^\n]%*c", termin_platnosci);
    printf("podaj ilosc pozycji faktury (maksimum 3)\n");
    scanf("%d", &ilosc_pozycji);
    if(ilosc_pozycji>3){
        ilosc_pozycji=3;
    }
    int i;
    fprintf(fakt, "%s;%s;%s;%s;%s;\n%d;\n", nazwa_kontrahenta, nip_kontrahenta, data_wystawienia, data_sprzedazy, termin_platnosci, ilosc_pozycji);
    for(i = 0; i < ilosc_pozycji; i++)
    {
        printf("podaj nazwe %d pozycji:\n", i+1);
        scanf(" %[^\n]%*c", copy);
        printf("podaj ilosc dla %d pozycji:\n", i+1);
        scanf("%d", &ilosc);
        printf("podaj cene jednostkowa dla %d pozycji:\n", i+1);
        scanf("%f", &cena);
        fprintf(fakt, "%s;%d;%f;\n", copy, ilosc, cena);
    }
    printf("Faktura dodana, wpisz q aby wyjsc\n");
    while((getchar()) != 'q'){}
    fclose(fakt);
    fclose(settings);
    return 0;
}


int size_fakt(faktura_node *head)   //zwraca wielkosc listy jednokierunkowej
{
    faktura_node *temp;
    int licz = 0;
    temp = head;
    while(temp){
        licz++;
        temp = temp -> next;
    }
    return licz;
}


void push_front(faktura_node **head, faktura_t temp)     //wrzuca strukture na liste
{
    faktura_node *curr = malloc(sizeof(faktura_node));
    curr -> invoice = temp;
    curr -> next = *head;
    *head = curr;
}
void print_fakt(faktura_node *current){     //ladne wypisywanie danej faktury

    printf("\t\t\tFaktura VAT\n");
    printf("\t\t\tNumer: %s\n", current->invoice.numer_faktury);
    printf("Sprzedawca:\t\t\t\tNabywca:\n");
    printf("%s\t\t\t\t%s\n", current->invoice.nazwa_przedsiebiorcy, current->invoice.nazwa_kontrahenta);
    printf("NIP: %s\t\t\t\tNIP: %s\n", current->invoice.nip_p, current->invoice.nip_kontrahenta);
    printf("nr konta:%s\n\n", current->invoice.nr_konta);
    printf("Data wystawienia: %s\n", current->invoice.data_wystawienia);
    printf("Data sprzedazy: %s\n", current->invoice.data_sprzedazy);
    printf("Termin platnosci: %s\n\n", current->invoice.termin_platnosci);
    printf("Nazwa\tIlosc\tCena jednostkowa netto\t Wartosc netto\n");
    float sum=0;
    if(current->invoice.ilosc_pozycji>0)
    {
        printf("%s\t%d\t%5.2f zl\t\t%5.2f zl\n", current->invoice.nazwa_1, current->invoice.ilosc_1, current->invoice.cena_1, current->invoice.ilosc_1*current->invoice.cena_1);
        sum+=current->invoice.ilosc_1*current->invoice.cena_1;
    }
    if(current->invoice.ilosc_pozycji>1)
    {
        printf("%s\t%d\t%5.2f zl\t\t%5.2f zl\n", current->invoice.nazwa_2, current->invoice.ilosc_2, current->invoice.cena_2, current->invoice.ilosc_2*current->invoice.cena_2);
        sum+=current->invoice.ilosc_2*current->invoice.cena_2;
    }
    if(current->invoice.ilosc_pozycji==3)
    {
        printf("%s\t%d\t%5.2f zl\t\t%5.2f zl\n", current->invoice.nazwa_3, current->invoice.ilosc_3, current->invoice.cena_3, current->invoice.ilosc_3*current->invoice.cena_3);
        sum+=current->invoice.ilosc_3*current->invoice.cena_3;
    }
    printf("\nSumaryczna wartosc netto: %.2f zl\n", sum);
    printf("Kwota VAT: 23%%\n");
    printf("Sumaryczna wartosc VAT: %.2f zl\n\n", sum*0.23);  // VAT !!!
    printf("Razem brutto: %.2f zl\n", sum*1.23);
}
void print_all(faktura_node *head){  //wypisuje wszystkie faktury z listy
    faktura_node *curr = head;
    while(curr != NULL){
        print_fakt(curr);
        if(curr->next != NULL)      //zeby wyswietlalo ~~ tylko miedzy fakturami
        {
            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        }

        curr = curr->next;
    }
}

void clean(faktura_node **head){	//czysci pamiec listy
    faktura_node *curr = *head;
    faktura_node *next;

    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}

int pop(faktura_node **head){	//popuje jedna wartosc z gory

    faktura_node *next_n = NULL;

    if(*head == NULL){
        return -1;
    }

    next_n = (**head).next;
    free(*head);
    *head = next_n;
    return 1;
}

int inv_del(faktura_node **head, char nr[51])    //usuwanie faktury po jej numerze
{
    faktura_node *curr = *head;
    faktura_node *temp = NULL;

    if(strcmp(curr->invoice.numer_faktury, nr)==0)      //jesli to pierwszy
    {
        return pop(head);
    }

    for(int i = 0; i < size_fakt(*head)-1; i++)
    {
        if(strcmp(curr->next->invoice.numer_faktury, nr)==0)
        {
            temp = curr->next;
            curr->next = temp->next;
            free(temp);
            return 1;
        }
        curr = curr->next;
    }
    return 0; //jesli nic nie usunal
}

int print_one(faktura_node **head, char nr[15])  //printowanie 1 faktury po nazwie
{
    faktura_node *curr = *head;
    if(strcmp(curr->invoice.numer_faktury, nr)==0)
    {
        print_fakt(curr);

        return 1;
    }

    for(int i = 0; i < size_fakt(*head); i++)
    {
        if(strcmp(curr->invoice.numer_faktury, nr)==0)
        {
            print_fakt(curr);
            return 1;
        }
        curr = curr->next;
    }
    return 0; //nie ma tej faktury
}