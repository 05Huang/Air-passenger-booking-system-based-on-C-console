
#include<iostream>
#include<string>
#include<windows.h>
#include <algorithm>
#include<bits/stdc++.h> 
using namespace std;

typedef struct CNode 
{
    char name[20];//姓名
    int num;//订票数 
    char ID[20];//身份证
    char day[20];
    int dengji;//舱等级
    CNode *next;
} CNode, *CList;//乘客节点

typedef struct HbNode 
{
    char name[20];//姓名
    char ID[20]; //身份证
    int num;//预定数量 
    struct HbNode *next;
} QhbNode, *Phb;//候补队列中的节点

typedef struct Queue 
{
    Phb front;//等候替补客户名单域的头指针
    Phb rear;//等候替补客户名单域的尾指针
} LinkQueue;//候补队列

typedef struct hangban {
    char qidian[20];//起点站名
    char zhongdian[20];//终点站名
    char hbID[20];//航班编号 
    char hbnum[20];//飞机号
    char day[20];//飞行日期（星期几）
    int maxnum;//总票数
    int less;//总余票量
    int lessgrade1; //等级1剩余量
    int lessgrade2; //等级2剩余量
    hangban *next;
    CList cusLinkList;//乘员名单，指向乘员名单链表的头指针
    LinkQueue wQgrade1;//等级1候补，等候替补的客户名单域，指向一个队列
    LinkQueue wQgrade2;//等级2候补，等候替补的客户名单域，指向一个队列
} hangban, *Phangban;//航班节点

//中期答辩 
int guanliyuanyemian();//管理员页面 
int denglu(); //登录 
void initFlight(); //初始化航班链表
void sousuoflight();//由起点终点搜索航班 
void sousuoFace();// 搜索界面
void printfall(hangban *pflight);//打印全部航班信息
int menu();// 菜单界面函数
int zengjiaFlight();//增加航班 
int shanchuFlight();// 删除航班
void sousuoFlight();//由起点和终点搜索对应航班 
int Create(Phangban flight1);//将航班信息插入到链表中 
int duibiFlight(hangban *flight, char flightCodeID[]);// 对比插入航班标号，防止标号重复 
//期末答辩 
hangban *find(); // 输入编号返回对应信息
int initQueue(LinkQueue &q);//初始化队列
int initCusLinkList(CList &cusLinkList);//初始化订票乘客链表
void addlink(CList &head, int need, char name[], char id[],char day[], int grade);//将订票成功的乘客节点加入到已订票链表 
void pricusInfo();//输出已经订票的函数 
void dingpiao();// 订票函数 
void fightinfo(hangban *p);//输出对应节点信息 
LinkQueue enqueue(LinkQueue &q, char name[], int need, char id[]);//将等待的客户入队
void tuipiao();// 退票
void dingpiaoFace();//  订票界面
void tuipiaoFace(); //退票界面



hangban *pFlight;//全局节点变量 
 //初始输入的航班信息 
hangban flight1[4] = {
        {"徐州", "南京", "1", "A1185", "星期五", 400, 200, 100, 100},
        {"苏州", "淮安", "2", "B4521", "星期一", 300, 200, 100, 100},
        {"北京", "上海", "3", "C8541", "星期六", 500, 200, 100, 300},
        {"扬州", "镇江", "4", "D3652", "星期三", 600, 200, 100, 400},
};


//初始化航班链表 
void initFlight() 
{
    pFlight = new hangban;
    pFlight->next = NULL;
}


// 将已有航班信息插入链表
int Create(Phangban flight1) 
 {
    hangban *p = pFlight, *q;
    for (int i=0; i < 4; i++) {
        q = new hangban;
        if (q == NULL)
            return 0;
        strcpy(q->qidian, flight1[i].qidian);//利用链表把每个信息录入 
        strcpy(q->zhongdian, flight1[i].zhongdian);
        strcpy(q->hbID, flight1[i].hbID);
        strcpy(q->hbnum, flight1[i].hbnum);
        strcpy(q->day, flight1[i].day);
        q->maxnum = flight1[i].maxnum;
        q->less = flight1[i].maxnum;
        q->lessgrade1 = flight1[i].lessgrade1;
        q->lessgrade2 = flight1[i].maxnum - flight1[i].lessgrade1;
        initCusLinkList(q->cusLinkList); //初始化已经订票乘客链表
        initQueue(q->wQgrade1);// 将两种等级的票入队 ，方便后面预定排队 
        initQueue(q->wQgrade2);
        q->next = p->next;
        p->next = q;
    }
    return 1;
}



