#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"
#include "SegTree.h"
#include "MyBitset.h"

/**
 * used for heuristic & preprocess
 */
class Graph
{
    bool reduced; // if so, the indices of vertices is mapped so we need to use map_refresh_id

public:
    ui n, m;
    ui *d; // degree
    // edge_to[ pstart[a] ... pstart[a+1] ] is the neighbor of a
    ui *edge_to;                          // size = m
    ui *pstart;                           // size = n+1
    unordered_map<ui, ui> map_refresh_id; // we need to re-map the reduced graph to {0,1,...,n-1}, thus requiring to record the map
    Graph() : n(0), m(0), d(nullptr), edge_to(nullptr), pstart(nullptr), reduced(false)
    {
    }
    ~Graph()
    {
        if (d != nullptr)
            delete[] d;
        map_refresh_id.clear();
        if (edge_to != nullptr)
            delete[] edge_to;
        if (pstart != nullptr)
            delete[] pstart;
    }
    /**
     * @return whether (a, b) ∈ E
     */
    bool exist_edge(ui a, ui b)
    {
        return has(edge_to + pstart[a], edge_to + pstart[a + 1], b);
    }
    /**
     * @brief read edges from file where the file format can be ".txt" ".mtx" ".bin" ".out" or no suffix name
     *
     * as for ".mtx" or no suffix name, we need to re-map the vertex index to [0, n-1]
     * as for  ".txt" ".bin" ".out", just read
     */
    void readFromFile(string file_path)
    {
        ifstream in(file_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", file_path.c_str());
            exit(1);
        }
        string suffix = get_file_name_suffix(file_path);
        if (suffix == "mtx")
        {
            string line;
            do
            {
                getline(in, line);
            } while (line[0] == '%');
            // the first line should be n n m
            {
                stringstream ss(line);
                ss >> n >> n >> m;
            }
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            vector<int> v_map(n + 1, -1);
            int id_v = 0;
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> a >> b;
                if (a == b)
                    continue;
                if (v_map[a] == -1)
                    v_map[a] = id_v++;
                if (v_map[b] == -1)
                    v_map[b] = id_v++;
                a = v_map[a], b = v_map[b];
                assert(a != b);
                assert(a < n && b < n);
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            for (ui i = 0; i + 1 < m; i++)
                assert(edges[i] < edges[i + 1]);
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    assert(h.x > last_v);
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }
        else if (suffix.size() == 0)
        {
            char ch;
            string s;
            in >> ch >> s >> n >> m;
            assert(ch == 'p');
            assert(s == "edge");
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            vector<int> v_map(n + 1, -1);
            int id_v = 0;
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> ch >> a >> b;
                assert(ch == 'e');
                if (a == b)
                    continue;
                if (v_map[a] == -1)
                    v_map[a] = id_v++;
                if (v_map[b] == -1)
                    v_map[b] = id_v++;
                a = v_map[a], b = v_map[b];
                assert(a != b);
                assert(a < n && b < n);
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            for (ui i = 0; i + 1 < m; i++)
                assert(edges[i] < edges[i + 1]);
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    assert(h.x > last_v);
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }
        else if (suffix == "bin")
        {
            FILE *in = fopen(file_path.c_str(), "rb");
            if (in == nullptr)
            {
                printf("Failed to open %s \n", file_path.c_str());
                exit(1);
            }
            fread(&n, sizeof(ui), 1, in);
            fread(&m, sizeof(ui), 1, in);
            d = new ui[n];
            pstart = new ui[n + 1];
            edge_to = new ui[m];
            fread(d, sizeof(ui), n, in);
            fread(edge_to, sizeof(ui), m, in);
            pstart[0] = 0;
            for (ui i = 1; i < n; i++)
                pstart[i] = pstart[i - 1] + d[i - 1];
        }
        else // default graph file format: n m \n edges
        {
            in >> n >> m;
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> a >> b;
                assert(a != b);
                assert(a < n && b < n);
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            for (ui i = 0; i + 1 < m; i++)
                assert(edges[i] < edges[i + 1]);
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    assert(h.x > last_v);
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }

        printf("Graph init ok\n");
        fflush(stdout);
    }
    /**
     * @brief StrongHeuris, extend u to a maximal plex
     * @return lb
     */
    int extend(ui u, set<ui> *solution = nullptr, set<ui> *must_contain = nullptr)
    {
        SegTree tr(n, d, edge_to + pstart[u], d[u]);
        set<ui> s{u};
        tr.remove(u);
        vector<ui> d(n, 0);
        vector<bool> rm(n, 0);
        rm[u] = 1;
        for (ui I = 0; I + 1 < n; I++) // enumerate the rest n-1 vertices
        {
            ui v = tr.get();
            d[v] = tr.get_cnt();
            tr.remove(v);
            rm[v] = 1;
            if (d[v] + paramK < s.size() + 1)
                break;
            ui *l = edge_to + pstart[v], *r = edge_to + pstart[v + 1];
            // check whether v can be inserted into s
            bool ok = 1;
            if (s.size() >= paramK)
            {
                for (ui w : s)
                {
                    if (!has(l, r, w) && d[w] + paramK == s.size())
                    {
                        ok = 0;
                        break;
                    }
                }
            }
            if (ok)
            {
                for (ui w : s)
                {
                    if (has(l, r, w))
                        d[w]++;
                }
            }
            else
                continue;
            s.insert(v);
            for (ui i = pstart[v]; i < pstart[v + 1]; i++)
            {
                ui w = edge_to[i];
                if (rm[w])
                    continue;
                tr.add(w);
            }
        }
        // update current solution
        if (solution != nullptr)
        {
            if (must_contain == nullptr && solution->size() < s.size())
            {
                solution->clear();
                for (ui v : s)
                    solution->insert(map_refresh_id[v]);
            }
            else if (must_contain != nullptr && must_contain->size() + s.size() > solution->size())
            {
                solution->clear();
                for (ui v : s)
                    solution->insert(map_refresh_id[v]);
                for (ui v : (*must_contain))
                    solution->insert(v);
            }
        }
        return s.size();
    }
    /**
     * useless, just a demo for usage
     * @return whether s is a k-plex
     */
    bool is_plex(set<ui> &s)
    {
        map<ui, ui> d;
        for (ui u : s)
        {
            d[u] = 0;
            for (ui v : s)
                if (exist_edge(u, v))
                    d[u]++;
            if (d[u] + paramK < s.size())
                return false;
        }
        return true;
    }
    /**
     * @brief select vertices that must occur in maximum k-plex, and store them in s
     *
     * @param rm rm[u]=1 <==> u in s
     */
    void get_v_must_include(set<ui> &s, vector<bool> &rm)
    {
        set<ui> satisfied;
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK >= n)
                satisfied.insert(i);
        for (ui u : satisfied)
        {
            if (d[u] + 1 == n)
            {
                s.insert(map_refresh_id[u]);
                rm[u] = 1;
                continue;
            }
            ui cnt = 0; // the number of satisfied vertices that are non-neighbor of u
            for (ui v : satisfied)
            {
                if (v != u && !exist_edge(u, v))
                    cnt++;
            }
            if (d[u] + cnt + 1 == n)
            {
                // all non-neighbors of u are in the set 'satisfied' => u must be included
                s.insert(map_refresh_id[u]);
                rm[u] = 1;
            }
        }
    }
    /**
     * @brief remove the vertices that must include, and update the degree of rest vertices
     *
     * @param rm mask of vertices that need to remove
     */
    void remove_v_must_include(vector<bool> &rm, int lb)
    {
        ui *q = new ui[n + 1];
        ui hh = 1, tt = 0;
        for (ui i = 0; i < n; i++)
            if (rm[i])
                q[++tt] = i;
        if (tt == 0)
        {
            delete[] q;
            return;
        }
        while (hh <= tt)
        {
            ui u = q[hh++];
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (rm[v])
                    continue;
                if (--d[v] + paramK <= lb) // actually, none of the rest vertices can be removed by weak reduce
                    q[++tt] = v, rm[v] = 1;
            }
        }
        ui new_n = 0;
        if (reduced)
        {
            unordered_map<ui, ui> new_map;
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    new_map[new_n] = map_refresh_id[i];
                    q[i] = new_n++;
                }
            map_refresh_id = new_map;
        }
        else
        {
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    map_refresh_id[new_n] = i;
                    q[i] = new_n++;
                }
            reduced = true;
        }
        ui *new_pstart = new ui[new_n + 1];
        ui *new_d = new ui[new_n];
        ui j = 0;
        for (ui i = 0; i < n; i++)
        {
            if (rm[i])
                continue;
            ui u = q[i];
            new_pstart[u] = j;
            for (ui p = pstart[i]; p < pstart[i + 1]; p++)
            {
                ui v = edge_to[p];
                if (!rm[v])
                    edge_to[j++] = q[edge_to[p]];
            }
            new_d[u] = j - new_pstart[u];
        }
        new_pstart[new_n] = j;
        delete[] d;
        delete[] pstart;
        d = new_d;
        pstart = new_pstart;
        if (j * 2 < m)
        {
            ui *new_edge_to = new ui[j];
            memcpy(new_edge_to, edge_to, sizeof(ui) * j);
            delete[] edge_to;
            edge_to = new_edge_to;
        }
        m = j;
        n = new_n;
        delete[] q;
    }
    /**
     * @brief stage-III: acquire a heuristic solution in the sqrt graph
     *
     * @param range the number of vertices in the subgraph where each vertex's id is 0~range-1
     * @param neighbor neighbor[u] is the array of u's neighbors
     * @param d d[u] = neighbor[u].size()
     * @param id id[u] is the true index of u in the input graph, i.e., id[u]∈[0, n-1]
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int sqrt_degeneracy(ui range, vector<vector<ui>> &neighbor, vector<ui> &d, vector<ui> &id, set<ui> *solution = nullptr)
    {
        LinearHeap heap(range, range, d);
        vector<bool> rm(range, 0);
        while (heap.get_min_key() + paramK < heap.sz)
        {
            ui sel = heap.get_min_node();
            heap.delete_node(sel);
            rm[sel] = 1;
            for (ui v : neighbor[sel])
            {
                if (rm[v])
                    continue;
                heap.decrease(--d[v], v);
            }
        }
        int rest = heap.sz;
        if (solution != nullptr && solution->size() < rest)
        {
            solution->clear();
            if (reduced)
            {
                for (ui i = 0; i < range; i++)
                    if (!rm[i])
                        solution->insert(map_refresh_id[id[i]]);
            }
            else
            {
                for (ui i = 0; i < range; i++)
                    if (!rm[i])
                        solution->insert(id[i]);
            }
        }
        return rest;
    }
    /**
     * @brief stage-II: induce a subgraph with sqrt(m) vertices
     *
     * @param start_u the start vertex when we bfs
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int sqrt_degeneracy(ui start_u, set<ui> *solution = nullptr)
    {
        ui range = sqrt(n);
        vector<ui> id(range, 0);
        vector<ui> vis(n, 0); // vis[u]!=0 <==> u in subgraph and id[vis[u]-1]=u
        ui cnt = 0;
        vis[start_u] = 1 + cnt;
        id[cnt++] = start_u;
        // bfs
        queue<ui> q;
        q.push(start_u);
        // {
        //     int max_neighbor = range>>4;
        //     auto nei=neighbor(start_u);
        //     sort(nei.begin(), nei.end(), [this](int a,int b){return d[a]>d[b];});
        //     for(int v:nei)
        //     {
        //         if (!vis[v])
        //         {
        //             q.push(v);
        //             vis[v] = 1 + cnt;
        //             id[cnt++] = v;
        //             if (cnt == range || --max_neighbor == 0)
        //                 break;
        //         }
        //     }
        // }
        while (q.size() && cnt < range) // 对于已加入的u，从u邻居的sqrt(n)个邻居中选出最好的3个
        {
            ui u = q.front();
            q.pop();
            int max_neighbor = 3;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                {
                    q.push(v);
                    vis[v] = 1 + cnt;
                    id[cnt++] = v;
                    if (cnt == range || --max_neighbor == 0)
                        break;
                }
            }
        }
        for (ui i = 0; i < n && cnt < range; i++) // if bfs didn't find range vertices, we fill the rest
            if (!vis[i])
            {
                vis[i] = 1 + cnt;
                id[cnt++] = i;
            }
        // induce a subgraph with vertices in id[]
        vector<vector<ui>> neighbor(range);
        vector<ui> d(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[id[u]]; i < pstart[id[u] + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                    continue;
                neighbor[u].push_back(vis[v] - 1);
            }
            d[u] = neighbor[u].size();
        }
        return sqrt_degeneracy(range, neighbor, d, id, solution);
    }
    /**
     * @brief stage-I: select a vertex (with max degree) as start vertex
     *
     * @param solution the solution set of heuristic plex
     * @param cnt the number of subgraphs
     *
     * @return lb
     */
    int sqrt_degeneracy(set<ui> *solution = nullptr, ui cnt = 5)
    {
        set<pii> s;
        for (ui i = 0; i < n; i++)
        {
            if (s.size() < cnt)
                s.insert({d[i], i});
            else if (d[i] > s.begin()->x)
            {
                s.erase(s.begin());
                s.insert({d[i], i});
            }
        }
        int ret = paramK;
        for (auto &h : s)
            ret = max(ret, sqrt_degeneracy(h.y, solution));
        ret = max(ret, first_sqrt_vertices_degeneracy(solution));
        return ret;
    }
    /**
     * @brief stage-IV: degeneracy of G[{0,1,...,sqrt(n)}]
     *
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int first_sqrt_vertices_degeneracy(set<ui> *solution = nullptr)
    {
        ui range = sqrt(n) + 1;
        vector<vector<ui>> neighbor(range);
        vector<ui> d(range, 0), id(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (v >= range)
                    break;
                neighbor[u].push_back(v);
            }
            d[u] = neighbor[u].size();
            id[u] = u;
        }
        return sqrt_degeneracy(range, neighbor, d, id, solution);
    }
    /**
     * useless, just a demo of usage of pstart[] and edge_to[]
     */
    vector<ui> neighbor(ui u)
    {
        vector<ui> ret;
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            ret.push_back(v);
        }
        return ret;
    }
    /**
     * @brief for u in G, is d[u]+k<=lb, remove u
     *
     * T(n)=O(n+m)
     * after reduction, the graph is re-built so that each vertex's id ∈[0, n-1], the map is stored in map_refresh_id
     */
    void weak_reduce(int lb)
    {
        ui *q = new ui[n + 1]; // queue
        vector<bool> rm(n, 0); // rm[u]=1 <=> u is removed
        ui hh = 1, tt = 0;     // used for queue
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK <= lb)
                q[++tt] = i, rm[i] = 1;
        if (tt == 0) // q is empty
        {
            if (!reduced)
            {
                reduced = 1;
                for (ui i = 0; i < n; i++)
                    map_refresh_id[i] = i;
            }
            delete[] q;
            return;
        }
        while (hh <= tt)
        {
            ui u = q[hh++];
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (rm[v])
                    continue;
                if (--d[v] + paramK <= lb)
                    q[++tt] = v, rm[v] = 1;
            }
        }
        // re-build the graph : re-map the id of the rest vertices; the array q[] is recycled to save the map
        ui new_n = 0;
        if (reduced)
        {
            unordered_map<ui, ui> new_map;
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    new_map[new_n] = map_refresh_id[i];
                    q[i] = new_n++;
                }
            map_refresh_id = new_map;
        }
        else
        {
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    map_refresh_id[new_n] = i;
                    q[i] = new_n++;
                }
            reduced = true;
        }
        ui *new_pstart = new ui[new_n + 1];
        ui *new_d = new ui[new_n];
        ui j = 0; // we don't need extra memory to store new-edge_to, just re-use edge_to[]
        for (ui i = 0; i < n; i++)
        {
            if (rm[i])
                continue;
            ui u = q[i];
            new_pstart[u] = j;
            for (ui p = pstart[i]; p < pstart[i + 1]; p++)
            {
                ui v = edge_to[p];
                if (!rm[v])
                    edge_to[j++] = q[edge_to[p]];
            }
            new_d[u] = j - new_pstart[u];
        }
        new_pstart[new_n] = j;
        delete[] d;
        delete[] pstart;
        d = new_d;
        pstart = new_pstart;
        // you can ignore the following
        if (j * 2 < m)
        {
            ui *new_edge_to = new ui[j];
            memcpy(new_edge_to, edge_to, sizeof(ui) * j);
            delete[] edge_to;
            edge_to = new_edge_to;
        }
        m = j;
        n = new_n;
        delete[] q;
    }
    /**
     * @brief for u in G, is d[u]+k<=lb, remove u but don't update degree
     *
     * T(n)=O(n) for reduction, O(m) for re-build graph but m is much smaller than input
     * after reduction, the graph is re-built so that each vertex's id ∈[0, n-1], the map is stored in map_refresh_id
     */
    void fast_weak_reduce(int lb)
    {
        vector<bool> rm(n, 0); // rm[u]=1 <=> u is removed
        int rm_cnt = 0;
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK <= lb)
                rm[i] = 1, rm_cnt++;
        if (rm_cnt == 0) // q is empty
        {
            if (!reduced)
            {
                reduced = 1;
                for (ui i = 0; i < n; i++)
                    map_refresh_id[i] = i;
            }
            return;
        }
        // re-build the graph : re-map the id of the rest vertices; the array q[] is recycled to save the map
        ui *q = new ui[n + 1];
        ui new_n = 0;
        if (reduced)
        {
            unordered_map<ui, ui> new_map;
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    new_map[new_n] = map_refresh_id[i];
                    q[i] = new_n++;
                }
            map_refresh_id = new_map;
        }
        else
        {
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    map_refresh_id[new_n] = i;
                    q[i] = new_n++;
                }
            reduced = true;
        }
        ui *new_pstart = new ui[new_n + 1];
        ui *new_d = new ui[new_n];
        ui j = 0; // we don't need extra memory to store new-edge_to, just re-use edge_to[]
        for (ui i = 0; i < n; i++)
        {
            if (rm[i])
                continue;
            ui u = q[i];
            new_pstart[u] = j;
            for (ui p = pstart[i]; p < pstart[i + 1]; p++)
            {
                ui v = edge_to[p];
                if (!rm[v])
                    edge_to[j++] = q[edge_to[p]];
            }
            new_d[u] = j - new_pstart[u];
        }
        new_pstart[new_n] = j;
        delete[] d;
        delete[] pstart;
        d = new_d;
        pstart = new_pstart;
        // you can ignore the following
        ui *new_edge_to = new ui[j];
        memcpy(new_edge_to, edge_to, sizeof(ui) * j);
        delete[] edge_to;
        edge_to = new_edge_to;
        m = j;
        n = new_n;
        delete[] q;
    }
    /**
     * @brief degenaracy order to get lb, i.e., each time we remove the vertex with min degree
     *
     * @param solution if not NULL, the heuristic result should be stored
     *
     * @return lb
     *
     * T(n)=O(n+m)
     */
    int degeneracyLB(set<ui> *solution = nullptr)
    {
        vector<bool> rm(n, 0); // rm[u]=1 <==> u is peeled and removed
        int *pd = new int[n];  // copy of d[]
        memcpy(pd, d, sizeof(int) * n);
        LinearHeap heap(n, n, pd);
        while (heap.get_min_key() + paramK < heap.sz)
        {
            ui u = heap.get_min_node();
            assert(u < n);
            heap.delete_node(u);
            rm[u] = 1;
            // update the degrees of the rest vertices
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                assert(v < n);
                if (!rm[v])
                {
                    assert(pd[v] > 0);
                    heap.decrease(--pd[v], v);
                }
            }
        }
        int rest = heap.sz;
        set<ui> plex;
        while (heap.sz)
        {
            ui u = heap.get_min_node();
            heap.delete_node(u);
            plex.insert(u);
        }
        // for(ui i=0;i<n;i++)
        // {
        //     if(!rm[i]) continue;
        //     //check whether i can be included into plex
        //     int d_i=0;
        //     bool ok=1;
        //     for(ui v:plex)
        //     {
        //         if(exist_edge(i, v))
        //             d_i++;
        //         else if(pd[v]+paramK == plex.size())
        //         {
        //             ok=0;
        //             break;
        //         }
        //     }
        //     if(ok && d_i+paramK >= plex.size()+1)
        //     {
        //         pd[i]=d_i;
        //         for(ui v:plex)
        //         {
        //             if(exist_edge(i, v))
        //                 pd[v]++;
        //         }
        //         plex.insert(i);
        //     }
        // }
        // next, we extend the plex to maximal
        delete[] pd;
        if (solution != nullptr && solution->size() < plex.size())
        {
            solution->clear();
            if (reduced)
            {
                for (ui i : plex)
                    solution->insert(map_refresh_id[i]);
            }
            else
            {
                for (ui i : plex)
                    solution->insert(i);
            }
        }
        return plex.size();
    }
    /**
     * @brief dump the reduced graph to file; abandoned now
     */
    void dump_to_file(string path, set<ui> *must_contain = nullptr)
    {
        FILE *out = fopen(path.c_str(), "w");
        if (out == nullptr)
        {
            printf("File open failed: %s\n", path.c_str());
            exit(1);
        }
        if (!n)
            m = 0;
        fprintf(out, "%u %u %d\n", n, m, lb);
        for (ui i = 0; i < n; i++)
        {
            for (ui j = pstart[i]; j < pstart[i + 1]; j++)
            {
                ui v = edge_to[j];
                fprintf(out, "%u %u\n", i, v);
            }
        }
        fprintf(out, "\n");
        // dump the map
        for (ui i = 0; i < n; i++)
            fprintf(out, "%u\n", map_refresh_id[i]);

        fprintf(out, "\n\n%d\n", must_contain == nullptr ? 0 : (int)must_contain->size());
        if (must_contain != nullptr)
            for (ui v : (*must_contain))
                fprintf(out, "%u\n", v);
        fclose(out);
    }
};

