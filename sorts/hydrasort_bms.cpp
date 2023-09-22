// god help you, person reading this.
#include <vector>
#include <iostream>

struct Hydra_bms {
    int it; // iterations of Hydra_bms
    int rows;
    std::vector<int> data; // not necessarily of ints but they're the easiest ordered data to work with
    std::vector<std::vector<int>> bms; // suffering
    Hydra_bms(std::vector<int>); // constructor -- takes data and put it in a Hydra.
    int extract_max(); //return the maximal datapoint, remove it, and then reorder so that hte maximal datapoint is in expected position again
    void bms_step();
    void print_debug(bool,bool,bool,bool);
};

void hydrasort_bms(std::vector<int> &list){
    Hydra_bms hydra = Hydra_bms(list); // create hydra
    std::vector<int> revsort; // create list
    while (hydra.data.size()>0){
        revsort.push_back(hydra.extract_max()); // repeatedly push everything into list from largest to smallest
    }
    list.clear(); // make room
    for (int i = revsort.size()-1; i >= 0; i--){
        list.push_back(revsort[i]); // fill from smallest to largest
    }
    return;
}

Hydra_bms::Hydra_bms(std::vector<int> list){
    rows = list.size();
    for (int i=0; i<list.size();i++){ // create columns
        std::vector<int> vec(rows, i);
        bms.push_back(vec);
    }
    it = 1; //initialize
    if (list.size()>1){
        std::vector<int> l2 = list; int last = l2[l2.size()-1]; l2.pop_back();
        Hydra_bms aux = Hydra_bms(l2); // construct smaller hydra
        if (last > aux.data[aux.data.size()-1]){
            aux.data.push_back(last); // put at end
        } else {
            aux.data.push_back(aux.data[aux.data.size()-1]); // dupe
            aux.data[aux.data.size()-2] = last; // put last where this was
        }
        std::vector<int> vec(rows, 0);
        aux.bms.push_back(vec);
        int max = aux.extract_max();
        bool flag = 0; // has max been encountered yet
        for (int i = 0; i<list.size(); i++){
            if (list[i] != max || flag){
                data.push_back(list[i]); // shove everything in except max
            } else {
                flag = 1;
            }
        }
        data.push_back(max); // add max last
    } else {
        data = list; // if the data is size one (or zero) just copy it directly
    }
}

int Hydra_bms::extract_max(){
    while (bms.size()>data.size()){
        bms_step();
    }
    int output = data[data.size()-1];
    data.pop_back();
    if (data.size()>1){
        std::vector<int> d2 = data;
        Hydra_bms aux = Hydra_bms(d2);
        int max = aux.extract_max();
        int maxpos;
        for (int i=0;;i++){ // find first instance of max
            if (data[i]==max){
                maxpos = i;
                break;
            }
        }
        data[maxpos] = data[data.size()-1]; // swap last entry with max
        data[data.size()-1] = max;
    }
    return output;
}
int parent(const std::vector<std::vector<int>>& mat, const int& pospos, const int& column){
    int out = 0;
    // find parents
    if (pospos>0){ // cross things out
        std::vector<bool> ancestors(mat.size(),0);
        int i = column;
        while (i>0){
            i = parent(mat,pospos-1,i);
            ancestors[i]=1; //recursively find ancestors of previous row
        }
        for (int i = 0; i<column;i++){
            if (mat[i][pospos] < mat[column][pospos] && ancestors[i]){
                out = i; // select
            };
        }
    } else { // NO CROSSY!!
        for (int i = 0; i<column;i++){
            if (mat[i][pospos] < mat[column][pospos]){
                out = i;
            };
        }
    }
    return out;
}

void Hydra_bms::bms_step(){
    it++;
    it = 2; //remember to change
    std::vector<int> last = bms[bms.size()-1];
    //bms.pop_back(); // don't pop YET -- easier to work with ancestors this way
    bool all0s = 1;
    int pospos; // last nonzero position in last
    for (int i =0; i<last.size();i++){
        if (last[i]>0) {all0s=0;pospos = i;}
    }
    if (!all0s){ // if all 0s, do nothing
        int br = parent(bms,pospos,bms.size()-1); // find badroot
        //std::cout << br;
        std::vector<int> brv = bms[br]; // and have it as a vec
        std::vector<int> delta(rows,0); int lr; // create delta
        for (int j = 0; j<pospos;j++){
            delta[j]=last[j]-brv[j];
        }
        for (int i = 0;i<delta.size();i++){
            //std::cout << delta[i] << " ";
        }//std::cout << std::endl;
        bms.pop_back(); // and now
        std::vector<std::vector<int>> bp;
        for (int i = br; i<bms.size();i++){ // construct bad part
            bp.push_back(bms[i]);
        }
        std::vector<std::vector<bool>> brancs; //ascension matrix
        {std::vector<bool> vec(rows,1);
            brancs.push_back(vec);}
        for (int i=br+1;i<bms.size();i++){
            std::vector<bool> v;
            for (int j = 0;j<rows;j++){
                int p = i;
                v.push_back(0);
                if (bms[i][j] == 0) break;
                while (p>br){
                    p = parent(bms,j,p);
                    if (p==br){
                        v[j] = 1;
                    }
                }
            }
            brancs.push_back(v);
        }
        for (int t = 0; t<it; t++){ // repeat this many times
            for (int i = 0; i<bp.size();i++){ // increment bad part by delta
                for (int j = 0; j<rows;j++){
                    if (brancs[i][j]){
                        bp[i][j]+=delta[j];
                    }
                }
            }
            for (int i = 0; i< bp.size();i++){
                bms.push_back(bp[i]);
            }
        }
    } else {
        bms.pop_back(); // don't forget to pop last if it's all zeros!
    }
}

void Hydra_bms::print_debug(bool d,bool p,bool i, bool f){
    if (p){
        if (rows>1){
            if (f){
                for (int j = 0; j<bms.size();j++){
                    std::cout << "(" << bms[j][0];
                    for (int i = 1; i<rows;i++){
                        std::cout << "," << bms[j][i];
                    }
                    std::cout << ")";
                }
                
            } else {
                for (int j = 0;j<bms.size();j++ ){
                    std::cout << "/" << bms[j][0] << "\\";
                }
                std::cout << std::endl;
                for (int i = 1; i<rows-1;i++){
                    for (int j = 0;j<bms.size();j++ ){
                        std::cout << "|" << bms[j][i] << "|";
                    }
                    std::cout << std::endl;
                }
                for (int j = 0;j<bms.size();j++ ){
                    std::cout << "\\" << bms[j][rows-1] << "/";
                }
            }
            std::cout << std::endl;
        } else {
            for (int j = 0;j<bms.size();j++ ){
                std::cout << "(" << bms[j][0] << ")";
            }
            std::cout << std::endl;
        }
    }
    if (i){
        std::cout << "iterator: " << it;
    }
    std::cout << std::endl;
}

int main(){
    std::vector<int> data = {3,1,2,4};
    /*
    Hydra_bms test_hydra = Hydra_bms(data);
    test_hydra.bms = {{0,0,0,0},{1,1,1,1},{2,2,2,1},{3,3,2,1},{4,3,2,0}};
    Hydra_bms hydra = Hydra_bms(data);
    for (int i = 0;i<5;i++){
        test_hydra.print_debug(0,1,0,1);
        test_hydra.bms_step();
    }
    */
    hydrasort_bms(data);
    for (int i = 0; i< data.size(); i++){
        std::cout << data[i];
    }
    return 0;
}