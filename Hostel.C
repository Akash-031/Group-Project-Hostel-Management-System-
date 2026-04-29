
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct hostel
{
    char name[55];
    int hid;
    int mob;
    int ts;
    int fs;
    int sf;
    int dfd;
    char gl[505];
    char otinfo[1005];
    struct hostel *next;



}H;






H *head=NULL;
H *last=NULL;








void addhostel()
{
    H *hos=malloc(sizeof(H));
    hos->next=NULL;
    printf("Enter your Hostel Name:\n");
    fgets(hos->name,sizeof(hos->name),stdin);

    printf("Enter your hostel unic ID no:\n");
    scanf("%d",&hos->hid);


    printf("Enter contacts no:\n");
    scanf("%d",&hos->mob);


    printf("Enter total seat no of your hostel:\n");
    scanf("%d",&hos->ts);


    printf("Enter free seat no:\n");
    scanf("%d",&hos->fs);


    printf("Enter seat fee amount per month:\n");
    scanf("%d",&hos->sf);


    printf("Enter google map distance from daffodil(km):\n");
    scanf("%d",&hos->dfd);
    getchar();

    printf("Paste google map link of your hostel:\n");
    fgets(hos->gl,sizeof(hos->gl),stdin);


    printf("give extra info about your hostel:\n");
    fgets(hos->otinfo,sizeof(hos->otinfo),stdin);

    hos->name[strcspn(hos->name, "\n")] = '\0';
    hos->gl[strcspn(hos->gl, "\n")] = '\0';
    hos->otinfo[strcspn(hos->otinfo, "\n")] = '\0';

    hos->next=NULL;

    if(head==NULL)
        {
            head=hos;
            last=hos;
    }

    else
        {
            last->next=hos;
            last=hos;
        }
}








void dishos()
{
    H *temp=head;
    if(temp!=NULL)
    {
        while(temp!=NULL)
        {
        printf("-----------------------------------\n");
        printf("Hostel name:%s\n",temp->name);
        printf("Hostel ID:%d\n",temp->hid);
        printf("contract no:%d\n",temp->mob);
        printf("Total seat:%d\n",temp->ts);
        printf("free seat:%d\n",temp->fs);
        printf("seat fee per month(TK): %d\n",temp->sf);
        printf("google map distance from daffodil(KM): %d\n",temp->dfd);
        printf("google map link : %s \n",temp->gl);
        printf("other info: %s \n",temp->otinfo);
        printf("-----------------------------------\n");

        temp=temp->next;
        }
        printf("\nPress Enter to continue...");
        getchar();


    }
    else{printf("\n**no record found**\n\n");}
}







void deleteHostel()
{
    int id,p=0;
    printf("Enter Hostel ID to delete:\n");
    scanf("%d", &id);

    H *temp = head;
    H *prev = NULL;

    if (head==NULL)
    {
        printf("NO record found\n press enter to continue");
        getchar();
        return;
    }

    if(head->hid==id)
    {
      head=head->next;
      free(temp);
      printf("successfully deleated\n press enter to continue\n");
      getchar();
      getchar();
      return;
    }
    while(temp!=NULL&&temp->hid!=id  )
    {
        prev=temp;
        temp=temp->next;

    }
    if(temp==NULL)
    {
       printf("NO record found\n press enter to continue");
        getchar();
        return;
    }

        prev->next=temp->next;
        if(prev->next==NULL)
        {
            last=prev;
        }
        free(temp);
     printf("successfully deleated\n press enter to continue\n");
      getchar();
      getchar();
      return;


}







