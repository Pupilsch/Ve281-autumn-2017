#include <iostream>
#include <getopt.h>
#include <string>
#include <stdlib.h>
#include <queue>
#include <map>
#include <limits.h>
#include <algorithm>

using namespace std;

struct order{
    int id;
    int time_stamp;
    string name;
    bool buy;
    string equity;
    int price;
    int share;
    int duration;
    order(int id,int time_stamp,string name, bool buy,string equity,int price,int share,int duration){
        this->id=id;
        this->time_stamp=time_stamp;
        this->name=name;
        this->buy=buy;
        this->equity=equity;
        this->price=price;
        this->share=share;
        this->duration=duration;
    }
};

struct traveler{
    int buy_day=-1;
    int sell_day=-1;
    int max=INT_MIN;
    int min=INT_MAX;
};

struct compare_buy
{
    bool operator()(order *a, order *b) const
    {
        if(a->price==b->price) return a->id<b->id;
        else return  a->price<b->price;
    }
};

struct compare_sell
{
    bool operator()(order *a, order *b) const
    {
        if(a->price==b->price) return a->id>b->id;
        else return  a->price>b->price;
    }
};

struct equity{
    string name;
    traveler trav;
    vector<int> median;
    priority_queue<order*,vector<order*>,compare_buy> *buy_pq;
    priority_queue<order*,vector<order*>,compare_sell> *sell_pq;
    equity(string name){
        this->name=name;
    }
};

struct client{
    string name;
    int buy_num;
    int sell_num;
    int net;
    client(string name){
        this->name=name;
        this->buy_num=0;
        this->sell_num=0;
        this->net=0;
    }
};

void TTT(order* new_order,equity* eq){
    if(new_order->buy){
        if((eq->trav.max<(new_order->price-eq->trav.min)) && eq->trav.min!=INT_MAX){
            eq->trav.max=new_order->price-eq->trav.min;
            eq->trav.sell_day=new_order->time_stamp;
        }
    }
    if(!new_order->buy){
        if(new_order->price<eq->trav.min){
            eq->trav.min=new_order->price;
            eq->trav.buy_day=new_order->time_stamp;
        }
    }
}

void Handle_deal(map<string,client*> &cli,map<string,equity*> &deal,order *new_order,int* commission,int* money_transfer,int* num_trade,int* num_share,int time,bool v){
    map<string,equity*>::iterator iter;
    iter=deal.find(new_order->name);
    equity* eq=iter->second;
//    if(new_order->duration==0){//IOC part
//        if(new_order->buy)
//    }
    if (new_order->buy){
        eq->buy_pq->push(new_order);
        TTT(new_order,eq);
    }
    else {
        eq->sell_pq->push(new_order);
        TTT(new_order,eq);
    }
    while(!eq->buy_pq->empty() && !eq->sell_pq->empty() && eq->sell_pq->top()->price<=eq->buy_pq->top()->price){
        order* buy_top=eq->buy_pq->top();
        order* sell_top=eq->sell_pq->top();
        if(buy_top->duration!=-1 && ((buy_top->time_stamp+buy_top->duration)<=time)){
            eq->buy_pq->pop();
            continue;
        }
        if(sell_top->duration!=-1 && ((sell_top->time_stamp+sell_top->duration)<=time)){
            eq->sell_pq->pop();
            continue;
        }
        //real trading part
        int deal_price;
        if(buy_top->id<sell_top->id) deal_price=buy_top->price;
        else deal_price=sell_top->price;
        int min_share=min(buy_top->share,sell_top->share);
        buy_top->share-=min_share;
        sell_top->share-=min_share;
        eq->median.push_back(deal_price);
        *commission+=((min_share*deal_price)/100)*2;
        *money_transfer+=min_share*deal_price;
        *num_trade+=1;
        *num_share+=min_share;
        map<string,client*>::iterator buy_iter=cli.find(buy_top->name);
        map<string,client*>::iterator sell_iter=cli.find(sell_top->name);
        client* buyer=buy_iter->second;
        client* seller=sell_iter->second;
        buyer->buy_num+=1;
        seller->sell_num+=1;
        buyer->net-=min_share*deal_price;
        seller->net+=min_share*deal_price;
        if(v) cout<<buy_top->name<<" purchased "<<min_share<<" of "<<eq->name<<" from "<<sell_top->name<<" for $"<<deal_price<<"/share"<<endl;
        if(buy_top->share==0) eq->buy_pq->pop();
        if(sell_top->share==0) eq->sell_pq->pop();
    }
}

void Median(map<string,equity*> &deal,int time){
    int median;
    map<string,equity*>::iterator itr=deal.begin();
    while(itr!=deal.end()){
        equity* eq2=itr->second;
        if(!eq2->median.empty()) {
            sort(eq2->median.begin(), eq2->median.end());//may be wrong
            int size=eq2->median.size();
            if(size%2==0){
                median=((eq2->median[size/2-1]+eq2->median[size/2])/2);
            }
            else{
                median=eq2->median[size/2];
            }
            cout<<"Median match price of "<<eq2->name<<" at time "<<time<<" is $"<<median<<endl;
        }
        itr++;
    }
}