//初始化已订票乘客指针链表
int initCusLinkList(CList &cusLinkList) 
{
    CList q = cusLinkList;
    cusLinkList = (CNode *) malloc(sizeof(CNode));
    cusLinkList->next = NULL;
}

//初始化带头结点的链队列
int initQueue(LinkQueue &q) 
{
    QhbNode *p;
    p = new QhbNode;
    p->next = NULL;
    q.front = q.rear = p;//队空 
    return 1;
}

bool isValidID(const char id[]) 
{
    int length = strlen(id);
    // 检查身份证号码长度是否为18位数字 
    return (length == 18) && all_of(id, id + length, ::isdigit); // 使用 C++11 的 all_of 函数检查所有字符是否为数字
}

// 订票函数
void dingpiao()  
{
    hangban *info;
    int need, grade;
    int num;//剩余的等级1数目或者等级2数目
    char name[20];
    char id[20];
    char day[20];
    info = find();//输入想订的航班，看是否还有空位置，并返回复制节点 
    srand(time(0));// 初始化随机数种子
    int seatCount = 0; // 已分配座位的数量
    if (info == NULL)
	{
        cout<<"不存在该航班"<<endl;
        dingpiao();
    }
  	cout<<"请输入您需要的票数"<<endl;
    while(1)
	{
    	cin>>need;
    	if(need<=0) 
		{
    		cout<<"请输入正确的订票数"<<endl;
			continue;
		}
    	else break;
	} 
    cout<<"请输入您需要的座位等级"<<endl; 
    while(1)
	{
    	cin>>grade;
    	if(grade!=1&&grade!=2) 
		{
    		cout<<"请输入正确的座位舱等级"<<endl;
			continue;
		}
    	else break;
	} 
    if (grade == 1)
        num = info->lessgrade1;
    else
        num = info->lessgrade2;
    if (need <= num) //订票数小于剩余数 
	{
		vector<int> allocatedSeats;
        int i;
        cout<<"请输入您的名字"<<endl;
    	cin>>name;
       	cout << "请输入您的身份证号码: ";
	    while (true) 
		{
	        cin>>id;
	        if (!isValidID(id)) 
			{
	            cout << "身份证号码格式不正确，请输入18位的数字: ";
	            continue;
	        } 
			else 
			{
	            break;
	        }
	    }
        cout<<"请输入你的飞行时间："<<endl;
		cin>>day; 
        CList head = info->cusLinkList; 
        addlink(head, need, name, id,day,grade);//订票成功，插入成员名单链表
		int seatStatus[info->maxnum] = {0}; // 0表示座位未被占用，1表示已被占用
        // 循环分配座位号
        for (int i = 0; i < need; ++i) 
		{
            int seatNumber;
            do {
                seatNumber = rand() % info->maxnum + 1; 
            } while (seatStatus[seatNumber - 1]); // 检查座位号是否已被占用 
            seatStatus[seatNumber - 1] = 1; // 标记座位号为已占用
            cout << name << "的座位号是" << seatNumber << endl;
        }
        info->less -= need;
        if (grade == 1)
            info->lessgrade1 -= need;
        else
            info->lessgrade2 -= need;
        cout<<"订票成功"<<endl; 
    } 
	else
        {
            char r;
            cout<<"该等级的票不足，以下为该航班乘客信息"<<endl;
            fightinfo(info);//输出该航班的订票信息 
           	cout<<"是否改变订票计划？Y/N"<<endl;
            cin>>r;
            if (r == 'Y' || r == 'y') //改变计划，重新选择航班
			{
                dingpiao();//返回订票主页面 
            } 
			else 
			{
               cout<<"您需要排队等候吗?(Y/N)"<<endl;
            	cin>>r;
                if (r == 'Y' || r == 'y') 
				{
				//不改变计划，排队候票
                cout<<"请输入您的姓名（排队订票）:"<<endl;
                cin>>name;
                cout<<"请输入您的身份证"<<endl; 
                cin>>id;
                if (grade == 1) //进入1等舱排队队列
				{
                    info->wQgrade1 = enqueue(info->wQgrade1, name, need, id);
                } 
				else //进入2等舱排队队列
				{
                    info->wQgrade2 = enqueue(info->wQgrade2, name, need, id);
                }
                cout<<"排队成功"<<endl; 
            } 	
        }
    }

}



//根据自己输入的航班标号查询是否存在并返回节点
hangban *find() 
{
    char number[10];
    int i = 0;
    cout<<"请输入航班编号"<<endl; 
    cin>>number;
    hangban *p = pFlight->next; //头结点的下一个节点开始遍历
    while (p != NULL) 
	{
        if (!strcmp(number, p->hbID))
            return p;
        p = p->next;
    }
    return NULL;
}


