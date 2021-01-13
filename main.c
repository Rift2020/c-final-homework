#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define tableAmount 50//��Ȼ����������������һ������
typedef struct Food{
    char foodName[25];
    float price;
}food;
typedef struct diningTable {
    int tableNum;
    int clientNum;
}dTable;
typedef struct clientFood{
    int sFoodNum;//selectedFood
    int foodAmount;
    struct clientFood* next;
}cFood;
struct clientMenu {
    dTable cData;
    cFood *head;
    cFood *tail;
    bool isPayed;
} ;
FILE* initializeMenu();
food* loadMenu(FILE* menuFile,int* menuFoodAmount);
int  countFoodAmount(FILE* menuFile);
void orderFood(struct clientMenu*,food *,int );
int searchCmenu(struct clientMenu* cMenu,int,int);
void addCmenu(struct clientMenu* cMenu,int num,int amount);
void printCmenu(struct clientMenu *cMenu,food* menu);
int deleteCmenu(struct  clientMenu *cMenu,int num,int);
void clientSettleAccounts(FILE *billFile,struct clientMenu *cMenu,food* menu,int menuFoodAmount);
void AccountsManage(struct  clientMenu*cMenu,int menuFoodAmount,food* menu);
void freeAllcFood(cFood* head);
void statisticsAccounts(int menuFoodAmount);
int main()
{
    FILE *menuFile = NULL;
    FILE *billFile=NULL;
    int menuFoodAmount,i;
    food *menu;
    struct clientMenu cMenu[tableAmount];
    for (i=0;i<tableAmount;i++){
        cMenu[i].head=NULL;
        cMenu[i].tail=NULL;
        cMenu[i].isPayed=false;
    }
    menuFile=initializeMenu(menuFile);
    menu=loadMenu(menuFile,&menuFoodAmount);
    while (1){
        for(i=0;i<80;i++)printf("*");
        printf("\n");
        printf("%*s\n",40,"��������ϵͳ");
        for(i=0;i<80;i++)printf("*");
        printf("\n");
        printf("1 ���\n2 �ͻ�����\n3 ��Ŀ����\n4 �͹�ͳ��\n5 �˳�\n��ѡ��");
        scanf("%d",&i);
        switch(i){
            case 1:
                orderFood(cMenu,menu,menuFoodAmount);
                break;
            case 2:
                clientSettleAccounts(billFile,cMenu,menu,menuFoodAmount);
                break;
            case 3:
                AccountsManage(cMenu,menuFoodAmount,menu);
                break;
            case 4:
                statisticsAccounts(menuFoodAmount);
                break;
            case 5:
                exit(0);
            default:
                printf("��Ч������\n");
        }
        system("cls");
    }
    //fclose(menuFile);
    return 0;
}
//��ʼ���ļ�ָ��
FILE* initializeMenu(){
    FILE *menu = NULL;
    menu=fopen("./menu.txt","r+");
    if(menu){
        printf("��ʼ���˵��ɹ���\n");
        return menu;
    }
    else{
        printf("�ƺ��㻹û�в˵�,�����ȴ����˵�\n");
        exit(0);
    }
}
food* loadMenu(FILE *menuFile,int *menuFoodAmount) {
    /*
     ��Ȼ�͹ݲ˵�������ҪƵ���Ľ��������Ե��
     ���Ҳ͹ݲ˵��ڵ��/����ִ��ʱ����䶯
    ������ȫû�б�Ҫ�ڴ˴�ʹ���������Բ�������
     */
    food *menu;
    int foodAmount = -1,i;
    printf("���ڼ��ز˵�");
    foodAmount = countFoodAmount(menuFile);
    fseek(menuFile, 0l, SEEK_SET);
    menu=(food*)malloc(sizeof(food)*foodAmount);
    for(i=0;i<foodAmount;i++){
        fscanf(menuFile,"%s%f",(menu+i)->foodName,&(menu+i)->price);
    }
    *menuFoodAmount=foodAmount;
    fclose(menuFile);
    printf("�˵����سɹ���\n");
    return menu;
}
int  countFoodAmount(FILE* menuFile){
    //ͨ��ͳ��txt�еĻ��з�����������˵�����
    char c=0,prev=0;int n=0;
    while(c=fgetc(menuFile)){
        if(c==EOF)break;
        if(c=='\n' && prev >= '0' && prev <= '9')
            //��ȥ�ļ��ж���Ļ��з�
            n++;
        prev=c;
    }
    if(prev >= '0' && prev <= '9')n++;//��ֹ���һ��δ���ж�û��ͳ��
    printf("%d\n",n);
    return n;
}
void orderFood(struct clientMenu* cMenu,food* menu,int menuFoodAmount){
    int i,num,amount,choose,tNum,pNum;
    system("cls");
    while (1){
        printf("��������������ź;Ͳ�����\n");
        scanf("%d%d",&tNum,&pNum);
        if (tNum>0&&tNum<=tableAmount&&pNum>0){
            break;
        }
        printf("��Ч������\n");
    }
    cMenu->cData.clientNum=pNum;
    cMenu->cData.tableNum=tNum;
    tNum--;
    while (1){
        system("cls");
        printf("1.���\n2.ɾ��\n3.����\n");
        scanf("%d",&choose);
        fflush(stdin);
        switch (choose) {
            case 1:
                for (i=0;i<menuFoodAmount;i++){
                    printf("%2d %20s %5f\n",i+1,(menu+i)->foodName,(menu+i)->price);
                    //��ӡ�˵�
                }
                scanf("%d%d",&num,&amount);
                if (num>menuFoodAmount||num<=0||amount<=0) {
                    //��ֹ���������
                    printf("��Ч������\n");
                    break;
                }
                num--;
                if (searchCmenu(&cMenu[tNum],num,amount)==0)
                    addCmenu(&cMenu[tNum],num,amount);
                printCmenu(&cMenu[tNum],menu);
                break;
            case 2:
                printCmenu(&cMenu[tNum],menu);
                scanf("%d%d",&num,&amount);
                if(deleteCmenu(&cMenu[tNum],num,amount)==0){
                    printf("ɾ��ʧ��\n");
                }
                printCmenu(&cMenu[tNum],menu);
                break;
            case 3:
                return;
        }
        fflush(stdin);
        printf("����ENTER����\n");
        getchar();
    }

}
int searchCmenu(struct clientMenu *cMenu,int num,int amount){
    //Ѱ�Ҹò��Ƿ����ڿͻ��˵���,�о�ֱ�����
    cFood *now=cMenu->head;
    while (now!=NULL){
        if (now->sFoodNum == num){
            now->foodAmount+=amount;
            return 1;
        }
        now=now->next;
    }
    //û�з���0
    return 0;
}
void addCmenu(struct clientMenu* cMenu,int num,int amount){
    //�ڿͻ��˵�����β�������
    cFood *current=(cFood*)malloc(sizeof(cFood));
    current->sFoodNum=num;
    current->foodAmount=amount;
    current->next=NULL;
    if (cMenu->tail==NULL||cMenu->head==NULL){
        cMenu->head=current;
        cMenu->tail=current;
    } else{
        cMenu->tail->next=current;
        cMenu->tail=current;
    }
}
int deleteCmenu(struct  clientMenu *cMenu,int num,int amount){
    //ɾ��������˵�������ֱͬ��ɾ����С�ڼ����������ڲ��Ķ� ����0
    int i=0;
    cFood *now=cMenu->head->next;
    cFood  *prev=cMenu->head;
    if (num<=0)return 0;
    if (num==1){
        if (amount==prev->foodAmount){
            cMenu->head=cMenu->head->next;
            free(prev);
            return 1;
        } else if (amount>prev->foodAmount||amount<0)
            return 0;
        else prev->foodAmount-=amount;
        return 1;
    }
    for (i=1;i<num-1;i++){
        if (now==NULL)return 0;
        prev=now;
        now=now->next;
    }
    if (now==NULL)return 0;
    if (amount==now->foodAmount){
        prev->next=now->next;
        free(now);
    } else if (amount>now->foodAmount||amount<0)
        return 0;
    else now->foodAmount-=amount;
    return 1;
}
void printCmenu(struct clientMenu *cMenu,food* menu){
    //��ӡ�ͻ��˵�
    int i=1;
    cFood *now=cMenu->head;
    if (now==NULL){
        printf("�ͻ��˵�Ϊ��\n");
        return;
    }
    while (now!=NULL){
        printf("%d. %s ������%d\n",i,(menu+now->sFoodNum)->foodName,now->foodAmount);
        i++;
        now=now->next;
    }
}
void clientSettleAccounts(FILE *billFile,struct clientMenu *cMenu,food* menu,int menuFoodAmount){
    //ͳ�ƽ�����ͬʱд���˵��ļ�������bool��Ϊtrue
    int i=0,tNum;float cost=0;
    struct clientMenu bill;
    cFood *now;
    system("cls");
    billFile=fopen("./accounts.txt","a");
    while (1){
        printf("���������������\n");
        scanf("%d",&tNum);
        if (tNum>0&&tNum<=tableAmount){
            break;
        }
        printf("��Ч������\n");
    }
    tNum--;
    bill=cMenu[tNum];
    now=bill.head;
    if (now==NULL){
        printf("�ͻ��˵�Ϊ�գ�����ʧ��");
        fflush(stdin);
        getchar();
        return;
    }
    while (now!=NULL){
        i++;
        printf("%d. %s ������%d %f\n",i,(menu+now->sFoodNum)->foodName,now->foodAmount,now->foodAmount*(menu+now->sFoodNum)->price);
        cost+=now->foodAmount*(menu+now->sFoodNum)->price;
        fprintf(billFile,"%s %f\n",(menu+now->sFoodNum)->foodName,now->foodAmount*(menu+now->sFoodNum)->price);
        //ʳ������*ʳ��۸�
        now=now->next;
    }
    printf("�ܼ۸�%f",cost);
    fprintf(billFile,"totalPrice %f\n",cost);
    fclose(billFile);
    cMenu[tNum].isPayed=true;
    printf("���˳ɹ�������ENTER����\n");
    fflush(stdin);
    getchar();
}
void AccountsManage(struct  clientMenu*cMenu,int menuFoodAmount,food *menu){
    int chooce,i;
    while (1) {
        system("cls");
        printf("1.��ʾ�����˵���Ϣ\n2.����ѽ��˲�����Ϣ\n3.����");
        scanf("%d", &chooce);
        switch (chooce) {
            case 1:
                system("cls");
                printf(":\n");
                for (i = 0; i < menuFoodAmount; i++) {
                    if ((cMenu + i)->head != NULL) {
                        printf("�����ţ�%d�����������%s\n", i+1, (cMenu + i)->isPayed ? "�ѽ���" : "δ����");
                        printCmenu(cMenu + i, menu);
                    }
                }
                break;
            case 2:
                for (i = 0; i < menuFoodAmount; i++) {
                    if ((cMenu + i)->isPayed == true) {
                        freeAllcFood((cMenu+i)->head);
                        (cMenu+i)->head=NULL;
                        (cMenu+i)->isPayed=false;
                    }
                }
                printf("����ɹ�\n");
                break;
            case 3:
                return;
            default:
                printf("��Ч������\n");
        }
        fflush(stdin);
        getchar();
    }
}
void freeAllcFood(cFood* head){
    //ɾ��ͷ����µ���������
    cFood *prev=head,*current;
    if (prev==NULL){
        return;
    }
    current=prev->next;
    while (current!=NULL){
        free(prev);
        prev=current;
        current=current->next;
    }
    free(prev);
}
void statisticsAccounts(int menuFoodAmount){
    //ͳ���˵��ļ��е���������
    FILE *accounts;
    int i=0,j,k;
    char  foodRank[menuFoodAmount][25],fsfood[25];
    float price[menuFoodAmount],fsprice,s=0,max;
    system("cls");
    memset(price,0,sizeof(price));
    accounts=fopen("./accounts.txt","r");
    while (fscanf(accounts,"%s%f\n",fsfood,&fsprice)!=EOF){
        //printf("%s %f\n",fsfood,fsprice);
        if (strcmp(fsfood,"totalPrice")==0){
            s+=fsprice;
            continue;
        }
        //�ظ��Ĳ�ֱ�����
        for (j=0;j<i&&j<menuFoodAmount;j++){
            if (strcmp(fsfood,foodRank[j])==0){
                price[i]+=fsprice;
                continue;
            }
        }
        strcpy(foodRank[i],fsfood);
        price[i]+=fsprice;
        i++;
    }
    //Ѱ�����ֵ
    max=price[0];
    k=0;
    for (j=0;j<i;j++){
        if (price[j]>max){
            max=price[j];
            k=j;
        }
    }
    printf("���ܻ�ӭ�Ĳˣ�%s\n�����룺%f",foodRank[k],s);
    fflush(stdin);
    getchar();
}