/**
 * base class : can be implemented using adj-matrix or adj-list
 */
class Graph_reduced
{
public:
    LinearHeap heap; // used for degeneracy & IE
    MyBitset vertex;
    ll n, m;
    vector<int> vertex_id; // for u in this, vertex_id[u] in G_input
    vector<int> must_contain;
    unordered_map<ll, int> triangles_nn; // the key of edge (u,v) is u*n+v , make sure u<v
    int *d;                              // degree
    AjacentMatrix A;
    // shared memory for CTCP
    vector<bool> bool_array_n_n, bool_array_m, bool_array_n;
    int *pstart, *edge_to, *triangles_m;
    vector<bool> edge_removed;
    Graph_reduced() : d(nullptr), pstart(nullptr), edge_to(nullptr), triangles_m(nullptr)
    {
    }
    /**
     * @return edge number
     */
    int get_m()
    {
        int ret;
        for (int v : vertex)
        {
            ret += d[v];
        }
        return ret >> 1;
    }
    /**
     * @brief prepare for IE & degeneracy
     */
    void init_heap()
    {
        heap = LinearHeap(n, n, d);
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    virtual void init_triangles()
    {
    }
    /**
     * @brief prepare some thing
     */
    virtual void init_before_IE()
    {
    }
    /**
     * @return the vertex with min degree
     */
    int get_min_degree_v()
    {
        int ret = heap.get_min_node();
        return ret;
    }
    /**
     * @brief after searching subgraph g_i, we exclude v_i and update the number of triangles of edges related to v_i
     * @param v the vertex we need to remove, i.e., v_i
     * @param lb_changed if so, we need to check each edge whether it can be reduced
     */
    void remove_v(int v, int lb, bool lb_changed)
    {
        CTCP(lb, v);
        if (lb_changed)
        {
            CTCP(lb);
        }
        // cout<<v<<' '<<vertex.size()<<' '<<get_m()<<endl;
        // if(v==169 || v==160)
        // {
        //     for (int v : vertex)
        //     {
        //         for (int i = pstart[v]; i < pstart[v + 1]; i++)
        //         {
        //             int w = edge_to[i];
        //             if (!vertex[w] || edge_removed[i])
        //                 continue;
        //             int cnt = 0;
        //             for (int j = pstart[w]; j < pstart[w + 1]; j++)
        //             {
        //                 if (edge_removed[j])
        //                     continue;
        //                 int x = edge_to[j];
        //                 if (!vertex[x])
        //                     continue;
        //                 if (has(edge_to + pstart[v], edge_to + pstart[v + 1], x))
        //                 {
        //                     cnt++;
        //                 }
        //             }
        //             // assert(cnt==triangles_m[i]);
        //         }
        //     }
        // }
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    virtual void CTCP(int lb, int v = -1)
    {
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     */
    virtual void induce_to_2hop(MyBitset &vis, int v)
    {
    }
    /**
     * @brief obtain the ground-truth: the maximum k-plex
     * @param s the max plex we found (without considering the vertices that must include)
     *          so we need to add must_contain to s
     * @param trans_id whether we need to transform the index
     */
    void get_ground_truth(set<int> &s, bool trans_id)
    {
        if (trans_id)
        {
            set<int> temp;
            for (int v : s)
                temp.insert(vertex_id[v]);
            s = temp;
        }
        s.insert(must_contain.begin(), must_contain.end());
    }
    /**
     * @return the number of vertices now
     */
    int size()
    {
        return vertex.size();
    }
    /**
     * @return whether the graph is stored as matrix
     */
    bool is_matrix()
    {
        return edge_to == nullptr;
    }
};

/**
 * used for IE; maybe not dense enough for using adjacent matrix
 * when g_i is searched and we exclude v_i, we need to invoke CTCP
 * note that this class can be merged with Graph; but we choose not to merge just for module
 */
class Graph_reduced_adjacent_list : public Graph_reduced
{
public:
    Graph_reduced_adjacent_list() : Graph_reduced() {}
    /**
     * init graph after stage-I(preprocessing)
     * @param g reduced graph
     * @param must the vertex set that must include because each vertex in it will occur in a maximum k-plex
     */
    Graph_reduced_adjacent_list(Graph &g, set<ui> &must) : Graph_reduced()
    {
        n = g.n;
        m = g.m;
        edge_removed.resize(m);
        printf("reduced graph n= %d m= %d lb= %d\n", n, m, lb);
        pstart = new int[n + 1];
        edge_to = new int[m];
        d = new int[n];
        for (ui i = 0; i < n; i++)
        {
            pstart[i] = g.pstart[i];
            d[i] = g.pstart[i + 1] - g.pstart[i];
            for (ui j = g.pstart[i]; j < g.pstart[i + 1]; j++)
            {
                edge_to[j] = g.edge_to[j];
            }
        }
        pstart[n] = g.pstart[n];
        vertex_id.resize(n);
        for (int i = 0; i < n; i++)
        {
            vertex_id[i] = g.map_refresh_id[i];
        }
        for (int u : must)
            must_contain.push_back(u);
        vertex = MyBitset(n);
        vertex.flip();
        printf("Graph for bnb init ok\n");
        fflush(stdout);
    }
    ~Graph_reduced_adjacent_list()
    {
        if (d != nullptr)
        {
            delete[] d;
            d = nullptr;
        }
        if (edge_to != nullptr)
        {
            delete[] edge_to;
            edge_to = nullptr;
        }
        if (pstart != nullptr)
        {
            delete[] pstart;
            pstart = nullptr;
        }
        if (triangles_m != nullptr)
        {
            delete[] triangles_m;
            triangles_m = nullptr;
        }
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    void init_triangles()
    {
        triangles_m = new int[m];
        MyBitset mask(n);
        for (int u = 0; u < n; u++)
        {
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
                mask.set(edge_to[i]);
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
            {
                int v = edge_to[i];
                if (d[v] > d[u])
                    continue;
                int cnt = 0;
                for (int j = pstart[v]; j < pstart[v + 1]; j++)
                {
                    int w = edge_to[j];
                    if (mask[w]) // w is common neighbor of u,v
                        cnt++;
                }
                triangles_m[i] = cnt;
            }
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
                mask.reset(edge_to[i]);
        }
        for (int u = 0; u < n; u++)
        {
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
            {
                int v = edge_to[i];
                if (d[v] <= d[u])
                    continue;
                triangles_m[i] = triangles_m[find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v]];
            }
        }
    }
    /**
     * @brief prepare some thing
     */
    void init_before_IE()
    {
        init_triangles();
        init_heap();
        bool_array_n.resize(n);
        bool_array_m.resize(m);
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    void CTCP(int lb, int v = -1)
    {
        queue<pii> q_edges; // an edge is stored as (edge_id, from)
        queue<int> q_vertex;
        vector<bool> &in_queue_e = bool_array_m; //(u,v) is already pushed into queue if in_queue_e[edge_id]=1
        vector<bool> &in_queue_v = bool_array_n; // a vertex u is already pushed into queue if in_queue_v[u]=1
        // CTCP is called because lb updated
        if (v == -1)
        {
            for (int u : vertex)
            {
                for (int i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (edge_removed[i])
                        continue;
                    int v = edge_to[i];
                    if (!vertex[v])
                        continue;
                    if (triangles_m[i] + paramK * 2 <= lb)
                    {
                        if (u < v)
                            q_edges.push({i, u});
                        in_queue_e[i] = true;
                    }
                }
                if (d[u] + paramK <= lb)
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
            }
        }
        else // CTCP is called because IE removed a vertex
        {
            q_vertex.push(v);
            in_queue_v[v] = 1;
        }
        while (q_edges.size() || q_vertex.size())
        {
            while (q_edges.size())
            {
                auto edge = q_edges.front();
                q_edges.pop();
                int edge_id = edge.x, u = edge.y, v = edge_to[edge_id];
                assert(!edge_removed[edge_id]);
                edge_removed[edge_id] = 1;
                int another_edge_id = find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v];
                assert(!edge_removed[another_edge_id]);
                edge_removed[another_edge_id] = 1;
                if (--d[u] + paramK <= lb && !in_queue_v[u])
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
                heap.decrease(d[u], u);
                if (--d[v] + paramK <= lb && !in_queue_v[v])
                {
                    q_vertex.push(v);
                    in_queue_v[v] = 1;
                }
                heap.decrease(d[v], v);
                int *a = edge_to + pstart[u], *b = edge_to + pstart[u + 1];
                int *l = edge_to + pstart[v], *r = edge_to + pstart[v + 1];
                // update the info of other edge triangles
                while (a < b && l < r)
                {
                    // note that we need to consider a situation where an edge is in the q_edges but not removed yet
                    if (edge_removed[a - edge_to] || in_queue_v[*a])
                    {
                        a++;
                        continue;
                    }
                    if (edge_removed[l - edge_to] || in_queue_v[*l])
                    {
                        l++;
                        continue;
                    }
                    if (*a == *l)
                    {
                        int w = *a; // w is the common neighbor of u,v
                        int id_uw = a - edge_to;
                        int id_wu = find(edge_to + pstart[w], edge_to + pstart[w + 1], u) + pstart[w];
                        assert(!edge_removed[id_wu]);
                        if (!in_queue_e[id_uw])
                        {
                            assert(!in_queue_e[id_wu]);
                            --triangles_m[id_uw];
                            --triangles_m[id_wu];
                            if (triangles_m[id_uw] + paramK * 2 <= lb)
                            {
                                in_queue_e[id_uw] = in_queue_e[id_wu] = 1;
                                q_edges.push({id_uw, u});
                            }
                        }
                        ui id_vw = l - edge_to;
                        ui id_wv = find(edge_to + pstart[w], edge_to + pstart[w + 1], v) + pstart[w];
                        if (!in_queue_e[id_vw])
                        {
                            assert(!in_queue_e[id_wv]);
                            --triangles_m[id_vw];
                            --triangles_m[id_wv];
                            if (triangles_m[id_vw] + paramK * 2 <= lb)
                            {
                                in_queue_e[id_vw] = in_queue_e[id_wv] = 1;
                                q_edges.push({id_vw, v});
                            }
                        }
                        a++;
                        l++;
                    }
                    else if (*a < *l)
                        a++;
                    else
                        l++;
                }
            }
            if (q_vertex.size())
            {
                ui u = q_vertex.front();
                q_vertex.pop();
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (in_queue_e[i])
                        continue;
                    ui v = edge_to[i];
                    if (in_queue_v[v])
                        continue;
                    if (--d[v] + paramK <= lb)
                    {
                        in_queue_v[v] = 1;
                        q_vertex.push(v);
                    }
                    heap.decrease(d[v], v);
                }
                // update the triangles containing u
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (in_queue_e[i])
                        continue;
                    int v = edge_to[i];
                    if (in_queue_v[v])
                        continue;
                    int *st = edge_to + pstart[v], *ed = edge_to + pstart[v + 1];
                    for (ui j = i + 1; j < pstart[u + 1]; j++)
                    {
                        if (in_queue_e[j])
                            continue;
                        int w = edge_to[j]; // v w are neighbors of u
                        if (in_queue_v[w])
                            continue;
                        if (has(st, ed, w)) // v is connected to w
                        {
                            ui id_vw = find(st, ed, w) + pstart[v];
                            ui id_wv = find(edge_to + pstart[w], edge_to + pstart[w + 1], v) + pstart[w];
                            assert(triangles_m[id_vw] == triangles_m[id_wv]);
                            assert(edge_to[id_vw] == w);
                            assert(edge_to[id_wv] == v);
                            if (in_queue_e[id_vw])
                                continue;
                            --triangles_m[id_wv];
                            --triangles_m[id_vw];
                            if (triangles_m[id_vw] + 2 * paramK <= lb)
                            {
                                in_queue_e[id_vw] = in_queue_e[id_wv] = 1;
                                q_edges.push({id_vw, v});
                            }
                        }
                    }
                }
                vertex.reset(u);
                heap.delete_node(u);
            }
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     */
    void induce_to_2hop(MyBitset &vis, int v)
    {
        assert(vertex[v]);
        for (int i = pstart[v]; i < pstart[v + 1]; i++)
        {
            if (edge_removed[i])
                continue;
            int a = edge_to[i];
            if (!vertex[a])
                continue;
            if (!vis[a])
                vis.set(a);
            for (int j = pstart[a]; j < pstart[a + 1]; j++)
            {
                if (edge_removed[j])
                    continue;
                int b = edge_to[j];
                if (!vertex[b])
                    continue;
                if (!vis[b])
                    vis.set(b);
            }
        }
    }
};

/**
 * used for IE; maybe not dense enough for using adjacent matrix
 * when g_i is searched and we exclude v_i, we need to invoke CTCP
 * note that this class can be merged with Graph; but we choose not to merge just for module
 */
class Graph_reduced_adjacent_matrix : public Graph_reduced
{
public:
    Graph_reduced_adjacent_matrix() : Graph_reduced() {}
    /**
     * init graph after stage-I(preprocessing)
     * @param g reduced graph
     * @param must the vertex set that must include because each vertex in it will occur in a maximum k-plex
     */
    Graph_reduced_adjacent_matrix(Graph &g, set<ui> &must) : Graph_reduced()
    {
        n = g.n;
        m = g.m;
        printf("reduced graph n= %d m= %d lb= %d\n", n, m, lb);
        A = AjacentMatrix(n);
        for (ui i = 0; i < n; i++)
        {
            for (ui j = g.pstart[i]; j < g.pstart[i + 1]; j++)
            {
                ui v = g.edge_to[j];
                if (i < v)
                    A.add_edge(i, v);
            }
        }
        vertex_id.resize(n);
        for (int i = 0; i < n; i++)
        {
            vertex_id[i] = g.map_refresh_id[i];
        }
        for (int u : must)
            must_contain.push_back(u);
        vertex = MyBitset(n);
        vertex.flip();

        d = new int[n];
        for (int i = 0; i < n; i++)
            d[i] = A[i].size();
        printf("Graph for bnb init ok\n");
        fflush(stdout);
    }
    ~Graph_reduced_adjacent_matrix()
    {
        if (d != nullptr)
        {
            delete[] d;
        }
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    void init_triangles()
    {
        for (int u = 0; u < n; u++)
        {
            for (int v : A[u])
            {
                if (v >= u)
                    break;
                triangles_nn[v * n + u] = A[v].intersect(A[u]);
            }
        }
    }
    /**
     * @brief prepare some thing
     */
    void init_before_IE()
    {
        init_triangles();
        init_heap();
        bool_array_n.resize(n);
        bool_array_n_n.resize(n * n);
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    void CTCP(int lb, int v = -1)
    {
        queue<pii> q_edges; // an edge is stored as (u,v) where u<v
        queue<int> q_vertex;
        vector<bool> &in_queue_e = bool_array_n_n; //(u,v) is already pushed into queue if in_queue_e[u*n+v]=1
        vector<bool> &in_queue_v = bool_array_n;   // a vertex u is already pushed into queue if in_queue_v[u]=1
        // CTCP is called because lb updated
        if (v == -1)
        {
            for (int u : vertex)
            {
                for (int v : A[u])
                {
                    if (v >= u)
                        break;
                    if (triangles_nn[v * n + u] + paramK * 2 <= lb)
                    {
                        q_edges.push({v, u});
                        in_queue_e[v * n + u] = true;
                    }
                }
                if (d[u] + paramK <= lb)
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
            }
        }
        else // CTCP is called because IE removed a vertex
        {
            q_vertex.push(v);
            in_queue_v[v] = 1;
        }
        while (q_edges.size() || q_vertex.size())
        {
            while (q_edges.size())
            {
                auto edge = q_edges.front();
                q_edges.pop();
                int u = edge.x, v = edge.y; // u<v
                assert(u < v);
                assert(A[u][v] && A[v][u]);
                // remove this edge and update the degree
                A[u].reset(v);
                A[v].reset(u);
                heap.decrease(--d[u], u);
                heap.decrease(--d[v], v);
                if (d[u] + paramK <= lb && !in_queue_v[u])
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
                if (d[v] + paramK <= lb && !in_queue_v[v])
                {
                    q_vertex.push(v);
                    in_queue_v[v] = 1;
                }
                // update the number of triangles of other edges
                auto common_neighbor = A[u];
                common_neighbor &= A[v];
                for (int w : common_neighbor)
                {
                    if (in_queue_v[w])
                        continue;
                    ll edge_uw = u < w ? u * n + w : w * n + u;
                    if (!in_queue_e[edge_uw])
                    {
                        if (--triangles_nn[edge_uw] + paramK * 2 <= lb)
                        {
                            pii edge = {u, w};
                            if (w < u)
                                edge = {w, u};
                            q_edges.push(edge);
                            in_queue_e[edge_uw] = 1;
                        }
                    }
                    ll edge_vw = v < w ? v * n + w : w * n + v;
                    if (!in_queue_e[edge_vw])
                    {
                        if (--triangles_nn[edge_vw] + paramK * 2 <= lb)
                        {
                            pii edge = {v, w};
                            if (w < v)
                                edge = {w, v};
                            q_edges.push(edge);
                            in_queue_e[edge_vw] = 1;
                        }
                    }
                }
            }

            if (q_vertex.size())
            {
                int u = q_vertex.front();
                q_vertex.pop();
                // update the degree
                for (int v : A[u])
                {
                    if (in_queue_v[v])
                        continue;
                    ll edge_uv = u < v ? u * n + v : v * n + u;
                    if (in_queue_e[edge_uv])
                        continue;
                    heap.decrease(--d[v], v);
                    if (d[v] + paramK <= lb)
                    {
                        q_vertex.push(v);
                        in_queue_v[v] = 1;
                    }
                    A[v].reset(u); // remove edge (u,v)
                }
                // update the number of triangles of edges
                for (int v : A[u])
                {
                    ll edge_uv = u < v ? u * n + v : v * n + u;
                    assert(!in_queue_e[edge_uv]);
                    if (in_queue_v[v])
                        continue;
                    for (int w : A[u])
                    {
                        if (w == v)
                            break;
                        if (in_queue_v[w])
                            continue;
                        ll edge_uw = u < w ? u * n + w : w * n + u;
                        assert(!in_queue_e[edge_uw]);
                        if (!A[v][w])
                            continue;
                        assert(A[w][v]);
                        assert(w < v);
                        ll edge_vw = w * n + v;
                        assert(!in_queue_e[edge_vw]);
                        if (--triangles_nn[edge_vw] + paramK * 2 <= lb)
                        {
                            pii edge = {w, v};
                            q_edges.push(edge);
                            in_queue_e[edge_vw] = 1;
                        }
                    }
                }
                // remove this vertex
                vertex.reset(u);
                A[u].clear();
                heap.delete_node(u);
            }
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     */
    void induce_to_2hop(MyBitset &vis, int v)
    {
        for (int u : A[v])
        {
            assert(vertex[u]);
            if (!vis[u])
                vis.set(u);
            vis |= A[u];
        }
    }
};

/**
 * used for bnb, i.e., each 2-hop induced subgraph is stored using adjacent matrix
 */
class Graph_adjacent
{
public:
    double init_time; // used for log
    AjacentMatrix adj_matrix;
    int n;
    vector<int> vertex_id; // for u in this, vertex_id[u] in G_reduced
    Graph_adjacent() : init_time(0) {}
    /**
     * @brief given vertex set V_mask, induce subgraph
     *
     * @param V_mask V_mask[u]=1 <==> u in the subgraph
     * @param g reduced graph which use adj-list to store edges
     * @param inv each vertex in subgraph is [0, n-1], so we need to save the origin index
     */
    Graph_adjacent(MyBitset &V_mask, Graph_reduced &g, vector<int> &inv)
    {
        if (g.is_matrix())
        {
            init_from_A(V_mask, g.A, inv);
        }
        else // init from adj-list
        {
            for (int i : V_mask)
            {
                vertex_id.push_back(i);
            }
            n = vertex_id.size();

            adj_matrix = AjacentMatrix(n);

            int id = 0;
            for (int i = 0; i < vertex_id.size(); i++)
                inv[vertex_id[i]] = i;
            Timer t;
            for (int u : V_mask)
            {
                for (int i = g.pstart[u]; i < g.pstart[u + 1]; i++)
                {
                    if (g.edge_removed[i])
                        continue;
                    int v = g.edge_to[i];
                    if (v >= u)
                        break;
                    if (!V_mask[v])
                        continue;
                    adj_matrix.add_edge(inv[u], inv[v]);
                }
            }
            init_time = t.get_time();
        }
    }
    /**
     * @brief given vertex set V_mask, induce subgraph
     *
     * @param V_mask V_mask[u]=1 <==> u in the subgraph
     * @param inv each vertex in subgraph is [0, n-1], so we need to save the origin index
     */
    void init_from_A(MyBitset &V_mask, AjacentMatrix &A, vector<int> &inv)
    {
        for (int i : V_mask)
        {
            vertex_id.push_back(i);
        }
        n = vertex_id.size();

        adj_matrix = AjacentMatrix(n);

        int id = 0;
        for (int i = 0; i < vertex_id.size(); i++)
            inv[vertex_id[i]] = i;
        double start_init = get_system_time_microsecond();
        for (int u : V_mask)
        {
            auto nei = V_mask;
            nei &= A[u];
            for (int v : nei)
            {
                if (v >= u)
                    break;
                adj_matrix.add_edge(inv[u], inv[v]);
            }
        }
        init_time = get_system_time_microsecond() - start_init;
    }
    /**
     * useless, just a demo
     */
    bool exist_edge(int a, int b)
    {
        return adj_matrix[a][b];
    }
    /**
     * @brief all vertices in s are in the subgraph g_i, and we need to know the origin indices of them
     */
    set<int> get_ori_vertices(set<int> &s)
    {
        set<int> ret;
        for (int v : s)
            ret.insert(vertex_id[v]);
        return ret;
    }
    /**
     * @brief obtain the number of vertices
     */
    int size()
    {
        return n;
    }
};

#endif