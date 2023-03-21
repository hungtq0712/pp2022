#include "pugixml-1.13/src/pugixml.hpp"
#include <curses.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <map>
#include<malloc.h>
#include <list>
char *output;
long lengfile=0;
using namespace std;
struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return strcmp(a, b) < 0;
   }
};

class Point{
    public:
        double x;
        double y;
        Point(){
        }
        Point(double x,double y){
            this->x=x;
            this->y=y;
        }
        double Leng(double x1, double y1, double x2, double y2){
            return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        }

};
class T{
    public:
        list<char*>adjJunction;
		list<char*>adjEdge;
};
//ax+by=c;
class Straight{
    public:
        double a,b,c;
        double anpha;
        Straight(){
        }
        Straight(double a,double b,double c){
            this->a=a;
            this->b=b;
            this->c=c;
        }
        Straight(double x1, double y1, double x2, double y2){
            this->anpha=atan2 (y2-y1,x2-x1);
            if(y1==y2){
                this->a=0;
                this->b=60;
                this->c=y1*60;
            }
            else{
                this->b=60;
                this->a=(c+c-b*(y1+y2))/(x1+x2);
                this->c=this->a*x1+60*y1;
            }
        }
        Point* nextPoint(double x,double y,double dist){
            x=x+dist*cos(anpha);
            y=y+dist*sin(anpha);
            return new Point(x,y);
        }
};

class Junction:public T{
    public:
        const char* id;
        const char* name;
        const char* type;
        const char* shape;
        int id_lane;
        Junction(){
        }
        Junction(const char* id, const char* type, const char* shape){
            this->id=id;
            this->type=type;
            this->shape=shape;
            this->id_lane=0;
            char s[40];
            strcpy( s,id);
            int i=0;
            for(i=0;i<40;i++)
                if(s[i]=='_'){
                    s[i]='\0';
                    break;
                }
            this->name=s;
            for(int j=i+1;j<strlen(id);j++){
                this->id_lane=(this->id_lane)*10+id[j]-'0';
            }
        }
};

class Crossing_Junction:public Junction{
    public:
        const char* from;
        const char* to;
        Crossing_Junction():Junction(){
        }
        Crossing_Junction(const char* id, const char* type, const char* shape,const char* from,const char* to):Junction(id,type,shape){
            this->from=from;
            this->to=to;
        }
};


class Start_Junction:public Junction{
    public:
        const char* id_edge;
        Start_Junction():Junction(){
        }
        Start_Junction(const char* id, const char* type, const char* shape,const char* id_edge):Junction(id,type,shape){
            this->id_edge=id_edge;
        }
};
class End_Junction:public Junction{
     public:
        const char* id_edge;
        End_Junction():Junction(){
        }
        End_Junction(const char* id, const char* type, const char* shape, const char* id_edge):Junction(id,type,shape){
            this->id_edge=id_edge;
        }
};
class Edge:public T{
	public:
		const char* id;
		const char* name;
		const char* from;
		const char* to;
		const char* shape;
		int id_lane;
		list<Crossing_Junction*>junction_crossing;
		Edge(){
		}
		Edge(const char* id, const char* from, const char* to,	const char* shape,const char* lane){
            this->id=id;
            this->name=id;
            this->from=from;
            this->to=to;
            this->shape=shape;
            this->id_lane=0;
            int i=0;
            for(i=0;i<strlen(lane);i++)
                if(lane[i]=='_'){
                    break;
                }
            for(int j=i+1;j<strlen(lane);j++){
                this->id_lane=(this->id_lane)*10+lane[j]-'0';
            }
		}
};
class Graph{
    public:
        map<char*,Edge*,cmp_str> edges;
        vector<Start_Junction*> start_junctions;
        vector<End_Junction*> end_junctions;
        map<char*,Junction*,cmp_str> junctions;
        map<char*,Crossing_Junction*,cmp_str> crossing__junctions;
};