void Midpoint(map<string,equity*> &deal,int time){
    map<string,equity*>::iterator itr=deal.begin();
    while(itr!=deal.end()){
        equity* eq3=itr->second;
        if(!eq3->buy_pq->empty() &&!eq3->sell_pq->empty()){
            int midpoint_price=(eq3->buy_pq->top()->price+eq3->sell_pq->top()->price)/2;
            cout<<"Midpoint of "<<eq3->name<<" at time "<<time<<" is "<<midpoint_price<<endl;
        } else{
            cout<<"Midpoint of "<<eq3->name<<" at time "<<time<<" is undefined"<<endl;
        }
        itr++;
    }
}

void End_of_time(map<string,equity*> &deal,bool m,bool p,int time){
    map<string,equity*>::iterator itr=deal.begin();
    while(itr!=deal.end()){
        equity* eq1=itr->second;
        if(!eq1->buy_pq->empty()){
            while((eq1->buy_pq->top()->time_stamp+eq1->buy_pq->top()->duration)<=time && !eq1->buy_pq->empty()){
                eq1->buy_pq->pop();
            }
            while((eq1->sell_pq->top()->time_stamp+eq1->sell_pq->top()->duration)<=time && !eq1->sell_pq->empty()){
                eq1->sell_pq->pop();
            }
        }
        itr++;
    }//clean the expired order.
    if(m){
        Median(deal,time);
    }
    if(p){
        Midpoint(deal,time);
    }
}

int main(int argc, char* argv[]) {
    int *commission=new int(0);
    int *money_transfer=new int(0);
    int *num_trade=new int(0);
    int *num_share=new int(0);
    int time=0;
    bool v=false;
    bool m=false;
    bool p=false;
    bool t=false;
    bool g=false;
    struct option long_options[]={
            {"verbose",0,NULL,'v'},
            {"median",0,NULL,'m'},
            {"midpoint",0,NULL,'p'},
            {"transfers",0,NULL,'t'},
            {"ttt",1,NULL,'g'},
            {NULL,0,NULL,0}
    };
    int c;
    string s;
    vector<string> ttt_queue;
    map<string,client*> cli;//map for different client
    while((c=getopt_long(argc,argv,"vmptg:",long_options,NULL))!=-1){
        switch (c)
        {
            case 'v':
                v=true;
                break;
            case 'm':
                m=true;
                break;
            case 'p':
                p=true;
                break;
            case 't':
                t=true;
                break;
            case 'g':
                g=true;
                s=optarg;
                ttt_queue.push_back(s);
                break;
        }
    }
    map<string,equity*> deal;//map for different equity
    int id=0;
    while(!cin.fail()){
        id+=1;
        int time_stamp;
        string name;
        string buy_s;
        bool buy;
        string equity_n;
        string price_s;
        string price_final;
        int price;
        string share_s;
        string share_final;
        int share;
        int duration;
        cin>>time_stamp;
        cin>>name;
        cin>>buy_s;
        cin>>equity_n;
        cin>>price_s;
        cin>>share_s;
        cin>>duration;
        if(buy_s=="BUY") buy=true;
        else buy= false;
        price_final=price_s.substr(1, sizeof(price_s)-1);
        price=atoi(price_final.c_str());
        share_final=share_s.substr(1, sizeof(price_s)-1);
        share=atoi(share_final.c_str());
        order *new_order= new order(id,time_stamp,name,buy,equity_n,price,share,duration);
        if(new_order->time_stamp!=time){
            End_of_time(deal,m,p,time);
            time=new_order->time_stamp;
        }
        if(deal.find(equity_n)==deal.end()){
            equity *new_equity=new equity(equity_n);
            deal.insert(map<string,equity*>::value_type(new_order->equity,new_equity));
        }
        if(cli.find(new_order->name)==cli.end()){
            client *new_client=new client(new_order->name);
            cli.insert(map<string,client*>::value_type(new_order->name,new_client));
        }
        Handle_deal(cli,deal,new_order,commission,money_transfer,num_trade,num_share,time,v);
    }
    cout<<"---End if Day---"<<endl;
    cout<<"Commission Earning: $"<<*commission<<endl;
    cout<<"Total Amount of Money Transferred: $"<<*money_transfer<<endl;
    cout<<"Number of Completed Trades: "<<*num_trade;
    cout<<"Number of Shares Traded: "<<*num_share;
    if(t){
        map<string,client*>::iterator iter;
        iter=cli.begin();
        while(iter!=cli.end()){
            client* cl=iter->second;
            cout<<cl->name<<" bought "<<cl->buy_num<<" and sold "<<cl->sell_num<<" for a net transfer of $"<<cl->net<<endl;
            iter++;
        }
    }
    if(g){
        map<string,equity*>::iterator itr;
        while(!ttt_queue.empty()){
            string tmp;
            tmp=ttt_queue.back();
            ttt_queue.pop_back();
            itr=deal.find(tmp);
            equity* temp=itr->second;
            cout<<"Time travelers would buy "<<temp->name<<" at time: "<<temp->trav.buy_day<<" and sell it at time: "<<temp->trav.sell_day<<endl;
        }
    }
    return 0;
}