void addlink(CList &head, int need, char name[], char id[],char day[],int grade)
//插入到已经订票客户链表 
{
    CList new1;//创建一个新节点 
    new1 = (CNode *) malloc(sizeof(CNode));
    strcpy(new1->name, name);
    strcpy(new1->ID, id);
    strcpy(new1->day,day);
    new1->num = need;
    new1->dengji = grade;
    new1->next = head->next;//头插入法加入成员名单域
    head->next = new1;
    return;
}


void fightinfo(hangban *p) 
{
    cout << "起点  终点  编号  飞机号  飞行日期  总载客  余票量  1级余量  2级余量" << endl;
    cout << p->qidian << "  " << p->zhongdian << "  " << p->hbID << "     " << p->hbnum << "     " << p->day << "     "
                      << p->maxnum << "     " << p->less << "     " << p->lessgrade1 << "     " << p->lessgrade2 << endl;
}



LinkQueue enqueue(LinkQueue &q, char name[], int need, char id[])
//入队，增加排队等候的客户名单域
{
    Phb new1;
    new1 = (Phb) malloc(sizeof(QhbNode));
    strcpy(new1->name, name);
    strcpy(new1->ID, id);
    new1->num = need;
    new1->next = NULL;
    q.rear->next = new1;
    q.rear = new1;
    return q;
}

void tuipiao() //退票功能 
{
    hangban *info;
    int grade;
    CNode *p1, *p2, *head;//p1为遍历指针，p2指向p1的前驱
    char name[20];//客户姓名
    char id[20];//客户身份证
    char day[20];
    info = find();//复制节点信息给info，find函数根据航班编号返回该航班节点的指针
    while (info == NULL)
	{
        cout<<"没有这个航班，请重新输入"<<endl; 
        tuipiao();
    }
    head = info->cusLinkList;//head为该航班的的乘员名单域的头指针
    p1 = head->next;    //带头结点的指针，head->next 开始遍历
    cout<<"请输入你的姓名: "<<endl;
    cin>>name;
    cout<<"请输入你的身份证号码:"<<endl; 
    cin>>id;
    cout<<"请输入飞行时间"<<endl; 
    cin>>day;
    p2 = head;
    while (p1 != NULL) 
	{
		//对比姓名和身份证 
        if ((strcmp(name, p1->name) == 0) && (strcmp(id, p1->ID) == 0)&&(strcmp(day, p1->day) == 0)) break;
        p2 = p1;
        p1 = p1->next;
    }
    if (p1 == NULL) 
	{
        cout<<"对不起，你没有订过票或姓名和身份证,与飞行周期不对应"<<endl;
        return;
    } 
	else 
	{//退票成功
        //从乘客名单域中移除该节点
        grade = p1->dengji;
        p2->next = p1->next;
        //加回该航班的剩余票
        info->less += p1->num;
        if (grade == 1) 
		{
            info->lessgrade1 += p1->num;
        } 
		else 
		{
            info->lessgrade2 += p1->num;
        }
        cout<<name<<"退票成功"<<endl; 
    }
}


int zengjiaFlight() 
{
    hangban *q;
    hangban *p = pFlight;
    int y = 1;
	cout << "请依次输入以下内容" << endl;
    system("pause");
    while (y != 0) 
	{
        q = new hangban;
        if (q == NULL)
            return 0;
        cout << "请输入航班编号" << endl;
        cin >> q->hbID;
        int t = duibiFlight(pFlight, q->hbID);
        if (t == 0) 
		{
            cout << "该航班编号已经存在，请重新输入航班编号" << endl;
            continue;
        }
        cout << "请输入起点站名" << endl;
        cin >> q->qidian;
        cout << "请输入终点站名" << endl;
        cin >> q->zhongdian;
        cout << "请输入型号" << endl;
        cin >> q->hbnum;
        cout << "请输入飞行日期" << endl;
        cin >> q->day;
        cout << "请输入乘客定额" << endl;
        cin >> q->maxnum;
        q->less = q->maxnum;
        cout << "请输入1等票数目" <<endl;
        cin >> q->lessgrade1;
        q->lessgrade2 = q->maxnum - q->lessgrade1;
        q->next = p->next;
        p->next = q;
        cout << "是否继续录入航班信息（任意数字继续，0表示停止）。\n";
        cout << "请输入：";
        cin >> y;
    }
    return 0;
}


