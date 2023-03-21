#include<iostream>
#include <fstream>
#include<vector>
#include<string.h>
#include<map>
#include <assert.h>
#include <queue>
#include<math.h>
#include<float.h>
#include <limits>
#include <tuple>
#include <algorithm>
//include "ComparisonTimeSpace.h"
using namespace std;
typedef std::tuple<double, double> Point;
typedef std::tuple<Point, Point, double> Shape;
typedef std::tuple<int, double> Index;
typedef std::tuple<std::string, int, Shape, std::vector<Index>, double,std::vector<int> > TimeSpace;
typedef struct Objective{
	string name;
	double earliness;
	double tardiness;
	double alpha;
	double beta;
	double gamma;
	Objective(string n,double earl,double tard,double a,double b,double g) {
        name = n;
        earl = earliness;
        tard = tardiness;
        alpha= a;
        beta = b;
        gamma =g;
    }
} Objective;

template<typename T>
std::vector<T> create_copy(std::vector<T> const &vec)
{
    std::vector<T> v(vec);
    return v;
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
    while(shape[i]!=',' && shape[i]!=' ' && shape[i]!='\0' && shape[i]!='_'){
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

bool isValid(char *namefile, double x){
    ifstream inFile;
    inFile.open(namefile);
    string line;
    char *line1;
    map < pair<string,int>,bool>Exit;
    while(getline(inFile, line)){
        line1=(char*)line.c_str();
        int i=0;
        //cout<<line<<"\n";
        char *nameEJ= new char[10];
        for(i=0;i<strlen(line1);i++)
            if(line1[i]==' '){
                nameEJ[i]='\0';
                i++;
                break;
            }
            else
                nameEJ[i]=line1[i];
        if(i>=strlen(line1))
            return false;
        int id=(int)splitNumber(line1,i);
        if(i>=strlen(line1))
            return false;
        double x1,x2,y1,y2;
        x1=splitNumber(line1,i);
        if(i>=strlen(line1))
            return false;
        y1=splitNumber(line1,i);
        if(i>=strlen(line1))
            return false;
        x2=splitNumber(line1,i);
        if(i>=strlen(line1))
            return false;
        y2=splitNumber(line1,i);
        if(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))-x>0.0001){
            cout<<sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))-x<<"\n";
            cout<<x1<<" "<<y1<<"  "<<x2<<" "<<y2<<"\n";
            return false;
        }
        //cout<<pair<char*,int> (nameEJ,id).first;
        //cout<<nameEJ<<" "<<id<<"\n";
        if(Exit.find(pair<string,int>(nameEJ,id))==Exit.end()){
            Exit.insert(pair<pair<string,int>,bool> ( pair<string,int> (nameEJ,id),true));
            //std::cout << "mymap.size() is " << Exit.size() << '\n';
        }
        else{
            cout<<line<<"\n";
            return false;
        }

    }
    return true;
}
bool checkValid(std::vector<TimeSpace> *all, double x, double epsilon) {
    vector<TimeSpace>::iterator it;
    double L;
    for ( it = all->begin() ; it != all->end(); it++){
        L=get<2>(get<2>(*it));
        if(fabs(L-x)/x>epsilon)
            return false;
    }

    return true;
}
void assignNeighbors(std::vector<TimeSpace> *all, int index) {
    vector<TimeSpace>::iterator it;
    int index_neighbor=0;
    for (it = all->begin() ; it != all->end(); it++,index_neighbor++){
        if(get<1>(get<2>(all->at(index)))==get<0>(get<2>(*it))){
            (get<3>(all->at(index))).push_back(tuple<int, double> (index_neighbor,0.0));
        }
    }
}
//2e
bool checkValidation(vector<vector<TimeSpace> > *graph, double V, double H, double dt){
    int N=ceil(H/dt);
    long numNodes=0;
    long numEdges=0;

    cout<<graph->size()<<"\n";
    for(vector<vector<TimeSpace> > :: iterator it=graph->begin();it!=graph->end();it++)
        numNodes+=(*it).size();
    cout<<"numnode="<<numNodes<<"\n";
    cout<<N+1;
    assert(numNodes%(N+1)==0);

    for(vector<vector<TimeSpace> > :: iterator it=graph->begin();it!=graph->end();it++)
        for(vector<TimeSpace>:: iterator it1=(*it).begin();it1!=(*it).end();it1++)
            numEdges+=get<3>(*it1).size();
    assert(numEdges>numNodes);

    typedef tuple<string, int, Shape, string, int, Shape> pairLayered ;
    map<pairLayered,int> M;
    pairLayered p;

    for(vector<vector<TimeSpace> > ::iterator it= graph->begin();it!=graph->end();it++)
        for(vector<TimeSpace>:: iterator it1=(*it).begin();it1!=(*it).end();it1++){
            vector<Index> edge_vector = get<3>(*it1);
            for(vector<Index>::iterator it2=edge_vector.begin();it2!=edge_vector.end();it2++)
                // j!=i && d2>d1
                if(get<0>(*it2)!=get<1>(*it1) && get<1>(*it2) >get<4>(*it1)){
                    //get  s2, j, S2
                    for(vector<vector<TimeSpace> > ::iterator it3= graph->begin();it3!=graph->end();it3++){
                        //d2==d
                        vector<TimeSpace>:: iterator it4=(*it3).begin();
                        if(get<4>(*it4)!=get<1>(*it2))
                            continue;
                        else{
                            int j=get<0>(*it2);
                            p=make_tuple(get<0>(*it1),get<1>(*it1),get<2>(*it1),get<0>((*it3)[j]),get<1>((*it3)[j]),get<2>((*it3)[j]));
                        }
                    }
                    if(M.find(p)!=M.end())
                        M[p]++;
                    else
                        M[p]=1;
                }
        }
    for(map<pairLayered,int> :: iterator it=M.begin();it!=M.end();it++){
        double L=get<2>(get<2>(it->first));
        //M*L/v<=H
        assert((it->second)*L/V<=2*H);
    }
    return true;
}
//3a
vector<vector<TimeSpace> >* updateEdge(string name, double length, double H, double dt, vector<vector<TimeSpace> > *graph){
    int N=ceil(H/dt);
    for(int i=0;i<=N;i++){
        vector<TimeSpace> temp = graph->at(i);
        for(int j=0;j<temp.size();j++){
            if(get<0>(temp.at(j))==name){
                vector<Index> indices = get<3>(temp.at(j));
                int S = indices.size() - 1;
                vector<Index> Neighbors;
                copy(indices.begin(), indices.end(), back_inserter(Neighbors));
                Neighbors.erase (Neighbors.begin()+j);
                vector<int> SubNeighbors{};
                vector<int> Remove{};

                for(int k=0;k<=S;k++){
                    int e=get<0>(indices.at(k));
                    if(k!=j){
                        if(get<1>(indices.at(k))>i*dt){
                            if(i+length<=N){
                                get<1>(indices.at(k))=(i+length)*dt;
                                SubNeighbors.push_back(e);
                            }
                            else{
                                Remove.push_back(e);
                            }
                        }
                    }
                    for(vector<Index>::iterator it=Neighbors.begin();it!=Neighbors.end();it++){
                        int find_e=0;
                        for(vector<int>:: iterator it1=SubNeighbors.begin();it1!=SubNeighbors.end();it1++)
                            if(*it1==get<0>(*it)){
                                find_e=1;
                                break;
                            }
                        if(find_e==0 && i+length<=N){
                            indices.push_back(make_tuple(e, (i+length)*dt));
                        }
                    }
                    int index_remove=0;
                    for(vector<int>:: iterator it=Remove.begin();it!=Remove.end();it++){
                        indices.erase(indices.begin()+(*it)-index_remove);
                        index_remove++;
                    }

                }
                get<3>(temp.at(j))=indices;
            }
        }
    }
    return graph;
}
/*/3b
int find(std::string name, vector<TimeSpace> v){
    int index=0;
    for(vector<TimeSpace>:: iterator it=v.begin();it!=v.end();it++,index++)
        if(get<0>(*it)==name)
            return index;
    return -1;
}
vector<pair<int, int> > getPossibleDestinations(Objective *obj, std::vector<std::vector<TimeSpace>> *graph, double dt, double H, double initTime){
     std::vector<std::pair<int, int> > result;
     priority_queue <ComparisonTimeSpace*,  vector<ComparisonTimeSpace*>, cmp> TSqueue;
     int index = find(obj->name, graph->at(0));
     int Init =  ceil(initTime/dt);
     int N=ceil(H/dt);
     for(int i=0;i<=N;i++){
          TSqueue.push(new ComparisonTimeSpace(i, index,i*dt-initTime,obj,dt));
     }
     while(!TSqueue.empty()){
        ComparisonTimeSpace *c=TSqueue.top();
        TSqueue.pop();
        result.push_back(make_pair(c->getFirstIndex(),c->getSecondIndex()));
     }


    return result;
}
*/
//4a
double weightEdge(std::vector<std::vector<TimeSpace> > graph, std::pair<int, int> a, std::pair<int, int> b, double dt, double start, Objective* obj){
    double alpha = obj->alpha;
    double beta = obj->beta;
    double gamma = obj->gamma;
    double w = 0;
    if(a.second!= b.second){
          w += alpha*max(0.0, obj->earliness - b.first*dt);
          w += gamma*max(0.0, b.first*dt - obj->tardiness);
    }
    else{
          w += beta*(a.first*dt - start);
          w += gamma*max(0.0, b.first*dt - obj->tardiness);
    }
    return w;
}
//4c
vector<pair<string, double> > toStrdouble( std::vector<pair<int, int> > path, std::vector<std::vector<TimeSpace> > graph, double dt){
    std::vector<pair<string, double> > result;
    for(int i=0;i<path.size();i++){
        int index0 = get<0>(path.at(i));
        double t = index0*dt;
        int index1= get<1>(path.at(i));
        string name = get<0>(graph.at(index0).at(index1));
        result.push_back(make_pair(name,t));
    }
    return result;
}
pair<int,double> TopKey(map<int,double> U){
    pair <int,double> minU = *min_element(U.begin(),U.end());
    return minU;
}
void UpdateVertex(int s,int stt_start, int stt_goal,vector<double> &g,vector<double> &rhs,map<int,double> &U,double start,int tang, Objective* target,double dt,std::vector<std::vector<TimeSpace> > graph){
    vector<TimeSpace> vector_TS=graph[tang];
    if(s!=stt_goal){
        rhs[s]=DBL_MAX;
        for(vector<Index>::iterator it=(get<3>(vector_TS[s])).begin();it!=(get<3>(vector_TS[s])).end();it++){
            if(fabs(get<1>(*it)-dt>0.001))
                continue;
            Point p1,p2;
            p1=get<0>(get<2>(vector_TS[s]));
            p2=get<0>(get<2>(vector_TS[get<0>(*it)]));
            rhs[s]=min(rhs[s],g[get<0>(*it)]+hypot(get<1>(p1)-get<1>(p2),get<0>(p1)-get<0>(p2)));
        }
    }
    if(U.find(s)!=U.end())
        U.erase(s);
    if(g[s]!=rhs[s])
        U[s]=hypot(min(g[stt_start],rhs[stt_start])+weightEdge(graph,make_pair(tang,stt_start),make_pair(tang,s),dt,start,target),min(g[stt_start],rhs[stt_start]));

}
vector<pair<int, int> > extract_path( int stt_start,int stt_goal,int tang,vector<double> g,vector<TimeSpace> vector_TS,double dt){
    vector<pair<int, int> > path= vector<pair<int, int> > ();
    path.push_back(make_pair(tang,stt_start));
    int s=stt_start;
    while(true){
        map<int,double> g_list=map<int,double>();
        for(vector<Index>::iterator it=(get<3>(vector_TS[s])).begin();it!=(get<3>(vector_TS[s])).end();it++){
                if(fabs(get<1>(*it)-dt>0.001))
                    continue;
                g_list[get<0>(*it)] = g[get<0>(*it)];
        }
        pair <int,double> min_g_list = *min_element(g_list.begin(),g_list.end());
        s=get<0>(min_g_list);
        path.push_back(make_pair(tang,s));
        if(s==stt_goal)
            break;
    }
    return path;
}
//4b
vector<pair<string, double> > findPath(string name, double start, Objective *target, double dt, vector<vector<TimeSpace> > graph){
    string s_start_name=name;
    int s_start_id=0;
    string s_goal_name=target->name;
    int s_goal_id=0;
    vector<TimeSpace> vector_TS;
    int tang=0;
    for(vector<vector<TimeSpace> >::iterator it=graph.begin();it!=graph.end();it++,tang++)
        if(fabs(get<4>((*it)[0])-dt)<0.0001){
            vector_TS=*it;
            break;
        }
    int stt_start=0;
    int stt_goal=0;
    for(vector<TimeSpace>::iterator it=vector_TS.begin();it!=vector_TS.end();it++,stt_start++)
        if(get<0>(*it)==s_start_name)
            break;
    for(vector<TimeSpace>::iterator it=vector_TS.begin();it!=vector_TS.end();it++,stt_goal++)
        if(get<0>(*it)==s_goal_name)
            break;
    vector<double> g(vector_TS.size(), numeric_limits<double>::infinity());
    vector<double> rhs(vector_TS.size(), numeric_limits<double>::infinity());
    map<int,double> U=map<int,double> ();
    rhs[stt_goal]=0.0;
    U[stt_start]=weightEdge(graph,make_pair(tang,stt_start),make_pair(tang,stt_goal),dt,start,target);
    //ComputePath
    while(true){
        //cout<<"run\n" <<U.size();

        pair<int,double> minU=TopKey(U);
        //cout<<minU.first<<" end";
        double CalculateKey_s_start=hypot(min(g[stt_start],rhs[stt_start])+weightEdge(graph,make_pair(tang,stt_start),make_pair(tang,stt_start),dt,start,target),min(g[stt_start],rhs[stt_start]));
        if(minU.second>=CalculateKey_s_start && fabs(rhs[stt_start]-g[stt_start])<=0.0001)
            break;
        double k_old=minU.second;
        U.erase(minU.first);
        double CalculateKey_s=hypot(min(g[stt_start],rhs[stt_start])+weightEdge(graph,make_pair(tang,stt_start),make_pair(tang,minU.second),dt,start,target),min(g[stt_start],rhs[stt_start]));
        int s=minU.first;
        if(k_old<CalculateKey_s)
            U[minU.first]=CalculateKey_s;
        else if(g[minU.first]>rhs[minU.first]){
            g[minU.first]=rhs[minU.first];
            //for
            for(vector<Index>::iterator it=(get<3>(vector_TS[s])).begin();it!=(get<3>(vector_TS[s])).end();it++){
                if(fabs(get<1>(*it)-dt)>0.001)
                    continue;
                UpdateVertex(get<0>(*it),stt_start,stt_goal,g,rhs,U,start,tang,target,dt,graph);
            }
        }
        else{
            g[s]=DBL_MAX;
            UpdateVertex(s,stt_start,stt_goal,g,rhs,U,start,tang,target,dt,graph);
            for(vector<Index>::iterator it=(get<3>(vector_TS[s])).begin();it!=(get<3>(vector_TS[s])).end();it++){
                if(fabs(get<1>(*it)-dt)>0.001)
                    continue;
                UpdateVertex(get<0>(*it),stt_start,stt_goal,g,rhs,U,start,tang,target,dt,graph);
            }
        }

    }
    cout<<"done for";
	vector<pair<int, int> > path=extract_path(stt_start,stt_goal,tang,g,vector_TS,dt);

	return toStrdouble(path,graph,dt);
}
//6 a
std::vector<std::vector<TimeSpace> > conflict_init(std::vector<std::vector<TimeSpace> > graph){
    for(vector<vector<TimeSpace> >:: iterator it=graph.begin();it!=graph.end();it++){
        std::vector<TimeSpace> temp = *it;
        vector<Index> Neighbors;
        for(int j=0;j<temp.size();j++){
            vector<Index> indices = get<3>(temp.at(j));
            copy(indices.begin(), indices.end(), back_inserter(Neighbors));
            Neighbors.erase (Neighbors.begin()+j);
            for(vector<Index>:: iterator it2=Neighbors.begin();it2!=Neighbors.end();it2++)
                get<5>(temp[get<0>(*it2)]).push_back(j);
        }
    }
    return graph;
}
//6b
bool isOverlapped(std::pair<int, int> *nA, std::pair<int, int> *nB,  std::vector<std::vector<TimeSpace> > graph, double x){
    int i=get<1>(*nA);
    int j=get<1>(*nB);
    int tStepA=get<0>(*nA);
    int tStepB=get<0>(*nB);
    if(tStepA == tStepB || tStepA == tStepB +1 || tStepA == tStepB-1){
        if(i==j)
            return true;
        vector<int> conflict = get<5>(graph[tStepA][i]);
        for(vector<int>::iterator it= conflict.begin();it!=conflict.end();it++)
            if(j==*it)
                return true;
        vector<Index> indices= get<3>(graph[tStepA][i]);
        for(vector<Index>::iterator it= indices.begin();it!=indices.end();it++){
            if(j==get<0>(*it))
                return true;
             conflict = get<5>(graph[tStepA][get<0>(*it)]);
             for(vector<int>::iterator it= conflict.begin();it!=conflict.end();it++)
                if(j==*it)
                    return true;
        }
        indices=get<3>(graph[tStepB][j]);
        for(vector<Index>::iterator it= indices.begin();it!=indices.end();it++){
            if(j==get<0>(*it))
                return true;
             conflict = get<5>(graph[tStepA][get<0>(*it)]);
             for(vector<int>::iterator it= conflict.begin();it!=conflict.end();it++)
                if(j==*it)
                    return true;
        }
    }
    return false;
}

