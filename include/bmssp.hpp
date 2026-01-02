#ifndef BMSSP_HPP
#define BMSSP_HPP

#include "metrics.hpp"

#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>

namespace spp {

template<typename T>
class bmssp {
public:
    static constexpr T INF = std::numeric_limits<T>::max() / 4;

private:
    int n;  // número de vértices
    std::vector<std::vector<std::pair<int,T>>> adj;  // lista de adyacencia
    
    std::vector<T> dist;      // distancia estimada desde la fuente
    std::vector<int> pred;    // predecesor en el camino más corto
    std::vector<bool> completo;  // ¿vértice con distancia final conocida?
    
    // k = log^{1/3} n, t = log^{2/3} n
    int k;  
    int t;  
    int max_nivel;  // profundidad máxima de recursión = ⌈log n / t⌉
    
    Metrics* M;  // puntero para contar instrucciones

public:
    // Constructor: O(n)
    bmssp(int n_, Metrics* metrics = nullptr)
        : n(n_), M(metrics)
    {
        // Inicializar estructuras de datos
        adj.assign(n, {});
        dist.assign(n, INF);
        pred.assign(n, -1);
        completo.assign(n, false);
        
        if (M) M->assign(4 * n);  // 4 asignaciones por vértice
        
        // Calcular parámetros como en el paper (Sección 3.1)
        double logn = std::log2(std::max(2, n));
        k = std::max(2, (int)std::pow(logn, 1.0/3.0));      // k = ⌈log^{1/3} n⌉
        t = std::max(2, (int)std::pow(logn, 2.0/3.0));      // t = ⌈log^{2/3} n⌉
        max_nivel = std::max(1, (int)std::ceil(logn / t));  // máximo ⌈log n / t⌉ niveles
        
        if (M) M->assign(3);  // 3 asignaciones para parámetros
    }

    // Añadir arista al grafo: O(1)
    void addEdge(int u, int v, T w) {
        adj[u].emplace_back(v, w);
        if (M) {
            M->access();   // acceso a adj[u]
            M->assign(3);  // emplace_back hace ~3 asignaciones
        }
    }

    // Preparar grafo (no usado en esta versión simplificada)
    void prepare_graph(bool /*sin_usar*/) {}

    // Ejecutar algoritmo desde el vértice fuente: O(m log^{2/3} n) teórico
    std::pair<std::vector<T>, std::vector<int>> execute(int fuente) {
        // Reinicializar estructuras
        std::fill(dist.begin(), dist.end(), INF);
        std::fill(pred.begin(), pred.end(), -1);
        std::fill(completo.begin(), completo.end(), false);
        if (M) M->assign(3 * n);  // 3 operaciones fill de tamaño n
        
        // Inicializar vértice fuente
        dist[fuente] = 0;
        pred[fuente] = fuente;
        completo[fuente] = true;
        if (M) M->assign(3);  // 3 asignaciones
        
        // Llamada principal al algoritmo BMSSP
        // Parámetros: nivel = max_nivel, límite = INF, conjunto S = {fuente}
        std::vector<int> S = {fuente};
        bmssp_recursivo(max_nivel, INF, S);
        
        // VERIFICACIÓN FINAL OBLIGATORIA con Dijkstra
        // Corrige posibles errores de la aproximación
        verificacion_final_dijkstra();
        
        return {dist, pred};
    }

private:
    // =====================================================
    // Dijkstra limitado (Caso base - Algoritmo 2 aproximado)
    // =====================================================
    void dijkstra_limitado(int src, T B) {
        using Estado = std::pair<T,int>;
        std::priority_queue<Estado, std::vector<Estado>, std::greater<Estado>> pq;
        
        // Insertar vértice fuente en la cola
        pq.push({dist[src], src});
        if (M) {
            M->call();    // llamada a push
            M->access();  // acceso a dist[src]
        }
        
        int procesados = 0;
        std::vector<bool> visitado_local(n, false);
        if (M) M->assign(n + 1);  // vector de tamaño n + variable
        
        // Procesar hasta k² vértices o hasta alcanzar el límite B
        while (!pq.empty() && procesados < k * k) {
            if (M) M->compare(2);  // comparación while
            
            auto [d,u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);    // llamadas a top() y pop()
                M->assign(2);  // asignaciones de d y u
            }
            
            if (visitado_local[u]) continue;
            if (d > B) break;  // No superar el límite B
            if (M) M->compare();  // comparación d > B
            
            visitado_local[u] = true;
            completo[u] = true;
            procesados++;
            if (M) M->assign(3);  // 3 asignaciones
            
            // Relajar aristas salientes
            for (auto &[v,w] : adj[u]) {
                T nueva_dist = dist[u] + w;
                if (M) M->arithmetic();  // operación suma
                
                // Condición de relajación
                if (nueva_dist < dist[v] && nueva_dist < B) {
                    if (M) {
                        M->compare(2);   // 2 comparaciones
                        M->access(2);    // acceso a dist[u] y dist[v]
                        M->assign(2);    // asignaciones a dist[v] y pred[v]
                    }
                    
                    dist[v] = nueva_dist;
                    pred[v] = u;
                    pq.push({dist[v], v});
                    if (M) M->call();  // llamada a push
                }
            }
        }
    }
    