int shanchuFlight() 
{
    string ID;
    cout << "请输入航班ID" << endl;
    cin >> ID;
    hangban *pre = pFlight;
    hangban *p = pre->next;
    while (p != NULL) 
	{
        if (ID == p->hbID) 
		{
            pre->next = p->next;
            delete p;
            return 1;
        }
        pre = p;
        p = p->next;
    }
    return 0;
}


int duibiFlight(hangban *flight, char ID[])
//对比航班的编号，防止增加航班时，出现重复 
{
    hangban *p = flight;
    while (p != NULL)
	{
        if (!strcmp(ID, p->hbID)) 
		{
            return 0;//航班ID重复
        }
        p = p->next;
    }
    //ID不重复
    return 1;

}


void sousuoFace() 
{
    int i;
    cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                    ";
        cout<<"搜索窗口"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             1.搜索航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.返回上一级                    "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-2,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
    cin >> i;
    switch (i) {
        case 1:
            sousuoflight();
            system("pause");
            sousuoFace();
            break;
        case 2:
            menu();
            break;
        default:
            sousuoFace();
    }
}


void sousuoflight() 
{
    string qidian, zhongdian;
    int flag = 0;
    cout << "请输入起点站名:";
    cin >> qidian;
    cout << "请输入终点站名:";
    cin >> zhongdian;
    hangban *p = pFlight->next;
    cout << "起点  终点  编号  飞机号  飞行日期  总载客  余票量  1级余量  2级余量" << endl;
    while (p != NULL) {
        if (qidian == p->qidian && zhongdian == p->zhongdian) 
		{
            flag = 1;  
            cout << p->qidian << "  " << p->zhongdian << "  " << p->hbID << "     " << p->hbnum << "     " << p->day << "     "
                      << p->maxnum << "     " << p->less << "     " << p->lessgrade1 << "     " << p->lessgrade2 << endl;
        }
        p = p->next;
    }
    cout <<endl <<endl;
    if (flag == 0)
        cout << "抱歉！不存在该航班！" << endl;
}


void dingpiaoFace() 
{
    int i;
    cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                    ";
        cout<<"订票窗口"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"             1.确认订票                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.搜索航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             3.浏览航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             4.返回上级菜单                  "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-4,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
    cin >> i;
    switch (i) {
        case 1:
            dingpiao();
            system("pause");
            dingpiaoFace();
            break;
        case 2:
            sousuoflight();
            system("pause");
            dingpiaoFace();
            break;
        case 3:
            printfall(pFlight);
            system("pause");
            dingpiaoFace();
            break;
        case 4:
            menu();
            break;
        default:
            dingpiaoFace();
    }
}



void tuipiaoFace()//退票模块界面
{
	int i;
    cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                    ";
        cout<<"退票窗口"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             1.办理退票                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.返回上级菜单                  "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-2,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
        cin >> i;
    switch (i) 
	{
        case 1:
            tuipiao();
            system("PAUSE");
            tuipiaoFace();
            break;
        case 2:
            menu();
            break;
        default:
            tuipiaoFace();
    }
}


void pricusInfo() 
{
    CList p;
    hangban *info;
    info = find();
    if (info == NULL) 
	{
        cout << "没有这个航班" << endl;
        return;
    }
    // 头结点的下一个节点开始遍历
    p = info->cusLinkList->next;
    if (p != NULL) 
	{
        cout << "客户姓名   订票数额   舱位等级       身份证号  " << endl;
        while (p) 
		{
            cout << p->name << "\t\t" << p->num << "\t" << p->dengji << "\t" <<p->ID<< endl;
            p = p->next;
      	}
    } 
	else 
	{
        cout << "该航班没有客户信息!!" << endl;
    }
}

void printfall(hangban *pflight) 
{
    hangban *p;
    p = pflight->next;
    cout << "起点  终点  编号  飞机型号  飞行日期  总载客  余票量  1级余量  2级余量 " << endl;
    while (p != NULL) 
	{
        cout << p->qidian << "  " << p->zhongdian << "  " << p->hbID << "      " << p->hbnum << "     " << p->day << "   "
                  << p->maxnum << "     " << p->less << "     " << p->lessgrade1 << "      " << p->lessgrade2 <<"      "<< endl;
        p = p->next;
    }
    cout << endl << endl;
}

