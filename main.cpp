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

// Definicion de informacion necesaria para datos ingresados
struct Colonia{
    int id;
    string nombre;
    int x, y;
    bool esCentral;
};

/*  --- Estructura de Grafo ---
 * => indiceColonias = hash para encontrar el indice 
 * => colonias = hash con datos de colonias
 * => edges = conexiones
 * 
 */
struct Graph {
    int n, m;
    unordered_map<string, Colonia> colonias;
    unordered_map<string, int> indiceColonias;
    unordered_map<int, string> stringColonias;
    vector<int> arregloCentrales;  
    vector<int> arregloNoCentrales;
    vector<pair<int, pair<int, int>>> edges;
    vector<pair<int,int>> selectedEdges;

     // Variables para algoritmos
    vector<int> parent;              // Para Union-Find (Kruskal)
    vector<vector<int>> distMatrix;  // Para Floyd-Warshall
    // Dentro de cada "celda" es el mejor path
    vector<vector<int>> succesorMatrix; // Helper matrix para encontrar el path en Floyd-Warshall

    Graph(int _n, int _m){
        this-> n = _n;
        this-> m  = _m;
        parent.resize(_n);
        distMatrix.assign(_n, vector<int>(_n, INF));
        succesorMatrix.assign(_n, vector<int>(_n, INF));
    }
    
    /* u = salida del arco
	 * v = llegada del arco
	 * w = costo del arco
     * 
     */ // Funcion para agregar una conexion al grafo
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
        // Inicializacion de conexiones en la matriz helper del path
        succesorMatrix[idx_v][idx_u] = idx_u;
        succesorMatrix[idx_u][idx_v] = idx_v; 
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
            stringColonias[id] = _nombre;
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
        cout << "--------------------------------------------------------" << endl;
        cout << "       1   Cableado óptimo de nueva fibra óptica" << endl;
        cout << "--------------------------------------------------------" << endl;

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
        
        cout << "   -Costo total del cableado: " << costoTotal << endl << endl;
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
                        distMatrix[i][j] = min(distMatrix[i][j], distMatrix[i][k] + distMatrix[k][j]);
                        succesorMatrix[i][j] = succesorMatrix[i][k];

                    }
                }
            }
        }
    } 

    void definirCentrales(){
        // Encontrams que son y que no centrales
        for (const auto& it : colonias){
            auto encontrar = indiceColonias.find(it.first);
            int indice =  encontrar->second;
            if (it.second.esCentral) {
                arregloCentrales.push_back(indice);
            }
            else{
                arregloNoCentrales.push_back(indice);
            }
        }
    }

    void displayPath(int origen, int destino){
        /*
        La idea es utilizar successorMatrix para poder desplegar el path
        */
       int actual = origen;
       if (distMatrix[origen][destino] == INF){
            cout << "No hay ruta";
            return;
       } 
       cout << actual << " -> "; 
        while(actual != origen){
            cout << actual;
            actual = succesorMatrix[actual][destino];
            if (actual != origen){
                cout << " -> "; 
            }
        }
    }

    void shortestPath(){
        int iteraciones = 0;
        int costo_total = 0;
        // se definen que tipo de colonia se va a utilizar
        const vector<int> &visitarColonias = arregloNoCentrales;
        cout << "--------------------------------------------------------" << endl;
        cout << " 2  Ruta más corta para visitar cada colonia NO central" << endl;
        cout << "--------------------------------------------------------" << endl;
        vector<int> ruta;
        unordered_map<int, bool> visitadas;
        int inicio = 0;
        int actual = inicio; 
        ruta.push_back(actual);
        visitadas[actual] = true; 
       for(int i = 0; i < visitarColonias.size()-1;     i++){
            int coloniaCercana = -1; // cual es la colonia mas cercana a visitar en indice
            int distMinima = INF;   // cual es su distancia en "costo"
            for(int it_colonia : visitarColonias){
                // Ya visitamos esta colonia?
                if(visitadas.find(it_colonia) == visitadas.end()){
                    if(distMatrix[actual][it_colonia] < distMinima){
                        distMinima = distMatrix[actual][it_colonia]; 
                        coloniaCercana = it_colonia; 
                    }
                }
                iteraciones++;
                cout << iteraciones << endl;
            }
            if (coloniaCercana != -1){
                costo_total += distMinima;
                actual = coloniaCercana;
                ruta.push_back(actual);
                visitadas[actual] = true;
            }
        }
        costo_total += distMatrix[actual][inicio];
        actual = inicio;
        while (actual + 1 < ruta.size()){
            displayPath(actual, actual + 1); 
        }
        
    }

     // ============================================
    // ALGORITMO 4: DISTANCIA EUCLIDIANA
    // ============================================
    /* Explicación:
     * - Para cada nueva colonia, encuentra la colonia existente más cercana
     * - Usa la fórmula de distancia euclidiana: sqrt((x2-x1)² + (y2-y1)²)
     * - Complejidad: O(n) por cada nueva colonia
     */
    void coloniaMasCercana(string nombreNueva, int x_nueva, int y_nueva){
        double distanciaMinima = INF;
        string coloniaCercana = "";
        int x_cercana = 0, y_cercana = 0;
        
        // Revisar cada colonia existente
        for (auto& col : colonias){
            // Calcular distancia euclidiana
            double distancia = sqrt(pow(col.second.x - x_nueva, 2) + 
                                   pow(col.second.y - y_nueva, 2));
            
            // Si es la más cercana hasta ahora, guardarla
            if (distancia < distanciaMinima){
                distanciaMinima = distancia;
                coloniaCercana = col.second.nombre;
                x_cercana = col.second.x;
                y_cercana = col.second.y;
            }
        }
        
        // Imprimir resultado
        cout << nombreNueva << " debe conectarse con " << coloniaCercana << endl;
        cout << "  Coordenadas: (" << x_nueva << ", " << y_nueva 
             << ") -> (" << x_cercana << ", " << y_cercana << ")" << endl;
    }

    

};

int main (){
    // Lectura de valores
    int n, m, k, q;
    cin >> n >> m >> k >> q;
    Graph g(n, m);
    int x, y, central, costo; 
    string nombre1, nombre2;

    // LLenar la información de las colonias.
    for(int i = 0; i < n; i++){
        cin >> nombre1 >> x >> y >> central;
        g.addColoniaInMap(nombre1, x, y, central);
    }

    // Agregar las conexiones entre colonias
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

    /* --- PROBLEMA 1 --- 
     * Se despliega la forma óptima de cablear la nueva fibra óptica compartiendo
     * informción de el menor costo posible. 
     * Se utiliza Kruskal
     */
    g.kruskal(cableadoNuevo);
    /* --- PROBLEMA 1 --- 
     * Se busca encontrar una forma eficiente de visitar todas las colonias que NO SON CENTRALES
     * El algoritmo requiere de la incializacion de FloydWarshall para conocer los costos minimos y
     * conocer cuales son las rutas para estos caminos minimos.
     * Se llama definir centrales ya que es el que envia el arreglo con las centrales a recorrer
     */
    g.floydWarshall();
    g.definirCentrales();
    g.shortestPath();
    return 0;
}