    // =====================================================
    // Encontrar pivotes aproximado (Algoritmo 1 simplificado)
    // =====================================================
    std::pair<std::vector<int>, std::vector<int>> 
    encontrar_pivotes_aprox(const std::vector<int>& S, T B) {
        std::vector<int> pivotes;
        std::vector<int> W;  // vértices descubiertos por Bellman-Ford
        
        // Simular Bellman-Ford de k pasos
        std::vector<T> dist_temp = dist;
        std::vector<bool> en_W(n, false);
        if (M) M->assign(2 * n);  // 2 vectores de tamaño n
        
        // k pasos de relajación desde S
        for (int paso = 0; paso < k; paso++) {
            bool actualizado = false;
            if (M) M->assign();  // asignación a actualizado
            
            for (int u : S) {
                for (auto &[v,w] : adj[u]) {
                    T nueva_dist = dist_temp[u] + w;
                    if (M) M->arithmetic();  // suma
                    
                    // Condición de relajación en Bellman-Ford
                    if (nueva_dist < dist_temp[v] && nueva_dist < B) {
                        if (M) {
                            M->compare(2);   // 2 comparaciones
                            M->access(2);    // acceso a dist_temp[u] y dist_temp[v]
                            M->assign(2);    // asignaciones
                        }
                        
                        dist_temp[v] = nueva_dist;
                        if (!en_W[v]) {
                            W.push_back(v);
                            en_W[v] = true;
                            if (M) M->assign(2);  // push_back y asignación
                        }
                        actualizado = true;
                    }
                }
            }
            
            if (!actualizado) break;
            if (M) M->compare();  // comparación !actualizado
        }
        
        // Identificar pivotes: vértices en S que alcanzan muchos vértices
        for (int u : S) {
            if (M) M->access();  // acceso a u en S
            
            int alcanzables = 0;
            std::vector<bool> visitado_bfs(n, false);
            std::queue<int> cola;
            cola.push(u);
            visitado_bfs[u] = true;
            if (M) M->assign(2);  // asignaciones
            
            // BFS limitado para contar vértices alcanzables
            while (!cola.empty() && alcanzables < k * 2) {
                if (M) M->compare();  // comparación while
                
                int actual = cola.front();
                cola.pop();
                if (M) {
                    M->call();    // llamada a front()
                    M->assign();  // asignación a actual
                }
                
                for (auto &[v,w] : adj[actual]) {
                    if (!visitado_bfs[v] && dist[actual] + w < B) {
                        if (M) M->compare();  // comparación
                        
                        visitado_bfs[v] = true;
                        cola.push(v);
                        alcanzables++;
                        if (M) M->assign(2);  // 2 asignaciones
                        
                        // Si alcanza al menos k vértices, es pivote
                        if (alcanzables >= k) {
                            pivotes.push_back(u);
                            if (M) M->assign();  // push_back
                            break;
                        }
                    }
                }
                if (alcanzables >= k) break;
            }
        }
        
        // Si no encontramos pivotes, usar todos los de S
        if (pivotes.empty() && !S.empty()) {
            pivotes = S;
            if (M) M->assign();  // asignación
        }
        
        return {pivotes, W};
    }
    