class Graph *graph;
void printEdge(char* nameOfEdge){
    Edge *e=graph->edges.find(nameOfEdge)->second;
	cout<<e->id<<" "<<e->from<<" "<<e->to<<" "<<e->shape<<endl;
}
int findindex(char* name){
    int index=0;
    for(int i=0;i<strlen(name);i++){
        if(name[i]<'0' || name[i]>'9')
            continue;
        index=index*10+name[i]-'0';
    }
    return index;
}
void printStart(){
    vector<Start_Junction*>::iterator it;
    for (it = graph->start_junctions.begin() ; it != graph->start_junctions.end(); it++)
        cout<<(*it)->id<<" ";
    cout<<endl;
}
void printEnd(){
    vector<End_Junction*>::iterator it;
    for (it = graph->end_junctions.begin() ; it != graph->end_junctions.end(); it++)
         cout<<(*it)->id<<" ";
    cout<<endl;
}
double splitNumber(char* shape,int &i){
    double x1=0;
    bool lock=1;
    int sign=1;
    int k=10;
    if(i>strlen(shape))
        return 0.0;
    if(shape[i]=='-'){
        sign=-1;
        i++;
    }
    while(shape[i]!=',' && shape[i]!=' ' && shape[i]!='\0'){
        //cout<<shape[i]<<endl;
        if(shape[i]=='.'){
            lock=0;
            i++;
            continue;
        }
        if(lock==1)
            x1=x1*10+(double)(shape[i]-'0')*sign;
        else{
            x1=x1+(double)(shape[i]-'0')/k*sign;
            k=k*10;
        }
        i++;
    }
    i++;
    return x1;
}