void searchHostel()
{
    int id;
    printf("Enter Hostel ID to search:\n");
    scanf("%d", &id);

    H *temp = head;
    int found = 0;

    while (temp != NULL)
    {
        if (temp->hid == id)
        {
            printf("\nHostel Found!\n");
            printf("Hostel name:%s\n", temp->name);
            printf("Hostel ID:%d\n", temp->hid);
            printf("contact no:%d\n", temp->mob);
            printf("Total seat:%d\n", temp->ts);
            printf("free seat:%d\n", temp->fs);
            printf("seat fee per month(TK): %d\n", temp->sf);
            printf("distance from daffodil(KM): %d\n", temp->dfd);
            printf("google map link:\n%s\n", temp->gl);
            printf("other info:\n%s\n", temp->otinfo);

            found = 1;
            break;
        }
        temp = temp->next;
    }

    if (found == 0)
    {
        printf("Hostel not found!\n");
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();

}






void loadhostelinfo()
{

 FILE *fp=fopen("allhos.txt","r");
 if(fp==NULL){printf("starting fresh\n"); return;}
 else{printf("Data loded from file\n");}
 while(1)
{


 H *hos=malloc(sizeof(H));
 hos->next=NULL;
 if(fscanf(fp,"%d %d %d %d %d %d\n",&hos->hid,&hos->mob,&hos->ts,&hos->fs,&hos->sf,&hos->dfd)!=6)
 {
     free(hos); break;

 }
 if(fgets(hos->name,sizeof(hos->name),fp)==NULL)
 {
     free(hos);break;
 }
 if(fgets(hos->gl,sizeof(hos->gl),fp)==NULL)
 {
     free(hos);break;
 }
 if(fgets(hos->otinfo,sizeof(hos->otinfo),fp)==NULL)
 {
     free(hos);break;
 }

 hos->name[strcspn(hos->name,"\n")] = '\0';
 hos->gl[strcspn(hos->gl,"\n")] = '\0';
 hos->otinfo[strcspn(hos->otinfo,"\n")] = '\0';

 if(head==NULL)
 {
     head=hos;
     last=hos;
 }
 else
 {
     last->next=hos;
     last=hos;
 }
}

}





void  savehostel()
{
   FILE *fp=fopen("allhos.txt","w");
   H *temp=head;
   while(temp!=NULL)
   {
       fprintf(fp, "%d %d %d %d %d %d\n",temp->hid, temp->mob, temp->ts,temp->fs, temp->sf, temp->dfd);
       fprintf(fp, "%s\n%s\n%s\n", temp->name, temp->gl, temp->otinfo);
       temp=temp->next;
    }
    fclose(fp);
}



void updatehostel() {
    int id;
    printf("Enter your hostel ID to update:\n");
    scanf("%d", &id);
    getchar(); // newline consume

    H *temp = head;
    int found = 0;

    while (temp != NULL) {
        if (temp->hid == id) {
            found = 1;
            printf("\nHostel Found! Enter new details:\n");

           /* printf("Enter new Hostel Name:\n");
            fgets(temp->name, sizeof(temp->name), stdin);
            temp->name[strcspn(temp->name, "\n")] = '\0';

            printf("Enter new contact no:\n");
            scanf("%d", &temp->mob);

            printf("Enter new total seat no:\n");
            scanf("%d", &temp->ts);*/

            printf("Enter new free seat no:\n");
            scanf("%d", &temp->fs);

            printf("Enter new seat fee per month:\n");
            scanf("%d", &temp->sf);

            /*printf("Enter new google map distance from daffodil (km):\n");
            scanf("%d", &temp->dfd);
            getchar(); // newline consume

            printf("Enter new google map link:\n");
            fgets(temp->gl, sizeof(temp->gl), stdin);
            temp->gl[strcspn(temp->gl, "\n")] = '\0';

            printf("Enter new extra info:\n");
            fgets(temp->otinfo, sizeof(temp->otinfo), stdin);
            temp->otinfo[strcspn(temp->otinfo, "\n")] = '\0';*/

            printf("\nHostel info updated successfully!\n");
            break;
        }
        temp = temp->next;
    }

    if (found==0) {
        printf("Hostel with ID %d not found!\n", id);
    }

    getchar();
}



void deleteAllHostels() {
    H *temp = head;
    H *nextNode;

    if (head == NULL) {
        printf("No hostel records found!\n");
        return;
    }

    while (temp != NULL) {
        nextNode = temp->next;
        free(temp);
        temp = nextNode;
    }

    head = NULL;
    last = NULL;

    printf("All hostel records deleted successfully!\n");
    printf("Press Enter to continue...");
    getchar();
}





int main()
{
    loadhostelinfo();
   int p;
    do{
    printf("########################################\n");
    printf("#                                      #\n");
    printf("#        WELCOME TO DIU-SAMS           #\n");
    printf("#                                      #\n");
    printf("########################################\n");



    printf("OPTION for hostel authority:\n------------------------------------------------------\n");
    printf("1:Add your Hostel.\n");
    printf("2:display all Hostel info.\n");
    printf("3:delate a Hostel info.\n");
    printf("4:UpdateHostel info.\n");
    printf("5:deleate all Hostel info.\n\n");
    printf("OPTION for Student:\n------------------------------------------------------\n");

    printf("6:See all hostel info around daffodil\n");
    printf("7:search hostel by unic hostel id.\n");

    printf("8:Save and Exit\n");




    printf("enter your Option no:\n");
    scanf("%d",&p);
    getchar();
    switch(p)
    {
        case 1:addhostel();break;
        case 2:dishos();break;
        case 3:deleteHostel();break;
        case 4:updatehostel();break;
        case 5:deleteAllHostels();break;
        case 6:dishos();break;
        case 7:searchHostel();break;

    }
    }while(p!=8);
    savehostel();

}