int yonghudenglu()
{
		string account1="黄暄";
		string passwords1="239007045";
		string account2="王峰";
		string passwords2="239007038";
		string account3="邓浩宇";
		string passwords3="239007049";
		string account4="陈记德";
		string passwords4="239007037";
		string account5="徐嘉诚";
		string passwords5="239007036";
		string account;
		string passwords;
		cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                  ";
        cout<<"用户登录页面"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"      请输入正确的账号与密码以进入系统       "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"          (返回首页请输入exit)               "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入账号:"<<endl;
		cout<<"                                  ";
		cout<<"account:"; 
		cin>>account;
		cout<<"                                  ";
		if(account=="exit")
		{
			denglu();
		 } 
		cout<<"请输入密码:"<<endl;
		cout<<"                                  ";
		cout<<"password:"; 
		cin>>passwords;
		if((account==account1&&passwords==passwords1)||(account==account2&&passwords==passwords2)||
		(account==account3&&passwords==passwords3)||(account==account4&&passwords==passwords4)||
		 (account==account5&&passwords==passwords5))
		{
			menu();
		}
		else
		{
			cout<<"                                  ";
			cout<<"输入错误，按任意键重新输入"<<endl; 
		}
} 



int guanliyuandenglu()
{
	string passwords="123456";
	string shuru;
		cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                 ";
        cout<<"管理员登录页面"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"        请输入管理员密码以验证身份           "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"          (返回首页请输入exit)               "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入密码:"<<endl;
		cout<<"                                  ";
		cout<<"password:"; 
		cin>>shuru;
		if(shuru==passwords)
		{
			cout<<"                                  ";
			cout<<"管理员身份验证成功！"<<endl; 
			guanliyuanyemian();
		}
		else if(shuru=="exit") 
		{
			denglu();
		} 
		else
		{
			cout<<"                                  ";
			cout<<"身份验证失败，按任意键重新输入"<<endl; 
			system("pause");
			guanliyuandenglu(); 
		 } 
} 



int guanliyuanyemian()
{
	    int i;
	    do{
	    cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                                    ";
        cout<<"管理员页面"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             1.插入航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.删除航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             3.浏览航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             4.查询客户信息                  "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             5.返回首页                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-2,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
        cin >> i;
        switch (i) {
            case 1:
                zengjiaFlight();
                guanliyuanyemian();
            break;
                break;
            case 2:
                if (1 == shanchuFlight()) 
				{
                	cout << "删除成功" << endl;
                } 
				else 
				{
                	cout << "没有这个航班，删除失败！" << endl;
                }
                system("pause");
            	guanliyuanyemian();
                break;
            case 3:
            	printfall(pFlight); 
                system("pause");
                guanliyuanyemian(); 
                break;
	        case 4:
	        	pricusInfo();
	        	system("pause");
	        	guanliyuanyemian(); 
	        	break;
			case 5:
        		denglu();
	            break; 
            default:
                guanliyuanyemian(); 
        }
    } while (true); 
}



int denglu() 
{
    int i;
    do {
    	cout<<endl<<endl<<endl;
    	cout<<"title：航空客运订票系统-江苏师范大学数据结构课题作业-24.5"<<endl;
		cout<<"author：黄暄，王锋，徐嘉诚,邓浩宇，陈记德"; 
		cout<<endl<<endl<<endl;
        cout<<"                                                     ";
        cout<<"登录页面"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             1.用户登录                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.管理员登录                    "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"                                             "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-2,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
        cin >> i;
        switch (i) {
            case 1:
                yonghudenglu();
                system("pause");
                break;
            case 2:
                guanliyuandenglu();
                system("pause");
                break;
            default:
                denglu();
        }
    } while (true); 
}


int menu() 
{
    int i;
    do {
		cout<<endl<<endl<<endl<<endl<<endl;
        cout<<"                                             ";
        cout<<"欢迎来到航空客运订票系统"<<endl; 
        cout<<"                                  ";
        cout<<"-----------------------------------------------"<<endl;
        cout<<"                                  ";
        cout<<"|"<<"             1.查询航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             2.搜索航班                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             3.订票                          "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             4.退票                          "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"|"<<"             5.退出系统                      "<<"|"<<endl;
		cout<<"                                  ";
		cout<<"-----------------------------------------------"<<endl;
		cout<<"                                  ";
		cout<<"请输入您的选择(1-5,否则无效！)"<<endl; 
		cout<<"                                  ";
		cout<<"您的选择是:";    
        cin >> i;
        switch (i) {
            case 1:
                printfall(pFlight); 
                system("pause");
                menu();
                break;
            case 2:
                sousuoFace();
                system("pause");
                menu();
                break;
            case 3:
                dingpiaoFace();  // 订票界面
                system("pause");
                menu();
                break; 
            case 4:
                tuipiaoFace();
				system("pause");
                menu();
                break; 
            case 5:
                denglu();
            default:
                menu(); 
        }
    } while (true); 
}

int main() 
{
	system("color F1");
    initFlight();//初始化航班链表 
    Create(flight1);//将航标信息插入链表 
    denglu();    //菜单
    return 0;
}