void splitShape(const char *e,double x1,double y1,double x2,double y2,double x,int &n){
    //int n_old=n;
    //lengfile=lengfile+sprintf(output+lengfile,"_%.2lf,%.2lf",x,y);
    Straight *line= new Straight(x1,y1,x2,y2);
    double x1_1,y1_1;
    if(x1<x2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(x1_1<x2){
            lengfile=lengfile+sprintf(output+lengfile,"%s %d_%.2lf,%.2lf_%.2lf,%.2lf\n",e,n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else if (x1>x2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(x1_1>x2){
            lengfile=lengfile+sprintf(output+lengfile,"%s %d_%.2lf,%.2lf_%.2lf,%.2lf\n",e,n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else if(y1<y2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(y1_1<y2){
            lengfile=lengfile+sprintf(output+lengfile,"%s %d_%.2lf,%.2lf_%.2lf,%.2lf\n",e,n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else{
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(y1_1>y2){
            lengfile=lengfile+sprintf(output+lengfile,"%s %d_%.2lf,%.2lf_%.2lf,%.2lf\n",e,n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }

    lengfile=lengfile+sprintf(output+lengfile,"%s %d_%.2lf,%.2lf_%.2lf,%.2lf\n",e,n,x1,y1,x2,y2);
    //cout<<"_TBC\n";
    n++;
}
void splitShape(double x1,double y1,double x2,double y2,double x,int &n){
    //int n_old=n;
    Straight *line= new Straight(x1,y1,x2,y2);
    double x1_1,y1_1;
    if(x1<x2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(x1_1<x2){
            printf(" %d_%.2lf,%.2lf_%.2lf,%.2lf",n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else if(x1>x2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(x1_1>x2){
            printf(" %d_%.2lf,%.2lf_%.2lf,%.2lf",n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else if(y1<y2){
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(y1_1<y2){
            printf(" %d_%.2lf,%.2lf_%.2lf,%.2lf",n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }
    else{
        Point *p=line->nextPoint(x1,y1,x);
        x1_1=p->x;
        y1_1=p->y;
        while(y1_1>y2){
            printf(" %d_%.2lf,%.2lf_%.2lf,%.2lf",n,x1,y1,x1_1,y1_1);
            n++;
            x1=x1_1;
            y1=y1_1;
            p=line->nextPoint(x1,y1,x);
            x1_1=p->x;
            y1_1=p->y;
        }
    }

    printf(" %d_%.2lf,%.2lf_%.2lf,%.2lf",n,x1,y1,x2,y2);
    //cout<<"_TBC\n";


}

void splitStart(double x, char* name){
    double x1=0,x2=0,y1=0,y2=0;
    char* shape=(char*)graph->edges.find(name)->second->shape;
    //cout<<shape<<endl;
    int i=0,n=0;
    x1=splitNumber(shape,i);
    y1=splitNumber(shape,i);
    x2=splitNumber(shape,i);
    y2=splitNumber(shape,i);
    cout<<name;
    splitShape(x1,y1,x2,y2,x,n);
    cout<<"\n";
}

void splitJunction(double x, char* name){
    double x1=0,x2=0,y1=0,y2=0;
    char *shape=(char*)graph->crossing__junctions[name]->shape;
    int i=0;
    //cout<<"name="<<(char*)graph->crossing__junctions[name]->id<<"\n";
    //cout<<shape<<endl;
    x1=splitNumber(shape,i);
    y1=splitNumber(shape,i);
    int n=0;
    while(i<strlen(shape)){
        x2=splitNumber(shape,i);
        y2=splitNumber(shape,i);
        splitShape(x1,y1,x2,y2,x,n);
        x1=x2;
        y1=y2;
    }
    cout<<"\n";
}
void trajectory(double x, double L, char* shape){
    int i=0;
    x=L-x;
    double x1=0,x2=0,y1=0,y2=0;
    x1=splitNumber(shape,i);
    y1=splitNumber(shape,i);
    x2=splitNumber(shape,i);
    y2=splitNumber(shape,i);
    Straight *line= new Straight(x1,y1,x2,y2);
    double x1_1,y1_1;
    Point *p=line->nextPoint(x1,y1,x);
    x1_1=p->x;
    y1_1=p->y;
    if(x1_1<x2)
        printf("%.2lf,%.2lf_%.2lf,%.2lf\n",x1,y1,x1_1,y1_1);
    else
        printf("%.2lf,%.2lf_%.2lf,%.2lf\n",x1,y1,x2,y2);
}

void printStartEdges(){
    vector<Start_Junction*>::iterator it;
    for (it = graph->start_junctions.begin() ; it != graph->start_junctions.end(); it++){
        char* id_e=(char*)(*it)->id_edge;
        printEdge( id_e);
    }
}

void printEdge2Juncs(char* nameOfEdge){
    list<Crossing_Junction*>::iterator it;
    Edge *e=graph->edges[nameOfEdge];
    cout<<e->id<<" "<<e->shape;
    for (it=e->junction_crossing.begin() ; it != e->junction_crossing.end(); it++)
        cout<<" "<<(*it)->id<<" "<<(*it)->shape;
    cout<<endl;
}

list<char*>adjEdge;

double lengShape(char* shape){
    double x1=0,x2=0,y1=0,y2=0;
    int i=0;
    double sum=0.0;
    x1=splitNumber(shape,i);
    y1=splitNumber(shape,i);
    while(i<strlen(shape)){
        x2=splitNumber(shape,i);
        y2=splitNumber(shape,i);
        sum+=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        x1=x2;
        y1=y2;
    }
    return sum;
}
bool isEndJunc(const char *s){
    vector<End_Junction*>:: iterator it;
    bool find=false;
    for(it=graph->end_junctions.begin(); it!=(graph->end_junctions.end()); it++){
        if(strcmp(s,(*it)->id_edge)==0)
            find=true;
    }

    return find;
}
void write_splite( char *e, double x){
    double x1=0,x2=0,y1=0,y2=0;
    char* shape;
    if(e[0]=='E' || e[1]=='E')
        shape=(char*)graph->edges.find(e)->second->shape;
    else
        shape=(char*)graph->junctions.find(e)->second->shape;

    int i=0;
    x1=splitNumber(shape,i);
    y1=splitNumber(shape,i);
    int n=0;

    while(i<strlen(shape)){
        x2=splitNumber(shape,i);
        y2=splitNumber(shape,i);
        splitShape(e,x1,y1,x2,y2,x,n);
        x1=x2;
        y1=y2;
    }

}
void allParts(double x){

    lengfile=0;
    output=(char*)malloc(900000*sizeof(char));
    list< char*>::iterator it;
    vector<Start_Junction*>:: iterator it2;
    queue < string> T,N;
    map < string,bool> S;
    string e;
    for(it2=graph->start_junctions.begin(); it2!=(graph->start_junctions.end()); it2++){
        T.push((char*)(*it2)->id);
        S.insert ( pair<string,bool>((*it2)->id,1) );
        //cout<<(*it2)->id_edge<<endl;
    }
    while(!T.empty()){

        e=T.front();
        T.pop();

        if(!isEndJunc(e.c_str())){
            //write
            //cout<<"run\n";
            write_splite((char*)e.c_str(),x);
            if(S.find(e.c_str())==S.end() )
                S.insert ( pair< string,bool>(e,1) );

            //cout<<graph->junctions[(char*)e]->adjEdge.size()<<" "<<graph->junctions[(char*)e]->adjJunction.size()<<"\n";
            if(e[0]=='J' || e[1]=='J'){

                if(graph->junctions[(char*)e.c_str()]->adjEdge.size()!=0){
                    for(it=graph->junctions[(char*)e.c_str()]->adjEdge.begin(); it != graph->junctions[(char*)e.c_str()]->adjEdge.end(); it++){
                        if(S.find(*it)==S.end() ){
                                T.push(*it);
                                 S.insert ( pair< char*,bool>(( char*)*it,1) );
                            }
                    }
                }
                if(graph->junctions[(char*)e.c_str()]->adjJunction.size()!=0){
                    for(it=graph->junctions[(char*)e.c_str()]->adjJunction.begin(); it != graph->junctions[(char*)e.c_str()]->adjJunction.end(); it++){
                            if(S.find(*it)==S.end() ){
                                T.push(*it);
                                S.insert ( pair< char*,bool>(( char*)*it,1) );
                            }
                    }
                }

            }
            else{

                if(graph->edges[(char*)e.c_str()]->adjEdge.size()!=0){

                    for(it=graph->edges[(char*)e.c_str()]->adjEdge.begin(); it != graph->edges[(char*)e.c_str()]->adjEdge.end(); it++){
                        if(S.find(*it)==S.end() ){
                                T.push(*it);
                                 S.insert ( pair< char*,bool>(( char*)*it,1) );
                        }
                    }
                }
                if(graph->edges[(char*)e.c_str()]->adjJunction.size()!=0){
                    for(it=graph->edges[(char*)e.c_str()]->adjJunction.begin(); it != graph->edges[(char*)e.c_str()]->adjJunction.end(); it++){
                            if(S.find(*it)==S.end() ){
                                T.push(*it);
                                S.insert ( pair< char*,bool>(( char*)*it,1) );
                            }
                    }
                }

            }


        }
    }
    FILE *fFile=fopen("AllParts.txt","w");
	fwrite(output,lengfile, 1,fFile);
	fclose(fFile);

    free(output);
}

int main(){
    char *nameXML="vd013.net.xml";
    graph=new Graph();
    pugi::xml_document doc;
    if (!doc.load_file(nameXML)) {
        cout<<"Can't read\n";
        return NULL;
    }
    //Doc va xu ky file
    pugi::xml_node panels = doc.child("net");
    for (pugi::xml_node panel = panels.child("edge"); panel; panel = panel.next_sibling("edge"))
    {
        //them junction
        if(strcmp(panel.first_attribute().next_attribute().name(),"from")!=0){

            for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
            {
                pugi::xml_attribute attr=child.first_attribute().next_attribute().next_attribute();
                if(strcmp(attr.name(),"disallow")!=0 || strcmp(attr.value(),"pedestrian")!=0 )
                    continue;
                Junction *j=new Junction(panel.first_attribute().value(),(const char*)"",child.last_attribute().value());
                graph->junctions.insert (   pair<char*,Junction*>((char*)panel.first_attribute().value(),j) );

                //cout<<(char*)panel.first_attribute().value()<<",shape="<<child.last_attribute().value()<<endl;
                break;
            }

            continue;
        }
        //them edges

        pugi::xml_attribute attr = panel.first_attribute();
        const char* e_id=attr.value();
        attr = attr.next_attribute();
		const char* e_from=attr.value();
		attr = attr.next_attribute();
		const char* e_to=attr.value();
		attr = attr.next_attribute();
		const char *lane;
		const char* e_shape;

        for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
        {
            attr=child.first_attribute().next_attribute().next_attribute();
            if(strcmp(attr.name(),"disallow")!=0 || strcmp(attr.value(),"pedestrian")!=0 )
                continue;
            e_shape=child.last_attribute().value();
            lane=child.first_attribute().value();
            break;
        }
        Edge *e= new Edge(e_id,e_from,e_to,e_shape,lane);
        graph->edges.insert (  pair<char*,Edge*>((char*)e_id,e) );
        //cout<<graph->edges.find((char*)e_id)->second->id<<"\n";
        //cout<<graph->edges.find((char*)e_id)->second->id;

    }
    //cout<<graph->edges.find("E0")->second->id;

    for (pugi::xml_node panel = panels.child("junction"); panel; panel = panel.next_sibling("junction")){
        if(strcmp(panel.first_attribute().next_attribute().value(),"dead_end")!=0){
            Junction *j=new Junction(panel.first_attribute().value(),panel.first_attribute().next_attribute().value(),panel.last_attribute().value());
            graph->junctions.insert (  pair<char*,Junction*>((char*)panel.first_attribute().value(),j) );
            continue;
        }
        pugi::xml_attribute attr = panel.first_attribute();

        double x1,x2,y1,y2;
        const char* j_shape=attr.next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().value();
        //cout<<j_shape<<"\n";
        int i=0;
        x1=splitNumber((char*)j_shape,i);
        y1=splitNumber((char*)j_shape,i);
        x2=splitNumber((char*)j_shape,i);
        y2=splitNumber((char*)j_shape,i);
        bool isStart = false;
        if(x1<x2)
            isStart=true;
        else if(x1==x2){
            if(y1<y2)
                isStart=true;
            else
                isStart=false;
        }
        else
            isStart = false;
        //cout<<incLanes<<endl;
        //cout<<graph->edges.find(incLanes)->second->id;
        //cout<<attr.value()<<" "<<isStart<<" "<<x1<<" "<<x2<<" "<<y1<<" "<<y2<<"\n";
        if(isStart == true){
            int find_from=0;
            map<char*,Edge*,cmp_str> ::iterator it;
            for(it=graph->edges.begin(); it!=(graph->edges.end()); it++){
                if(strcmp(it->second->from,panel.first_attribute().value())==0  ){
                    find_from=1;
                    break;
                }
            }

            //cout<<"from="<<panel.first_attribute().value()<<" "<<find_from<<endl;
            if(find_from==1){
                //start edge
                attr = panel.first_attribute();
                Start_Junction *j=new Start_Junction(attr.value(),attr.next_attribute().value(),attr.next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().value(),it->first);
                graph->start_junctions.push_back( j);
                if(graph->junctions.find((char*)panel.first_attribute().value())==graph->junctions.end()){
                    graph->junctions.insert (  pair<char*,Junction*>((char*)attr.value(),j) );
                    //cout<<attr.value()<<"\n";
                }
                graph->junctions[(char*)attr.value()]->adjEdge.push_back(it->first);
            }
        }
        else{
            int find_to=0;
            map<char*,Edge*,cmp_str> ::iterator it2;
            for(it2=graph->edges.begin(); it2!=(graph->edges.end()); it2++){
                if(strcmp(it2->second->to,panel.first_attribute().value())==0){
                    find_to=1;
                    break;
                }
            }

            //mot junction vua loai start vua loai end ?
            if(find_to==1){
                //end edge
                attr = panel.first_attribute();
                End_Junction *j=new End_Junction(attr.value(),attr.next_attribute().value(),attr.next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().next_attribute().value(),it2->first);
                graph->end_junctions.push_back( j);
                if(graph->junctions.find((char*)panel.first_attribute().value())==graph->junctions.end())
                    graph->junctions.insert (  pair<char*,Junction*>((char*)panel.first_attribute().value(),j) );
            }
        }

    }
    //cout<<graph->edges.find("E1")->second;
    for (pugi::xml_node panel = panels.child("connection"); panel; panel = panel.next_sibling("connection")){
        if(strcmp(panel.first_attribute().next_attribute().next_attribute().next_attribute().next_attribute().name(),"via")==0
           && strcmp(panel.first_attribute().next_attribute().next_attribute().next_attribute().value(),"1")==0
           && strcmp(panel.first_attribute().next_attribute().next_attribute().value(),"1")==0){
            char *via=(char*)panel.first_attribute().next_attribute().next_attribute().next_attribute().next_attribute().value();
            int x=0;
            for(int k=0;k<strlen(via);k++){
                if(via[k]=='_')
                    x++;
                if(x==2){
                    via[k]='\0';
                    break;
                }
            }

            //crossing
              //  cout<<"via="<<via<<"shape="<<graph->junctions[via]->shape<<endl;
            if(graph->junctions.find(via)!=graph->junctions.end()){

                Crossing_Junction *j=new Crossing_Junction((const char*)via,(const char*)"via",graph->junctions[via]->shape,panel.first_attribute().value(),panel.first_attribute().next_attribute().value());
                //cout<<(char*)panel.first_attribute().value()<<"\n";


                if(panel.first_attribute().value()[1]!='J')
                    graph->edges[(char*)panel.first_attribute().value()]->junction_crossing.push_front(j);
                graph->crossing__junctions.insert (  pair<char*,Crossing_Junction*>((char*)via,j) );
             //   cout<<panel.first_attribute().value()<<"shape="<<j->shape<<"\n";
            cout<<via<<endl;
            char *junc=via;
            for(int k=1;k<strlen(via);k++)
                if(via[k]=='_'){
                    junc[k-1]='\0';
                    break;
                }
                else
                    junc[k-1]=via[k];
             char*from,*to;
             from=(char*)panel.first_attribute().value();
             to=(char*)panel.first_attribute().next_attribute().value();
             graph->edges[from]->adjJunction.push_back(junc);
             graph->junctions[junc]->adjEdge.push_back(to);
            }
        }
        /*
        char*from,*to;
        from=(char*)panel.first_attribute().value();
        to=(char*)panel.first_attribute().next_attribute().value();
        if( to[1]=='J'){
            if(graph->junctions.find(to)==graph->junctions.end())
                continue;
            if( from [1]=='J'){
                if(graph->junctions.find(from)==graph->junctions.end())
                    continue;
                graph->junctions[from]->adjJunction.push_back(to);
            }
            else{
                if(graph->edges.find(from)==graph->edges.end())
                    continue;
                graph->edges[from]->adjJunction.push_back(to);
            }
        }
        else{
            if(graph->edges.find(to)==graph->edges.end())
                continue;
            if( from [1]=='J'){
                if(graph->junctions.find(from)==graph->junctions.end())
                    continue;
                graph->junctions[from]->adjEdge.push_back(to);
            }
            else{
                if(graph->edges.find(from)==graph->edges.end())
                    continue;
                graph->edges[from]->adjEdge.push_back(to);
            }
        }
        */

    }
    //Tao modon
    //printStartEdges();
    //cout<<graph->end_junctions.size();
    //printEdge("E0");
    for(map<char*,Edge*,cmp_str>::iterator it=graph->edges.begin();it!=graph->edges.end();it++)
        cout<<(it)->first<<"\n";
    double x,L;
    int mo_dun=100;
    char *k=new char[5];;
    char *e_id= new char[50];
    char* name= new char[50];
    char* shape= new char[100];
    while(mo_dun!=0){
    	mo_dun=100;
    	//system("clear");
		cout<<"\n\n\tMENU\n\n";
        cout<<"0.Ket thuc chuong trinh\n";
        cout<<"1.In thong tin canh printEdge(char* nameOfEdge).\n";
        cout<<"2.In thong tin junction loại crossing  printEdge2Juncs(char* nameOfEdge).\n";
        cout<<"3.In ten cac junction loai start printStart().\n";
        cout<<"4.In ra Edge xuat phat tu junction loai start printStartEdges().\n";
        cout<<"5.Chia canh Start ra cac canh bang nhau splitStart(double x, char* name) \n";
        cout<<"6.Toa do ngoac cua xe trajectory(double x, double L, char* shape).\n";
        cout<<"7.Chia doan start thanh cac canh bang nhau splitJunction(double x, char* name) .\n";
        cout<<"8.In junction loai End printEnd().\n";
        cout<<"9.In doan duong co chieu dai x  allParts(double x).\n";
        cout<<"Moi ban chon mo dun:";
    	cin>>mo_dun;
    	switch(mo_dun){
    		case 0:
    			system("clear");
			    break;
    		case 1:
			    system("clear");
			    cout<<"1.Nhap ten Edge.\n";
			    cin>>e_id;
                printEdge(e_id);
                cout<<"Nhan phim bat ky de tiep tuc:";
                fflush(stdin);
                //getch();
                cin>>k;
			    break;
            case 2:
			    system("clear");
			    cout<<"1.Nhap ten Edge.\n";
			    cin>>e_id;
                printEdge2Juncs(e_id);
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 3:
                system("clear");
                printStart();
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 4:
                system("clear");
                printStartEdges();
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 5:
                system("clear");
                cout<<"1.Nhap x va name .\n";
                cin>>x>>name;
                splitStart(x,name);
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 6:
                system("clear");
                cout<<"1.Nhap x va L .\n";
                cin>>x>>L;
                cout<<"2.Nhap shape :\n";
                cin.ignore();
                cin.getline(shape,100);
                trajectory(x, L, shape);
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 7:
                system("clear");
                cout<<"1.Nhap x :\n";
                cin>>x;
                cout<<"2.Nhap ten canh Start  :\n";
                cin.ignore();
                cin.getline(name,50);
                splitJunction(x, name);
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 8:
                system("clear");
                printEnd();
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            case 9:
                system("clear");
                cout<<"1.Nhap x :\n";
                cin>>x;
                allParts(x);
                cout<<"Nhan ky tu bat ky de tiep tuc:";
                fflush(stdin);
                cin>>k;
			    break;
            default:
                break;

        }
    }
	return 0;
}
