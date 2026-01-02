#ifndef BMSSP_HPP
#define BMSSP_HPP

#include <vector>
#include <list>
#include <set>
#include <queue>
#include <tuple>
#include <cmath>
#include <limits>
#include <algorithm>
#include "metrics.hpp"

namespace spp {

/* ============================================================
 * Unique distance (Assumption 2.1)
 * ============================================================ */
template<typename wT>
struct uniqueDistT : std::tuple<wT,int,int,int> {
    uniqueDistT() = default;
    uniqueDistT(wT d,int h,int v,int p)
        : std::tuple<wT,int,int,int>(d,h,v,p) {}
};

/* ============================================================
 * Worst-case hash map (theoretical O(1))
 * ============================================================ */
template<typename T>
struct wc_hash {
    std::vector<int> flag;
    std::vector<T> val;
    int tick = 1;

    wc_hash(int n): flag(n,0), val(n) {}

    void clear() { tick++; }

    bool contains(int i) const {
        return flag[i]==tick;
    }

    T& operator[](int i){
        flag[i]=tick;
        return val[i];
    }
};

/* ============================================================
 * Batch Priority Queue (Lemma 3.3)
 * ============================================================ */
 template<typename D, typename It>
struct UBCompare {
    bool operator()(const std::pair<D,It>& a,
                    const std::pair<D,It>& b) const {
        return std::get<0>(a.first) < std::get<0>(b.first);
    }
};

template<typename D>
class batchPQ {
    using elem = std::pair<int,D>;

    std::list<std::list<elem>> buckets;

    using bucket_it = typename std::list<std::list<elem>>::iterator;
    std::set<std::pair<D,bucket_it>, UBCompare<D,bucket_it>> ub;

    wc_hash<D> best;

    int M;
    int size = 0;

public:
    batchPQ(int n): best(n) {}

    void init(int M_, D B){
        M=M_;
        buckets.clear();
        buckets.push_back({});
        ub.clear();
        ub.insert({B,buckets.begin()});
        best.clear();
        size=0;
    }

    bool empty() const { return size==0; }

    void insert(int u, D d){
        if(best.contains(u) && best[u]<=d) return;
        best[u]=d;
        auto it = ub.lower_bound({d,buckets.begin()})->second;
        it->push_back({u,d});
        size++;
    }

    std::pair<D,std::vector<int>> pull(){
        std::vector<elem> all;
        for(auto& b:buckets)
            for(auto&e:b) all.push_back(e);

        if((int)all.size()<=M){
            std::vector<int> S;
            for(auto&[u,_]:all) S.push_back(u);
            size=0;
            return {ub.begin()->first,S};
        }

        std::nth_element(
            all.begin(), all.begin()+M, all.end(),
            [](auto&a,auto&b){return a.second<b.second;}
        );

        D B = all[M].second;
        std::vector<int> S;
        for(auto&[u,d]:all)
            if(d<B) S.push_back(u);

        size -= S.size();
        return {B,S};
    }
};

/* ============================================================
 * BMSSP main class
 * ============================================================ */
template<typename wT>
class bmssp {
    using D = uniqueDistT<wT>;

    int n;
    std::vector<std::vector<std::pair<int,wT>>> adj;
    std::vector<wT> dist;
    std::vector<int> parent, depth;

    int k,t,L;
    std::vector<batchPQ<D>> PQs;
    Metrics* metrics;

public:
    bmssp(int n_, Metrics* m=nullptr)
        : n(n_), adj(n_), dist(n_), parent(n_), depth(n_),
          metrics(m)
    {
        double lg = std::log2(std::max(2,n));
        k = std::max(1,(int)std::pow(lg,1.0/3));
        t = std::max(1,(int)std::pow(lg,2.0/3));
        L = std::ceil(lg/t);
        PQs.assign(L,batchPQ<D>(n));
    }

    void addEdge(int u,int v,wT w){
        adj[u].push_back({v,w});
        if(metrics) metrics->count++;
    }

    void prepare_graph(bool){
        // No-op (solo para compatibilidad)
    }

    void execute(int s){
        for(int i=0;i<n;i++){
            dist[i]=inf();
            parent[i]=i;
            depth[i]=0;
        }
        dist[s]=0;
        bmsspRec(L,D(inf(),0,0,0),{s});
    }

private:
    wT inf() const {
        return std::numeric_limits<wT>::max()/4;
    }

    D getD(int u){
        if(metrics) metrics->count++;
        return D(dist[u],depth[u],u,parent[u]);
    }

    D relaxD(int u,int v,wT w){
        if(metrics) metrics->count++;
        return D(dist[u]+w,depth[u]+1,v,u);
    }

    void relax(int u,int v,wT w){
        dist[v]=dist[u]+w;
        depth[v]=depth[u]+1;
        parent[v]=u;
        if(metrics) metrics->count+=3;
    }

    /* ================= Base Case ================= */
    std::pair<D,std::vector<int>>
    baseCase(D B,int s){
        std::priority_queue<D,std::vector<D>,std::greater<D>> pq;
        pq.push(getD(s));
        std::vector<int> comp;

        while(!pq.empty() && (int)comp.size()<=k){
            auto d=pq.top(); pq.pop();
            int u=std::get<2>(d);
            if(d>getD(u)) continue;
            comp.push_back(u);

            for(auto&[v,w]:adj[u]){
                auto nd=relaxD(u,v,w);
                if(nd<=getD(v) && nd<B){
                    relax(u,v,w);
                    pq.push(nd);
                }
            }
        }

        if((int)comp.size()<=k)
            return {B,comp};

        D newB = getD(comp.back());
        comp.pop_back();
        return {newB,comp};
    }

    /* ================= Recursive BMSSP ================= */
    std::pair<D,std::vector<int>>
    bmsspRec(int l,D B,const std::vector<int>& S){
        if(metrics) metrics->count++;

        if(l==0)
            return baseCase(B,S[0]);

        auto& Q = PQs[l-1];
        Q.init(1<<(l*t),B);

        for(int x:S)
            Q.insert(x,getD(x));

        std::vector<int> comp;
        while(!Q.empty()){
            auto [tryB,miniS]=Q.pull();
            auto [newB,add]=bmsspRec(l-1,tryB,miniS);
            B=newB;
            comp.insert(comp.end(),add.begin(),add.end());
        }
        return {B,comp};
    }
};

} // namespace spp
#endif
