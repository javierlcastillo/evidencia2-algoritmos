// Actividad intregradora 2
// Autores: Javier Luis Castillo Solórzano y Diego García González
// Matrículas: A01658415 A01198976

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <set>

using namespace std;
#define INF 999999  // Definir INF para usar en Floyd-Warshall

struct Colonia{
    int id;
    string nombre;
    int x, y;
    bool esCentral;
};

struct Graph {
    int n, m;
    unordered_map<string, Colonia> colonias;
    unordered_map<string, int> indiceColonias;  
    vector<pair<int, pair<int, int>>> edges;
    vector<pair<int,int>> selectedEdges;

     // Variables para algoritmos
    vector<int> parent;              // Para Union-Find (Kruskal)
    vector<vector<int>> distMatrix;  // Para Floyd-Warshall

    Graph(int _n, int _m){
        this-> n = _n;
        this-> m  = _m;
        parent.resize(_n);
        distMatrix.assign(_n, vector<int>(_n, INF));
    }

    // u = salida del arco
	// v = llegada del arco
	// w = costo del arco

	void addEdge(string u, string v, int w){ // -> u vetice1, v vertice2, w costo
        auto it_u = indiceColonias.find(u);
        auto it_v = indiceColonias.find(v);
        if (it_u == indiceColonias.end() || it_v == indiceColonias.end()){
            cout << "Colonia a conectar no encontrada" << endl; 
            return;
        } 
		
        edges.push_back({w, {it_u->second, it_v->second}}); // -> utilizado en kruskal: first = costo, second = conexión

        // Actualizar matriz de distancias
        int idx_u = it_u->second;
        int idx_v = it_v->second;
        distMatrix[idx_u][idx_v] = min(distMatrix[idx_u][idx_v], w);
        distMatrix[idx_v][idx_u] = min(distMatrix[idx_v][idx_u], w);
    }

    /* --- Manejo de colonias en <unoredered_map> ---
     * ----------------------------------------------
     * Se verifica que la colonia no exista para después asignarle un id numerico
     * para que sea utilizado como indice en kruskal. 
     * 
    */
    void addColoniaInMap(string _nombre, int _x, int _y, int _esCentral){
        auto it = colonias.find(_nombre);
        if (it == colonias.end()){
            Colonia temp;
            int id = colonias.size();
            indiceColonias[_nombre] = id;
            temp.id = id;
            temp.x = _x;
            temp.y = _y;
            temp.esCentral = _esCentral;
            temp.nombre = _nombre; 
            colonias[_nombre] = temp;
            distMatrix[id][id] = 0;  // Distancia a sí mismo = 0
        }
    }
     
    // ============================================
    // UNION-FIND (para Kruskal)
    // ============================================
    /* Encuentra el representante (raíz) del conjunto al que pertenece i
     * Usa path compression para optimizar futuras búsquedas
     */
    int find(int i){
        if (parent[i] != i)
            parent[i] = find(parent[i]);
        return parent[i];
    }
    
    void unionSet(int i, int j){
         int root_i = find(i);
        int root_j = find(j);
        parent[root_i] = root_j;  // Hacer que la raíz de i apunte a la raíz de j
    }
    
    // 1. KRUSKAL 
    void kruskal(vector<pair<string, string>>& cableadoNuevo){
        cout << "-------------------" << endl;
        cout << "1 - Cableado óptimo de nueva fibra." << endl;
        
        // Marcar aristas nuevas
        set<pair<int, int>> nuevas;
        for (auto& e : cableadoNuevo){
            int u = indiceColonias[e.first];
            int v = indiceColonias[e.second];
            nuevas.insert({min(u, v), max(u, v)});
        }
        
        // Ordenar por costo
        sort(edges.begin(), edges.end());
        
        // Inicializar parent
        for (int i = 0; i < n; i++) parent[i] = i;
        
        int costoTotal = 0;
        
        for (auto& edge : edges){
            int costo = edge.first;
            int u = edge.second.first;
            int v = edge.second.second;
            
            if (find(u) != find(v)){
                unionSet(u, v);
                
                // Buscar nombres
                string nombre_u, nombre_v;
                for (auto& col : colonias){
                    if (col.second.id == u) nombre_u = col.second.nombre;
                    if (col.second.id == v) nombre_v = col.second.nombre;
                }
                
                cout << nombre_u << " - " << nombre_v << endl;
                
                // Solo sumar si no es nueva
                pair<int, int> arista = {min(u, v), max(u, v)};
                if (nuevas.find(arista) == nuevas.end()){
                    costoTotal += costo;
                }
            }
        }
        
        cout << "Costo total: " << costoTotal << endl << endl;
    }
    
// ============================================
    // ALGORITMO 2: FLOYD-WARSHALL
    // ============================================
    /* Explicación:
     * - Calcula el camino más corto entre TODOS los pares de nodos
     * - Complejidad: O(n³) pero muy simple de implementar
     * 
     * Idea: Para cada nodo k, pregunta:
     * "¿Es más corto ir de i a j pasando por k?"
     * Si sí, actualiza dist[i][j] = dist[i][k] + dist[k][j]
     */
    void floydWarshall(){
        // Triple bucle
        for (int k = 0; k < n; k++){           // k = intermediario
            for (int i = 0; i < n; i++){       // i = origen
                for (int j = 0; j < n; j++){   // j = destino
                    if (distMatrix[i][k] != INF && distMatrix[k][j] != INF){
                        // ¿Es mejor ir i->k->j que i->j directo?
                        distMatrix[i][j] = min(distMatrix[i][j], 
                                              distMatrix[i][k] + distMatrix[k][j]);
                    }
                }
            }
        }
    } 
};

int main (){

    int n, m, k, q;
    cin >> n >> m >> k >> q;

    Graph g(n, m);

    int x, y, central, costo; 
    string nombre1, nombre2;

    for(int i = 0; i < n; i++){

        cin >> nombre1 >> x >> y >> central;
        g.addColoniaInMap(nombre1, x, y, central);

    }

    for(int i = 0; i < m; i++){

        cin >> nombre1 >> nombre2 >> costo;
        g.addEdge(nombre1, nombre2, costo);

    }

     // Leer k conexiones de cableado nuevo
    vector<pair<string, string>> cableadoNuevo;
    for(int i = 0; i < k; i++){
        cin >> nombre1 >> nombre2;
        cableadoNuevo.push_back({nombre1, nombre2});
    }
    
    // Ejecutar Kruskal para encontrar MST
    g.kruskal(cableadoNuevo);

    return 0;
}