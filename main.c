#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define tableAmount 50//显然餐厅的桌子数量是一个常量
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
        printf("%*s\n",40,"餐厅管理系统");
        for(i=0;i<80;i++)printf("*");
        printf("\n");
        printf("1 点菜\n2 客户结账\n3 账目管理\n4 餐馆统计\n5 退出\n请选择：");
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
                printf("无效的输入\n");
        }
        system("cls");
    }
    //fclose(menuFile);
    return 0;
}
//初始化文件指针
FILE* initializeMenu(){
    FILE *menu = NULL;
    menu=fopen("./menu.txt","r+");
    if(menu){
        printf("初始化菜单成功！\n");
        return menu;
    }
    else{
        printf("似乎你还没有菜单,请首先创建菜单\n");
        exit(0);
    }
}
food* loadMenu(FILE *menuFile,int *menuFoodAmount) {
    /*
     显然餐馆菜单可能需要频繁的进行搜索以点菜
     而且餐馆菜单在点菜/程序执行时不会变动
    所以完全没有必要在此处使用链表，所以采用数组
     */
    food *menu;
    int foodAmount = -1,i;
    printf("正在加载菜单");
    foodAmount = countFoodAmount(menuFile);
    fseek(menuFile, 0l, SEEK_SET);
    menu=(food*)malloc(sizeof(food)*foodAmount);
    for(i=0;i<foodAmount;i++){
        fscanf(menuFile,"%s%f",(menu+i)->foodName,&(menu+i)->price);
    }
    *menuFoodAmount=foodAmount;
    fclose(menuFile);
    printf("菜单加载成功！\n");
    return menu;
}
int  countFoodAmount(FILE* menuFile){
    //通过统计txt中的换行符数量来计算菜的数量
    char c=0,prev=0;int n=0;
    while(c=fgetc(menuFile)){
        if(c==EOF)break;
        if(c=='\n' && prev >= '0' && prev <= '9')
            //略去文件中多余的换行符
            n++;
        prev=c;
    }
    if(prev >= '0' && prev <= '9')n++;//防止最后一行未换行而没被统计
    printf("%d\n",n);
    return n;
}
void orderFood(struct clientMenu* cMenu,food* menu,int menuFoodAmount){
    int i,num,amount,choose,tNum,pNum;
    system("cls");
    while (1){
        printf("首先输入你的桌号和就餐人数\n");
        scanf("%d%d",&tNum,&pNum);
        if (tNum>0&&tNum<=tableAmount&&pNum>0){
            break;
        }
        printf("无效的输入\n");
    }
    cMenu->cData.clientNum=pNum;
    cMenu->cData.tableNum=tNum;
    tNum--;
    while (1){
        system("cls");
        printf("1.点菜\n2.删菜\n3.返回\n");
        scanf("%d",&choose);
        fflush(stdin);
        switch (choose) {
            case 1:
                for (i=0;i<menuFoodAmount;i++){
                    printf("%2d %20s %5f\n",i+1,(menu+i)->foodName,(menu+i)->price);
                    //打印菜单
                }
                scanf("%d%d",&num,&amount);
                if (num>menuFoodAmount||num<=0||amount<=0) {
                    //阻止错误的输入
                    printf("无效的输入\n");
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
                    printf("删除失败\n");
                }
                printCmenu(&cMenu[tNum],menu);
                break;
            case 3:
                return;
        }
        fflush(stdin);
        printf("按下ENTER继续\n");
        getchar();
    }

}
int searchCmenu(struct clientMenu *cMenu,int num,int amount){
    //寻找该菜是否已在客户菜单中,有就直接添加
    cFood *now=cMenu->head;
    while (now!=NULL){
        if (now->sFoodNum == num){
            now->foodAmount+=amount;
            return 1;
        }
        now=now->next;
    }
    //没有返回0
    return 0;
}
void addCmenu(struct clientMenu* cMenu,int num,int amount){
    //在客户菜单链表尾加入菜项
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
    //删菜数量与菜的数量相同直接删除，小于减数量，大于不改动 返回0
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
    //打印客户菜单
    int i=1;
    cFood *now=cMenu->head;
    if (now==NULL){
        printf("客户菜单为空\n");
        return;
    }
    while (now!=NULL){
        printf("%d. %s 数量：%d\n",i,(menu+now->sFoodNum)->foodName,now->foodAmount);
        i++;
        now=now->next;
    }
}
void clientSettleAccounts(FILE *billFile,struct clientMenu *cMenu,food* menu,int menuFoodAmount){
    //统计金额并结账同时写入账单文件，结账bool置为true
    int i=0,tNum;float cost=0;
    struct clientMenu bill;
    cFood *now;
    system("cls");
    billFile=fopen("./accounts.txt","a");
    while (1){
        printf("首先输入你的桌号\n");
        scanf("%d",&tNum);
        if (tNum>0&&tNum<=tableAmount){
            break;
        }
        printf("无效的输入\n");
    }
    tNum--;
    bill=cMenu[tNum];
    now=bill.head;
    if (now==NULL){
        printf("客户菜单为空，结账失败");
        fflush(stdin);
        getchar();
        return;
    }
    while (now!=NULL){
        i++;
        printf("%d. %s 数量：%d %f\n",i,(menu+now->sFoodNum)->foodName,now->foodAmount,now->foodAmount*(menu+now->sFoodNum)->price);
        cost+=now->foodAmount*(menu+now->sFoodNum)->price;
        fprintf(billFile,"%s %f\n",(menu+now->sFoodNum)->foodName,now->foodAmount*(menu+now->sFoodNum)->price);
        //食物数量*食物价格
        now=now->next;
    }
    printf("总价格：%f",cost);
    fprintf(billFile,"totalPrice %f\n",cost);
    fclose(billFile);
    cMenu[tNum].isPayed=true;
    printf("结账成功，按下ENTER返回\n");
    fflush(stdin);
    getchar();
}
void AccountsManage(struct  clientMenu*cMenu,int menuFoodAmount,food *menu){
    int chooce,i;
    while (1) {
        system("cls");
        printf("1.显示所有账单信息\n2.清除已结账餐桌信息\n3.返回");
        scanf("%d", &chooce);
        switch (chooce) {
            case 1:
                system("cls");
                printf(":\n");
                for (i = 0; i < menuFoodAmount; i++) {
                    if ((cMenu + i)->head != NULL) {
                        printf("餐桌号：%d，结账情况：%s\n", i+1, (cMenu + i)->isPayed ? "已结账" : "未结账");
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
                printf("清除成功\n");
                break;
            case 3:
                return;
            default:
                printf("无效的输入\n");
        }
        fflush(stdin);
        getchar();
    }
}
void freeAllcFood(cFood* head){
    //删除头结点下的整个链表
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
    //统计账单文件中的所有数据
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
        //重复的菜直接添加
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
    //寻找最大值
    max=price[0];
    k=0;
    for (j=0;j<i;j++){
        if (price[j]>max){
            max=price[j];
            k=j;
        }
    }
    printf("最受欢迎的菜：%s\n总收入：%f",foodRank[k],s);
    fflush(stdin);
    getchar();
}