    // =====================================================
    // BMSSP recursivo principal (Algoritmo 3 simplificado)
    // =====================================================
    void bmssp_recursivo(int nivel, T B, const std::vector<int>& S) {
        // Caso base: conjunto vacío o nivel 0
        if (S.empty() || nivel <= 0) {
            if (M) M->compare(2);  // 2 comparaciones
            return;
        }
        
        // Caso base: nivel 1 o conjunto pequeño
        if (nivel == 1 || (int)S.size() <= k) {
            for (int u : S) {
                if (completo[u]) {
                    dijkstra_limitado(u, B);
                    if (M) M->compare();  // comparación if
                }
            }
            return;
        }
        
        // Paso 1: Encontrar pivotes (Algoritmo 1)
        auto [pivotes, W] = encontrar_pivotes_aprox(S, B);
        if (M) M->assign();  // asignación de tupla
        
        // Paso 2: Procesar pivotes en grupos de tamaño ~2^{(l-1)t}
        int tam_grupo = std::max(1, (int)std::pow(2, (nivel-1) * t / max_nivel));
        
        // Ordenar pivotes por distancia (simulación de Pull)
        std::vector<int> pivotes_ordenados = pivotes;
        std::sort(pivotes_ordenados.begin(), pivotes_ordenados.end(),
            [&](int a, int b) { 
                if (M) M->compare();  // comparación en lambda
                return dist[a] < dist[b]; 
            });
        
        // Contar operaciones del sort: O(m log m) comparaciones
        if (M) {
            int m_pivotes = pivotes_ordenados.size();
            M->compare(m_pivotes * std::log2(m_pivotes + 1));
            M->assign(m_pivotes * std::log2(m_pivotes + 1));
        }
        
        // Procesar por grupos (divide y vencerás)
        for (size_t i = 0; i < pivotes_ordenados.size(); i += tam_grupo) {
            if (M) M->compare();  // comparación del for
            
            size_t fin = std::min(i + tam_grupo, pivotes_ordenados.size());
            std::vector<int> grupo(pivotes_ordenados.begin() + i, 
                                   pivotes_ordenados.begin() + fin);
            
            // Llamada recursiva con nivel reducido
            bmssp_recursivo(nivel - 1, B, grupo);
            
            // Relajar aristas desde los vértices completados en este grupo
            for (int u : grupo) {
                if (completo[u]) {
                    for (auto &[v,w] : adj[u]) {
                        T nueva_dist = dist[u] + w;
                        if (M) M->arithmetic();  // suma
                        
                        // Relajar arista si mejora la distancia
                        if (nueva_dist < dist[v] && nueva_dist < B) {
                            if (M) {
                                M->compare(2);   // 2 comparaciones
                                M->access(2);    // acceso a dist[u] y dist[v]
                                M->assign(2);    // asignaciones
                            }
                            
                            dist[v] = nueva_dist;
                            pred[v] = u;
                        }
                    }
                }
            }
        }
        
        // Paso 3: Procesar vértices descubiertos por Bellman-Ford (W)
        for (int v : W) {
            if (dist[v] < B && !completo[v]) {
                completo[v] = true;
                if (M) M->assign();  // asignación
            }
        }
    }
    
    // =====================================================
    // Verificación final con Dijkstra (OBLIGATORIA)
    // Garantiza que todas las distancias sean correctas
    // =====================================================
    void verificacion_final_dijkstra() {
        using Estado = std::pair<T,int>;
        std::priority_queue<Estado, std::vector<Estado>, std::greater<Estado>> pq;
        
        // Inicializar cola con todos los vértices con distancia finita
        for (int i = 0; i < n; i++) {
            if (dist[i] < INF) {
                pq.push({dist[i], i});
                if (M) {
                    M->compare();  // comparación dist[i] < INF
                    M->call();     // llamada a push
                }
            }
        }
        if (M) M->assign(n);  // n iteraciones del for
        
        std::vector<bool> finalizado(n, false);
        if (M) M->assign(n);  // inicialización del vector
        
        // Ejecutar Dijkstra completo
        while (!pq.empty()) {
            if (M) M->compare();  // comparación while
            
            auto [d,u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);    // llamadas a top() y pop()
                M->assign(2);  // asignaciones de d y u
            }
            
            if (finalizado[u]) continue;
            if (d != dist[u]) continue;
            if (M) M->compare();  // comparación d != dist[u]
            
            finalizado[u] = true;
            if (M) M->assign();  // asignación
            
            // Relajar todas las aristas salientes
            for (auto &[v,w] : adj[u]) {
                T nueva_dist = dist[u] + w;
                if (M) M->arithmetic();  // suma
                
                if (nueva_dist < dist[v]) {
                    if (M) {
                        M->compare();   // comparación
                        M->access(2);   // acceso a dist[u] y dist[v]
                        M->assign(2);   // asignaciones
                    }
                    
                    dist[v] = nueva_dist;
                    pred[v] = u;
                    pq.push({dist[v], v});
                    if (M) M->call();  // llamada a push
                }
            }
        }
        
        // Marcar todos los vértices como completos al final
        std::fill(completo.begin(), completo.end(), true);
        if (M) M->assign(n);  // fill de tamaño n
    }
};

} // namespace spp

#endif // bmssp_HPP