int main(){
    cout<<"Day la chuong trinh thuc hien bai 2 tai link:https://docs.google.com/document/d/1snBJqZHk2xKfge8OAGWh05AgDuYDsExLnakwHrhpXcw/edit\n";
    char *namefile="AllParts.txt";
    vector<TimeSpace> *init = new vector<TimeSpace>;
    ifstream inFile;
    inFile.open(namefile);
    string line;
    char *line1;
    while(getline(inFile, line)){
        line1=(char*)line.c_str();
        int i=0;
        char *nameEJ= new char[10];
        for(i=0;i<strlen(line1);i++)
            if(line1[i]==' '){
                nameEJ[i]='\0';
                i++;
                break;
            }
            else
                nameEJ[i]=line1[i];
        int id=(int)splitNumber(line1,i);
        double x1,x2,y1,y2;
        x1=splitNumber(line1,i);
        y1=splitNumber(line1,i);
        x2=splitNumber(line1,i);
        y2=splitNumber(line1,i);
        Point p1=tuple<double, double> (x1,y1);
        Point p2=tuple<double, double> (x2,y2);
        double L=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        Shape s=tuple<Point, Point, double>(p1,p2,L);
        std::vector<Index> myvector1{};
        std::vector<int> myvector2{};
        TimeSpace times=tuple<string, int, Shape, vector<Index>, double,vector<int> > (nameEJ,id,s,myvector1,0.0,myvector2);
        init->push_back(times);
    }

    //cout<<checkValid(init,1.41,0.001);
    //cau d
    //giá trị thực H và giá trị thực dt, một vector std::vector<TimeSpace> init, vận tốc V
    vector<TimeSpace>::iterator it;
    int index=0;

    for (it = init->begin() ; it != init->end(); it++,index++)
        assignNeighbors(init,index);
    double H=3,dt=1;
    double V=1;
    int N=ceil(H/dt);
    std::vector<std::vector<TimeSpace> > graph;
    for(int i=0;i<=N;i++){
        std::vector<TimeSpace> temp;
        copy(init->begin(), init->end(), back_inserter(temp));
        //cout<<temp.size()<<"\n";
        for(int j=0;j<temp.size();j++){
            vector<Index> indices = get<3>(temp.at(j));
            int S=indices.size()-1;
            for(int k=0;k<=S;k++){
                indices.at(k) = make_tuple(get<0>(indices.at(k)), i*dt);
            }
            double x=get<2>(get<2>(temp.at(j)));
            int min1=ceil(x/V);
             for(int k=0;k<=S;k++){
                 if(i+min1<=N)
                    indices.push_back(make_tuple(get<0>(indices.at(k)),(i+min1)*dt));
             }
             if((i + 1) <= N)
                indices.push_back(make_tuple(j, (i+1)*dt));
             get<4> (temp.at(j))= i*dt;
             get<3>(temp.at(j))=indices;
        }
        //temp.update(indices);
        //get<3>(temp.at(j))=indices;
        graph.push_back(temp);

    }
    //cout<<"Hop le:"<<isValid("AllParts.txt",1.41);
    cout<<checkValidation(&graph,1,3,1.0);
    for(vector<Index>::iterator it=(get<3>(graph[0][46])).begin();it!=(get<3>(graph[0][46])).end();it++)
        cout<<get<0>(*it)<<" "<<get<1>(*it)<<"\n";
    cout<<"done\n";
    vector<pair<string, double> > path;
    Objective *o=new Objective("E298",23,100,2,3,1);
    path=findPath("E226",1.0,o,1.0,graph);
    for(vector<pair<string, double> >::iterator it=path.begin();it!=path.end();it++)
        cout<<(*it).first<<" "<<(*it).second<<"\n";
    cout<<"done";
}
